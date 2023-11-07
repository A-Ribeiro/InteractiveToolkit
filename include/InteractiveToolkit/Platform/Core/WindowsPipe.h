#pragma once

#if !defined(_WIN32)
#error ERROR: Including WindowsPipe in a Non-Windows OS
#endif

#include "../platform_common.h"
#include "ObjectBuffer.h"

#define WIN_INVALID_FD NULL
// #define PIPE_READ_FD 0
// #define PIPE_WRITE_FD 1

namespace Platform
{

    class WindowsPipe
    {
        bool read_is_blocking;

        bool read_signaled;
        bool write_signaled;

        HANDLE read_event;
        OVERLAPPED overlapped;

        static std::string randomNamedPipeName()
        {
            static Platform::Mutex mutex;
            static uint64_t uid = 0;
            Platform::AutoLock autoLock(&mutex);
            char aux[64];
            sprintf(aux, "\\\\.\\pipe\\%" PRIu64, uid++);
            return aux;
        }

    public:
        union
        {
            int fd[2];
            struct
            {
                HANDLE read_fd;
                HANDLE write_fd;
            };
        };

        WindowsPipe()
        {
            read_is_blocking = true;
            read_signaled = false;
            write_signaled = false;
            read_fd = WIN_INVALID_FD;
            write_fd = WIN_INVALID_FD;

            read_event = NULL;

            read_event = CreateEvent(0, TRUE, FALSE, NULL);
            ITK_ABORT(read_event == NULL, "Error to create Event. Message: %s", ITKPlatformUtil::win32_GetLastErrorToString().c_str());

            ZeroMemory(&overlapped, sizeof(OVERLAPPED));
            overlapped.hEvent = read_event;

            SECURITY_DESCRIPTOR sd;
            InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
            SetSecurityDescriptorDacl(&sd, TRUE, (PACL)0, FALSE);

            SECURITY_ATTRIBUTES sa;
            sa.nLength = sizeof(sa);
            sa.lpSecurityDescriptor = &sd;
            sa.bInheritHandle = TRUE;

            // bool bool_result = CreatePipe(&read_fd, &write_fd, &sa, 0);

            // LPCTSTR lpszPipename = TEXT("\\\\.\\pipe\\mynamedpipe");

            std::string named_pipe_name = WindowsPipe::randomNamedPipeName();

            read_fd = CreateNamedPipe(
                named_pipe_name.c_str(),
                PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED, // read/write access //PIPE_ACCESS_DUPLEX | PIPE_ACCESS_OUTBOUND
                PIPE_TYPE_BYTE |                            // PIPE_TYPE_MESSAGE |       // message type pipe
                    PIPE_READMODE_BYTE |                    // PIPE_READMODE_MESSAGE |   // message-read mode
                    PIPE_WAIT,                              // blocking mode
                PIPE_UNLIMITED_INSTANCES,                   // max. instances
                512,                                        // output buffer size
                512,                                        // input buffer size
                0,                                          // client time-out
                &sa);                                       // default security attribute

            ITK_ABORT(read_fd == INVALID_HANDLE_VALUE, "%s", ITKPlatformUtil::win32_GetLastErrorToString().c_str());

            write_fd = ::CreateFile(
                named_pipe_name.c_str(),
                GENERIC_WRITE,
                0,
                &sa,
                OPEN_EXISTING,
                FILE_FLAG_OVERLAPPED,
                NULL);

            ITK_ABORT(write_fd == INVALID_HANDLE_VALUE, "%s", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
        }

        ~WindowsPipe()
        {
            close();
        }

        size_t write(const void *buffer, size_t size)
        {
            if (write_fd == WIN_INVALID_FD || write_signaled)
                return 0;

            // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-writefile
            // WriteFile
            DWORD written = 0;

            bool bool_result = ::WriteFile(write_fd, buffer, (DWORD)size, &written, NULL);

            // can have another returns
            if (!bool_result)
            {
                // if (errno == EAGAIN)
                //{
                //     // non blocking
                // }
                // else
                {
                    // any other error
                    printf("[WindowsPipe] write ERROR: %s", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
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
            if (read_fd == WIN_INVALID_FD || read_signaled)
                return false;
            if (outputBuffer->size == 0)
                outputBuffer->setSize(64 * 1024); // 64k

            // https://stackoverflow.com/questions/42402673/createprocess-and-capture-stdout

            // https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-readfile
            // ReadFile

            Platform::Thread *currentThread = Platform::Thread::getCurrentThread();

            DWORD received = 0;
            bool bool_result = ::ReadFile(read_fd, outputBuffer->data, outputBuffer->size, &received, &overlapped);
            // ITK_ABORT(!bool_result, "%s", ITKPlatformUtil::win32_GetLastErrorToString().c_str());

            if (bool_result)
            {
                if (received > 0)
                {
                    outputBuffer->setSize(received);
                    return true;
                }
                else if (received == 0)
                {
                    outputBuffer->setSize(0);
                    // EOF
                    closeReadFD();
                    return false;
                }
                else
                {
                    // any other error
                    printf("[WindowsPipe] read ERROR: unkown error\n");
                    read_signaled = true;
                }
            }

            // bool result is false from here
            DWORD dwError = GetLastError();

            bool continue_waiting = true;

            while (continue_waiting)
            {

                continue_waiting = false;

                switch (dwError)
                {
                case ERROR_BROKEN_PIPE:
                {
                    outputBuffer->setSize(0);
                    // EOF
                    closeReadFD();
                    return false;
                }
                case ERROR_HANDLE_EOF:
                {
                    outputBuffer->setSize(0);
                    // EOF
                    closeReadFD();
                    return false;
                }
                case ERROR_IO_INCOMPLETE:
                {
                    printf("[WindowsPipe] I/O incomplete.\n");
                    continue_waiting = true;
                    break;
                }
                case ERROR_IO_PENDING:
                {
                    // need to wait the overlapped event
                    DWORD dwWaitResult;
                    HANDLE handles_threadInterrupt_sem[2] = {
                        read_event,                             // WAIT_OBJECT_0 + 0
                        currentThread->m_thread_interrupt_event // WAIT_OBJECT_0 + 1
                    };

                    DWORD dwWaitTime = INFINITE;
                    if (!read_is_blocking)
                        dwWaitTime = 0;

                    dwWaitResult = WaitForMultipleObjects(
                        2,                           // number of handles in array
                        handles_threadInterrupt_sem, // array of thread handles
                        FALSE,                       // wait until all are signaled
                        dwWaitTime                   // INFINITE //INFINITE
                    );

                    if (dwWaitResult == WAIT_TIMEOUT)
                    {
                        // NOT EOF, the wait gives timeout... trying to wait again
                        printf("[WindowsPipe] Timeout...\n");
                        CancelIo(read_fd);
                        {
                            // check if have any already read data after call to cancelIO
                            bool_result = GetOverlappedResult(read_fd, &overlapped, &received, FALSE);
                            if (bool_result)
                            {
                                ResetEvent(read_event);
                                outputBuffer->setSize(received);
                                return (received > 0);
                            }
                        }
                        outputBuffer->setSize(0);
                        return false;
                    }
                    else if (dwWaitResult == WAIT_OBJECT_0 + 1)
                    {
                        // true if the interrupt is signaled (and only the interrupt...)
                        // Thread Interrupted
                        printf("thread interrupted...\n");
                        CancelIo(read_fd);
                        {
                            // check if have any already read data after call to cancelIO
                            bool_result = GetOverlappedResult(read_fd, &overlapped, &received, FALSE);
                            if (bool_result)
                            {
                                ResetEvent(read_event);
                                outputBuffer->setSize(received);
                                return true;
                            }
                        }
                        outputBuffer->setSize(0);
                        return false;
                    }
                    else if (dwWaitResult == WAIT_OBJECT_0 + 0)
                    {
                        // true if the read signaled the IO event...
                        bool_result = GetOverlappedResult(read_fd, &overlapped, &received, FALSE);
                        if (bool_result)
                        {
                            ResetEvent(read_event);
                            outputBuffer->setSize(received);
                            return true;
                        }
                        else
                        {
                            // forward the error to the next iteration...
                            dwError = GetLastError();
                            continue_waiting = true;
                        }
                    }
                    break;
                }

                default:
                {
                    printf("[WindowsPipe] read UNHANDLED ERROR: %s", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
                    break;
                }
                }
            }

            // if (received > 0)
            //{
            //     outputBuffer->setSize(received);
            //     return true;
            // }
            // outputBuffer->setSize(0);
            // if (received == 0)
            //{
            //     // EOF
            //     closeReadFD();
            //     return false;
            // }

            //// can have another returns
            // if (!bool_result)
            //{
            //     //if (errno == EAGAIN)
            //     //{
            //     //    // non blocking
            //     //    return false;
            //     //}
            //     //else
            //     {
            //         // any other error
            //         printf("[WindowsPipe] read ERROR: %s", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
            //         read_signaled = true;
            //     }
            // }

            return false;
        }

        bool isReadSignaled()
        {
            return read_fd == WIN_INVALID_FD || read_signaled || Platform::Thread::isCurrentThreadInterrupted();
        }

        bool isWriteSignaled()
        {
            return write_fd == WIN_INVALID_FD || write_signaled || Platform::Thread::isCurrentThreadInterrupted();
        }

        bool isReadBlocking()
        {
            /*auto flags = fcntl(read_fd, F_GETFL);
            return (flags & O_NONBLOCK) != 0;*/
            return read_is_blocking;
        }

        bool isWriteBlocking()
        {
            /*auto flags = fcntl(write_fd, F_GETFL);
            return (flags & O_NONBLOCK) != 0;*/
            return true;
        }

        void setReadBlocking(bool v)
        {
            /*auto flags = fcntl(read_fd, F_GETFL);
            fcntl(read_fd, F_SETFL, (v) ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK));*/
            read_is_blocking = v;
        }

        void setWriteBlocking(bool v)
        {
            /*auto flags = fcntl(write_fd, F_GETFL);
            fcntl(write_fd, F_SETFL, (v) ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK));*/
        }

        bool isReadFDClosed()
        {
            return read_fd == WIN_INVALID_FD;
        }

        bool isWriteFDClosed()
        {
            return write_fd == WIN_INVALID_FD;
        }

        void closeReadFD()
        {
            if (read_fd == WIN_INVALID_FD)
                return;
            CloseHandle(read_fd);
            read_fd = WIN_INVALID_FD;
        }

        void closeWriteFD()
        {
            if (write_fd == WIN_INVALID_FD)
                return;
            CloseHandle(write_fd);
            write_fd = WIN_INVALID_FD;
        }

        void close()
        {
            closeReadFD();
            closeWriteFD();

            if (read_event != NULL)
            {
                CloseHandle(read_event);
                read_event = NULL;
            }
        }
    };

}
