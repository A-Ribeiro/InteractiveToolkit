#pragma once

#if !defined(__linux__) && !defined(__APPLE__)
#error ERROR: Including WindowsPipe in a Non-Windows OS
#endif

#include "../platform_common.h"
#include "ObjectBuffer.h"
#include "../Thread.h"

#define INVALID_FD -1
#define PIPE_READ_FD 0
#define PIPE_WRITE_FD 1

namespace Platform
{

    // STDOUT_FILENO | STDERR_FILENO
    void SinkStdFD(int fd)
    {
        if (fd == STDOUT_FILENO | fd == STDERR_FILENO)
        {
            int o_fd = open("/dev/null", O_WRONLY);
            if (o_fd != -1)
            {
                dup2(o_fd, fd);
                close(o_fd);
            }
            else
            {
                perror("SinkStdFD STDOUT/STDERR...\n");
            }
        }
        else if (fd == STDIN_FILENO)
        {
            int o_fd = open("/dev/null", O_RDONLY);
            if (o_fd != -1)
            {
                dup2(o_fd, fd);
                close(o_fd);
            }
            else
            {
                perror("SinkStdFD STDIN...\n");
            }
        }
    }

    class UnixPipe
    {

        bool read_signaled;
        bool write_signaled;

    public:
        union
        {
            int fd[2];
            struct
            {
                int read_fd;
                int write_fd;
            };
        };

        UnixPipe()
        {
            read_signaled = false;
            write_signaled = false;
            read_fd = INVALID_FD;
            write_fd = INVALID_FD;
            ITK_ABORT(pipe(fd) != 0, "UnixPipe ERROR: %s", strerror(errno));
        }

        ~UnixPipe()
        {
            close();
        }

        size_t write(const void *buffer, size_t size)
        {
            if (write_fd == INVALID_FD || write_signaled)
                return 0;

            ssize_t written = ::write(write_fd, buffer, size);

            // can have another returns
            if (written == -1)
            {
                if (errno == EAGAIN)
                {
                    // non blocking
                }
                else
                {
                    // any other error
                    printf("[UnixPipe] write ERROR: %s", strerror(errno));
                    write_signaled = true;
                }
                return 0;
            }

            return (size_t)written;
        }

        // returns true if read something...
        // false the other case ...
        // need to call isSignaled if receive false
        bool read(Platform::ObjectBuffer *outputBuffer)
        {
            if (read_fd == INVALID_FD || read_signaled)
                return false;
            if (outputBuffer->size == 0)
                outputBuffer->setSize(64 * 1024); // 64k

            ssize_t received = ::read(read_fd, outputBuffer->data, outputBuffer->size);
            if (received > 0)
            {
                outputBuffer->setSize(received);
                return true;
            }
            outputBuffer->setSize(0);
            if (received == 0)
            {
                // EOF
                closeReadFD();
                return false;
            }

            // can have another returns
            if (received == -1)
            {
                if (errno == EAGAIN)
                {
                    // non blocking
                    return false;
                }
                else
                {
                    // any other error
                    printf("[UnixPipe] read ERROR: %s", strerror(errno));
                    read_signaled = true;
                }
            }

            return false;
        }

        bool isReadSignaled()
        {
            return read_fd == INVALID_FD || read_signaled || Platform::Thread::isCurrentThreadInterrupted();
        }

        bool isWriteSignaled()
        {
            return write_fd == INVALID_FD || write_signaled || Platform::Thread::isCurrentThreadInterrupted();
        }

        // STDIN_FILENO
        void aliasReadAs(int fd = STDIN_FILENO)
        {
            dup2(read_fd, fd);
        }

        // STDOUT_FILENO | STDERR_FILENO
        void aliasWriteAs(int fd = STDOUT_FILENO)
        {
            dup2(write_fd, fd);
        }

        bool isReadBlocking()
        {
            auto flags = fcntl(read_fd, F_GETFL);
            return (flags & O_NONBLOCK) != 0;
        }

        bool isWriteBlocking()
        {
            auto flags = fcntl(write_fd, F_GETFL);
            return (flags & O_NONBLOCK) != 0;
        }

        void setReadBlocking(bool v)
        {
            auto flags = fcntl(read_fd, F_GETFL);
            fcntl(read_fd, F_SETFL, (v) ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK));
        }

        void setWriteBlocking(bool v)
        {
            auto flags = fcntl(write_fd, F_GETFL);
            fcntl(write_fd, F_SETFL, (v) ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK));
        }

        bool isReadFDClosed()
        {
            return read_fd == INVALID_FD;
        }

        bool isWriteFDClosed()
        {
            return write_fd == INVALID_FD;
        }

        void closeReadFD()
        {
            if (read_fd == INVALID_FD)
                return;
            ::close(read_fd);
            read_fd = INVALID_FD;
        }

        void closeWriteFD()
        {
            if (write_fd == INVALID_FD)
                return;
            ::close(write_fd);
            write_fd = INVALID_FD;
        }

        void close()
        {
            closeReadFD();
            closeWriteFD();
        }
    };

}
