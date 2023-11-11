#pragma once

#include "platform_common.h"

#include "Mutex.h"
#include "Semaphore.h"
#include "Sleep.h"
#include "AutoLock.h"
#include "Core/NetworkConstants.h"
#include "Core/SocketUtils.h"


namespace Platform {

    class SocketTCPAccept;

    class SocketTCP {

        //private copy constructores, to avoid copy...
        SocketTCP(const SocketTCP& v) :read_semaphore(1), write_semaphore(1) {}
        void operator=(const SocketTCP& v) {}

#if defined(_WIN32)
        SOCKET fd;
#else
        int fd;
#endif
        struct sockaddr_in addr_in;
        struct sockaddr_in addr_out;

        bool signaled;

        uint32_t read_timeout_ms;
        uint32_t write_timeout_ms;

#if defined(_WIN32)
        HANDLE wsa_read_event;
#endif


#if defined(_WIN32)
        void initializeWithNewConnection(const SOCKET &fd, const struct sockaddr_in& addr_in) {
#else
        void initializeWithNewConnection(int fd, const struct sockaddr_in& _addr_in) {
#endif
        
            Platform::AutoLock auto_lock(&mutex);

            bool read_aquired = read_semaphore.blockingAcquire();
            bool write_aquired = write_semaphore.blockingAcquire();

            ITK_ABORT(this->fd != ITK_INVALID_SOCKET, "Cannot initialize a new connection with an already initialized socked.\n");

            this->fd = fd;
            //this->addr_in = _addr_in;
            this->addr_out = _addr_in;

            read_timeout_ms = 0xffffffff;//INFINITE;
            write_timeout_ms = 0xffffffff;//INFINITE;

#if defined(_WIN32)
            wsa_read_event = WSACreateEvent();
            ITK_ABORT(wsa_read_event == WSA_INVALID_EVENT, "Error to create WSAEvent. Message: %s", SocketUtils::getLastSocketErrorMessage().c_str());

            ITK_ABORT(
                ::WSAEventSelect(fd, wsa_read_event, FD_READ | FD_CLOSE) == SOCKET_ERROR,// FD_READ | FD_WRITE
                "WSAEventSelect error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str()
            );
#endif

            signaled = false;

            // get ephemeral port info
            socklen_t len = sizeof(struct sockaddr_in);
            getsockname(fd, (sockaddr *)&addr_in, &len);

            if (read_aquired) read_semaphore.release();
            if (write_aquired) write_semaphore.release();

            // print stats
            printf("[SocketTCP] Connected\n");
            printf("              in_addr: %s:%u\n", inet_ntoa(addr_in.sin_addr),ntohs(addr_in.sin_port));
            printf("              out_addr: %s:%u\n", inet_ntoa(addr_out.sin_addr),ntohs(addr_out.sin_port));
        }

        Platform::Mutex mutex;

    public:

        void setTTL(int ttl) {
            Platform::AutoLock auto_lock(&mutex);
            ITK_ABORT(this->fd == ITK_INVALID_SOCKET, "Socket not initialized.\n");

            //set default ttl
            if (ttl == -1)
                ttl = 64;

            ITK_ABORT(
                ::setsockopt(fd, IPPROTO_IP, IP_TTL, (char *)&(ttl), sizeof(int)) == -1,
                "setsockopt SO_REUSEADDR error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str()
            );
        }

        void setKeepAlive(bool keepAlive) {
            Platform::AutoLock auto_lock(&mutex);
            int aux;
            aux = (keepAlive) ? 1 : 0;
            ITK_ABORT(
                ::setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (char *)&aux, sizeof(int)) == -1,
                "setsockopt SO_KEEPALIVE error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str()
            );
        }

        void setNoDelay(bool noDelay) {
            Platform::AutoLock auto_lock(&mutex);
            int aux;
            aux = (noDelay) ? 1 : 0;
            ITK_ABORT(
                ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&aux, sizeof(int)) == -1,
                "setsockopt TCP_NODELAY error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str()
            );
        }

        void setWriteTimeout(uint32_t timeout_ms) {
            write_timeout_ms = timeout_ms;
            Platform::AutoLock auto_lock(&mutex);
            struct timeval timeout;
            timeout.tv_sec = timeout_ms / 1000;
            timeout.tv_usec = (timeout_ms % 1000) * 1000;
            ITK_ABORT(
                ::setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(struct timeval)) == -1,
                "setsockopt SO_SNDTIMEO error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str()
            );
        }

        void setReadTimeout(uint32_t timeout_ms) {
            read_timeout_ms = timeout_ms;
            Platform::AutoLock auto_lock(&mutex);
            struct timeval timeout;
            timeout.tv_sec = timeout_ms / 1000;
            timeout.tv_usec = (timeout_ms % 1000) * 1000;
            ITK_ABORT(
                ::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval)) == -1,
                "setsockopt SO_RCVTIMEO error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str()
            );
        }

        void setBlocking(bool blocking) {
            Platform::AutoLock auto_lock(&mutex);
            ITK_ABORT(this->fd == ITK_INVALID_SOCKET, "Socket not initialized.\n");

            SocketUtils::SetSocketBlockingEnabled(fd, blocking);
        }

        bool connect(const std::string &address_ip, uint16_t port) {
            Platform::AutoLock auto_lock(&mutex);

            bool read_aquired = read_semaphore.blockingAcquire();
            bool write_aquired = write_semaphore.blockingAcquire();

            ITK_ABORT(this->fd != ITK_INVALID_SOCKET, "Cannot initialize a new connection with an already initialized socked.\n");

            fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            ITK_ABORT(fd == ITK_INVALID_SOCKET, "Error to create Socket. Message: %s", SocketUtils::getLastSocketErrorMessage().c_str());

            read_timeout_ms = 0xffffffff;//INFINITE;
            write_timeout_ms = 0xffffffff;//INFINITE;

            addr_out.sin_family = AF_INET;
            if (address_ip.size() == 0 || address_ip.compare("INADDR_ANY") == 0)
                addr_out.sin_addr.s_addr = htonl(INADDR_ANY);
            else if (address_ip.compare("INADDR_LOOPBACK") == 0)
                addr_out.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            else
                addr_out.sin_addr.s_addr = inet_addr(address_ip.c_str());
            addr_out.sin_port = htons(port);


#if defined(_WIN32)

            setBlocking(false);

            HANDLE wsa_connect_event = WSACreateEvent();
            ITK_ABORT(wsa_connect_event == WSA_INVALID_EVENT, "Error to create WSAEvent. Message: %s", SocketUtils::getLastSocketErrorMessage().c_str());

            ITK_ABORT(
                ::WSAEventSelect(fd, wsa_connect_event, FD_CONNECT | FD_CLOSE) == SOCKET_ERROR,// FD_READ | FD_WRITE
                "WSAEventSelect error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str()
            );

            ::connect(fd, (struct sockaddr*)&addr_out, sizeof(struct sockaddr_in));

            bool connected = false;

            Platform::Thread* currentThread = Platform::Thread::getCurrentThread();

            DWORD dwWaitResult;
            HANDLE handles_threadInterrupt_sem[2] = {
                wsa_connect_event, // WAIT_OBJECT_0 + 0
                currentThread->m_thread_interrupt_event // WAIT_OBJECT_0 + 1
            };

            DWORD dwWaitTime = INFINITE;

            /*
            if (read_timeout_ms != 0xffffffff)
                dwWaitTime = read_timeout_ms;
            */

            dwWaitResult = WaitForMultipleObjects(
                2,   // number of handles in array
                handles_threadInterrupt_sem,     // array of thread handles
                FALSE,          // wait until all are signaled
                dwWaitTime // INFINITE //INFINITE
            );

            if (dwWaitResult == WAIT_TIMEOUT) {
                connected = false;
            }
            else
            // true if the interrupt is signaled (and only the interrupt...)
            if (dwWaitResult == WAIT_OBJECT_0 + 1) {
                connected = false;
            }
            else
            // true if the socket is signaled (might have the interrupt or not...)
            if (dwWaitResult == WAIT_OBJECT_0 + 0) {

                WSANETWORKEVENTS NetworkEvents = { 0 };
                //int nReturnCode = WSAWaitForMultipleEvents(1, &lphEvents[0], false, WSA_INFINITE, false);
                //if (nReturnCode==WSA_WAIT_FAILED) 
                    //throw MyException("WSA__WAIT_FAILED.\n");
                ITK_ABORT(
                    WSAEnumNetworkEvents(fd, wsa_connect_event, &NetworkEvents) == SOCKET_ERROR,
                    "WSAEnumNetworkEvents error. %s",
                    SocketUtils::getLastSocketErrorMessage().c_str());

                if (NetworkEvents.lNetworkEvents & FD_CONNECT) {
                    connected = true;
                    int error_code;
                    int error_code_size = sizeof(error_code);
                    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, (char*)&error_code, &error_code_size) == 0) {
                        connected = (error_code == 0);
                    }
                }
                else if (NetworkEvents.lNetworkEvents & FD_CLOSE) {
                    connected = false;
                }
                else {
                    // any other error...
                    connected = false;
                }
            }

            if (wsa_connect_event != NULL) {
                ::WSACloseEvent(wsa_connect_event);
                wsa_connect_event = NULL;
            }

            if (!connected) {


                printf("Failed to connect socket. %s\n", SocketUtils::getLastSocketErrorMessage().c_str());

                signaled = true;

                if (read_aquired) read_semaphore.release();
                if (write_aquired) write_semaphore.release();

                setBlocking(true);
                return false;
            }

            setBlocking(true);
#else

            if (::connect(fd, (struct sockaddr *)&addr_out, sizeof(struct sockaddr_in)) == -1) {
                printf("Failed to connect socket. %s\n", SocketUtils::getLastSocketErrorMessage().c_str());

                signaled = true;

                if (read_aquired) read_semaphore.release();
                if (write_aquired) write_semaphore.release();

                return false;
            }

#endif

#if defined(_WIN32)
            wsa_read_event = WSACreateEvent();
            ITK_ABORT(wsa_read_event == WSA_INVALID_EVENT, "Error to create WSAEvent. Message: %s", SocketUtils::getLastSocketErrorMessage().c_str());

            ITK_ABORT(
                ::WSAEventSelect(fd, wsa_read_event, FD_READ | FD_CLOSE) == SOCKET_ERROR,// FD_READ | FD_WRITE
                "WSAEventSelect error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str()
            );
#endif

            signaled = false;

            // get ephemeral port info
            socklen_t len = sizeof(struct sockaddr_in);
            getsockname(fd, (sockaddr *)&addr_in, &len);

            if (read_aquired) read_semaphore.release();
            if (write_aquired) write_semaphore.release();

            // print stats
            printf("[SocketTCP] Connected\n");
            printf("              in_addr: %s:%u\n", inet_ntoa(addr_in.sin_addr),ntohs(addr_in.sin_port));
            printf("              out_addr: %s:%u\n", inet_ntoa(addr_out.sin_addr),ntohs(addr_out.sin_port));

            return true;
        }

        Platform::Semaphore read_semaphore;//used to read complex data
        Platform::Semaphore write_semaphore;//used to write complex data

        // this write is blocking...
        bool write_buffer(const uint8_t* data, uint32_t size, uint32_t *write_feedback = NULL) {
            if (isSignaled() || fd == ITK_INVALID_SOCKET) {
                if (write_feedback != NULL)
                    *write_feedback = 0;
                return false;
            }
            uint32_t current_pos = 0;
            if (write_feedback != NULL)
                *write_feedback = current_pos;

#if !defined(_WIN32)
            Platform::Thread *currentThread = Platform::Thread::getCurrentThread();
#endif

            while (current_pos < size) {

#if !defined(_WIN32)
                //force count the socket as a semaphore 
                // per thread signal logic
                currentThread->semaphoreLock();
                if (isSignaled()) {
                    //signaled = true;
                    currentThread->semaphoreUnLock();

                    return false;
                }
                else {
                    currentThread->semaphoreWaitBegin(NULL);
                    currentThread->semaphoreUnLock();
#endif

#if defined(_WIN32)
                    int iResult = ::send(fd, (char*)&data[current_pos], size, 0);
#else
                    int iResult = ::send(fd, (char*)&data[current_pos], size, MSG_NOSIGNAL);
#endif

#if !defined(_WIN32)
                    currentThread->semaphoreWaitDone(NULL);
#endif

                    if (iResult > 0) {
                        //received some quantity of bytes...
                        current_pos += iResult;
                        if (write_feedback != NULL)
                            *write_feedback = current_pos;
                    }
                    else if (iResult == 0) {
                        printf("send write 0 bytes (connection closed)...\n");
                        signaled = true;
                        return false;
                    }
#if defined(_WIN32)
                    else if (WSAGetLastError() == WSAEWOULDBLOCK) {
#else
                    else if (errno == EWOULDBLOCK || errno == EAGAIN) {
#endif

                        printf("not blocking write mode... retrying...\n");
                        //signaled = true;
                        //return false;
                        continue;
                    }
                    else {
                        //iResult == SOCKET_ERROR
                        //some error occured...
                        printf("send failed: %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                        signaled = true;
                        return false;
                    }

#if !defined(_WIN32)
                    }
#endif
                }
            return true;
            }

        // this read is blocking...
        bool read_buffer(uint8_t* data, uint32_t size, uint32_t *read_feedback = NULL) {
            if (isSignaled() || fd == ITK_INVALID_SOCKET) {
                if (read_feedback != NULL)
                    *read_feedback = 0;
                return false;
            }

            //printf("Start reading...\n");

            uint32_t current_pos = 0;
            if (read_feedback != NULL)
                *read_feedback = current_pos;

            Platform::Thread *currentThread = Platform::Thread::getCurrentThread();

            while (current_pos < size) {


#if defined(_WIN32)
                DWORD dwWaitResult;
                HANDLE handles_threadInterrupt_sem[2] = {
                    wsa_read_event, // WAIT_OBJECT_0 + 0
                    currentThread->m_thread_interrupt_event // WAIT_OBJECT_0 + 1
                };

                DWORD dwWaitTime = INFINITE;
                
                if (read_timeout_ms != 0xffffffff)
                    dwWaitTime = read_timeout_ms;

                dwWaitResult = WaitForMultipleObjects(
                    2,   // number of handles in array
                    handles_threadInterrupt_sem,     // array of thread handles
                    FALSE,          // wait until all are signaled
                    dwWaitTime // INFINITE //INFINITE
                );

                if (dwWaitResult == WAIT_TIMEOUT) {
                    signaled = true;
                    if (read_feedback != NULL)
                        *read_feedback = 0;
                    return false;
                } else

                // true if the interrupt is signaled (and only the interrupt...)
                if (dwWaitResult == WAIT_OBJECT_0 + 1) {
                    //signaled = true;
                    if (read_feedback != NULL)
                        *read_feedback = 0;
                    return false;
                } else

                // true if the socket is signaled (might have the interrupt or not...)
                if (dwWaitResult == WAIT_OBJECT_0 + 0) {

                    WSANETWORKEVENTS NetworkEvents = { 0 };
                    //int nReturnCode = WSAWaitForMultipleEvents(1, &lphEvents[0], false, WSA_INFINITE, false);
                    //if (nReturnCode==WSA_WAIT_FAILED) 
                        //throw MyException("WSA__WAIT_FAILED.\n");
                    ITK_ABORT(
                        WSAEnumNetworkEvents(fd, wsa_read_event, &NetworkEvents) == SOCKET_ERROR,
                        "WSAEnumNetworkEvents error. %s",
                        SocketUtils::getLastSocketErrorMessage().c_str());

                    //read event...
                    int iResult = recv(fd, (char*)&data[current_pos], size - current_pos, 0);
                    if (iResult > 0) {
                        //received some quantity of bytes...
                        current_pos += iResult;
                        if (read_feedback != NULL)
                            *read_feedback = current_pos;
                    }
                    else if (iResult == 0) {
                        // close connection
                        printf("recv read 0 bytes (connection closed)...\n");
                        signaled = true;
                        return false;
                    }
                    else if (WSAGetLastError() == WSAEWOULDBLOCK) {

                        //printf("not blocking read mode... retrying...\n");
                        //signaled = true;
                        //return false;
                        continue;
                    }
                    else {
                        //some error occured...
                        printf("recv failed: %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                        signaled = true;
                        return false;
                    }
                }
#else

                //force count the socket as a semaphore 
                // per thread signal logic
                currentThread->semaphoreLock();
                if (isSignaled()) {
                    //signaled = true;
                    currentThread->semaphoreUnLock();

                    return false;
                }
                else {
                    currentThread->semaphoreWaitBegin(NULL);
                    currentThread->semaphoreUnLock();

                    int iResult = recv(fd, (char*)&data[current_pos], size - current_pos, 0);

                    currentThread->semaphoreWaitDone(NULL);

                    if (iResult > 0) {
                        //received some quantity of bytes...
                        current_pos += iResult;
                        if (read_feedback != NULL)
                            *read_feedback = current_pos;
                    }
                    else if (iResult == 0) {
                        // close connection
                        printf("recv read 0 bytes (connection closed)...\n");
                        signaled = true;
                        return false;
                    }
                    else if (errno == EWOULDBLOCK || errno == EAGAIN) {

                        //printf("not blocking read mode... retrying...\n");
                        //signaled = true;
                        //return false;
                        continue;
                    }
                    else {
                        //some error occured...
                        printf("recv failed: %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                        signaled = true;
                        return false;
                    }


                }

#endif
            }

            return true;
        }


        bool read_uint8(uint8_t* v, bool blocking = true) {
            if (signaled || fd == ITK_INVALID_SOCKET)
                return false;
            if (blocking)
                return read_buffer(v, 1);

            // non-blocking read 1 uint8_t
            int iResult = recv(fd, (char*)v, 1, MSG_PEEK | MSG_DONTWAIT);
            if (iResult == 1) {
                recv(fd, (char*)v, 1, 0);
                return true;
            }
            else if (iResult == 0) {
                printf("recv read 0 bytes (connection closed)...\n");
                signaled = true;
                return false;
            }
#if defined(_WIN32)
            else if (WSAGetLastError() == WSAEWOULDBLOCK) {
#else
            else if (errno == EWOULDBLOCK || errno == EAGAIN) {
#endif
                // non-blocking result
                return false;
            }
            else {
                //some error occured...
                printf("recv failed: %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                signaled = true;
                return false;
            }

            return false;
            }

        bool isSignaled() const {
            return signaled || Platform::Thread::isCurrentThreadInterrupted();
        }

        bool isClosed() {
            return fd == ITK_INVALID_SOCKET;
        }

        void close() {
            Platform::AutoLock auto_lock(&mutex);

            bool read_aquired = read_semaphore.blockingAcquire();
            bool write_aquired = write_semaphore.blockingAcquire();

            if (fd != ITK_INVALID_SOCKET) {
                printf("PlatformTCPSocket Close...\n");

                ITK_ABORT(
                    ::closesocket(fd) != 0,
                    "closesocket error. %s",
                    SocketUtils::getLastSocketErrorMessage().c_str()
                );

                fd = ITK_INVALID_SOCKET;
            }

#if defined(_WIN32)
            if (wsa_read_event != NULL) {
                ::WSACloseEvent(wsa_read_event);
                wsa_read_event = NULL;
            }
#endif
            if (read_aquired) read_semaphore.release();
            if (write_aquired) write_semaphore.release();
        }

#if defined(_WIN32)
        SOCKET getNativeFD() 
#else
        int getNativeFD()
#endif
        {
            return fd;
        }

        const struct sockaddr_in &getAddrIn() {
            return addr_in;
        }

        // valid only after a successfull connection to a server
        const struct sockaddr_in &getAddrOut() {
            return addr_out;
        }

        SocketTCP() :read_semaphore(1), write_semaphore(1) {
            SocketUtils::Instance()->InitSockets();

            fd = ITK_INVALID_SOCKET;
            
            addr_in.sin_family = AF_INET;
            addr_in.sin_addr.s_addr = htonl(INADDR_NONE);
            addr_in.sin_port = htons(0);

            addr_out.sin_family = AF_INET;
            addr_out.sin_addr.s_addr = htonl(INADDR_NONE);
            addr_out.sin_port = htons(0);

            signaled = false;
#if defined(_WIN32)
            wsa_read_event = WSA_INVALID_EVENT;
#endif
        }

        virtual ~SocketTCP() {
            close();
        }

        friend class SocketTCPAccept;

        };

    class SocketTCPAccept {

#if defined(_WIN32)
        SOCKET fd;
#else
        int fd;
#endif

        Platform::Semaphore semaphore;
        struct sockaddr_in server_addr;

        bool listen;
        bool signaled;

        bool blocking;
        bool reuseAddress;
        bool noDelay;

#if defined(_WIN32)
        HANDLE wsa_accept_event;
#endif

        //private copy constructores, to avoid copy...
        SocketTCPAccept(const SocketTCPAccept& v) :semaphore(1) {}
        void operator=(const SocketTCPAccept& v) {}


        void initialize(bool blocking = true, bool reuseAddress = true, bool noDelay = true) {
            SocketUtils::Instance()->InitSockets();
            this->blocking = blocking;
            this->reuseAddress = reuseAddress;
            this->noDelay = noDelay;

            signaled = false;
#if defined(_WIN32)
            wsa_accept_event = NULL;
#endif

            bool aquired = semaphore.blockingAcquire();

            listen = false;

            fd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            ITK_ABORT(fd == ITK_INVALID_SOCKET, "Error to create Socket. Message: %s", SocketUtils::getLastSocketErrorMessage().c_str());

            SocketUtils::SetSocketBlockingEnabled(fd, blocking);

            int aux;

            aux = (reuseAddress) ? 1 : 0;
            ITK_ABORT(
                ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&aux, sizeof(int)) == -1,
                "setsockopt SO_REUSEADDR error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str()
            );

            aux = (noDelay) ? 1 : 0;
            ITK_ABORT(
                ::setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&aux, sizeof(int)) == -1,
                "setsockopt TCP_NODELAY error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str()
            );

#if defined(_WIN32)
            if (blocking) {
                wsa_accept_event = WSACreateEvent();
                ITK_ABORT(wsa_accept_event == WSA_INVALID_EVENT, "Error to create WSAEvent. Message: %s", SocketUtils::getLastSocketErrorMessage().c_str());

                ITK_ABORT(
                    ::WSAEventSelect(fd, wsa_accept_event, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR,// FD_READ | FD_WRITE
                    "WSAEventSelect error. %s",
                    SocketUtils::getLastSocketErrorMessage().c_str()
                );
            }
#endif

            if (aquired) semaphore.release();
        }

    public:

#if defined(_WIN32)
        SOCKET getNativeFD()
#else
        int getNativeFD() 
#endif
        {
            return fd;
        }

        const struct sockaddr_in &getAddr() {
            return server_addr;
        }

        SocketTCPAccept(bool blocking = true, bool reuseAddress = true, bool noDelay = true) :semaphore(1) {
            initialize(blocking, reuseAddress, noDelay);
        }

        bool isClosed() {
            return fd == ITK_INVALID_SOCKET;
        }

        bool isListening() {
            return listen;
        }

        void close() {
            bool aquired = semaphore.blockingAcquire();

            listen = false;

            if (fd != ITK_INVALID_SOCKET) {
                printf("PlatformTCPAcceptSocket Close...\n");
                ITK_ABORT(
                    ::closesocket(fd) != 0,
                    "closesocket error. %s",
                    SocketUtils::getLastSocketErrorMessage().c_str()
                );

                fd = ITK_INVALID_SOCKET;
            }

#if defined(_WIN32)
            if (wsa_accept_event != NULL) {
                ::WSACloseEvent(wsa_accept_event);
                wsa_accept_event = NULL;
            }
#endif

            if (aquired) semaphore.release();
        }

        virtual ~SocketTCPAccept() {
            close();
        }

        bool bindAndListen(const std::string &address_ip, uint16_t port, int incoming_queue_size = 10) {
            bool aquired = semaphore.blockingAcquire();

            if (listen || fd == ITK_INVALID_SOCKET || !aquired) {
                if (aquired) semaphore.release();
                return false;
            }

            server_addr.sin_family = AF_INET;
            if (address_ip.size() == 0 || address_ip.compare("INADDR_ANY") == 0)
                server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            else if (address_ip.compare("INADDR_LOOPBACK") == 0)
                server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            else
                server_addr.sin_addr.s_addr = inet_addr(address_ip.c_str());
            server_addr.sin_port = htons(port);

            /*
            ITK_ABORT(
                ::bind(fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) == -1,
                "Failed to bind socket. %s",
                SocketUtils::getLastSocketErrorMessage().c_str());
            */
            if (::bind(fd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) == -1) {
                printf("Failed to bind socket. %s", SocketUtils::getLastSocketErrorMessage().c_str());
                if (aquired) semaphore.release();
                return false;
            }

            /*
            ITK_ABORT(
                ::listen(fd, incoming_queue_size), //SOMAXCONN -> 2147483647 -> 0x7FFFFFFF
                "Failed to listen socket. %s",
                SocketUtils::getLastSocketErrorMessage().c_str());
            */

            // get ephemeral port info
            socklen_t len = sizeof(struct sockaddr_in);
            getsockname(fd, (sockaddr *)&server_addr, &len);

            if (::listen(fd, incoming_queue_size) != 0) {
                printf("Failed to listen socket. %s", SocketUtils::getLastSocketErrorMessage().c_str());
                if (aquired) semaphore.release();
                return false;
            }

            listen = true;

            if (aquired) semaphore.release();

            // print stats
            printf("[PlatformTCPAcceptSocket] Bind OK\n");
            printf("          TCP Server Addr: %s\n", inet_ntoa(server_addr.sin_addr));
            printf("                     Port: %u\n", ntohs(server_addr.sin_port));

            return true;
        }

        bool accept(SocketTCP *result) {

            if (blocking && !semaphore.blockingAcquire())
                return false;

            if (!listen || fd == ITK_INVALID_SOCKET) {

                if (blocking)
                    semaphore.release();

                return false;
            }

#if defined(_WIN32)
            //WSAWaitForMultipleEvents
            //WSAEnumNetworkEvents

            if (blocking) {

                Platform::Thread *currentThread = Platform::Thread::getCurrentThread();

                DWORD dwWaitResult;
                HANDLE handles_threadInterrupt_sem[2] = {
                    wsa_accept_event, // WAIT_OBJECT_0 + 0
                    currentThread->m_thread_interrupt_event // WAIT_OBJECT_0 + 1
                };

                dwWaitResult = WaitForMultipleObjects(
                    2,   // number of handles in array
                    handles_threadInterrupt_sem,     // array of thread handles
                    FALSE,          // wait until all are signaled
                    INFINITE //INFINITE
                );

                // true if the interrupt is signaled (and only the interrupt...)
                if (dwWaitResult == WAIT_OBJECT_0 + 1) {
                    //signaled = true;
                    semaphore.release();
                    return false;
                }

                // true if the socket is signaled (might have the interrupt or not...)
                if (dwWaitResult == WAIT_OBJECT_0 + 0) {

                    /*
                    ITK_ABORT(
                        ::WSAResetEvent(wsa_accept_event) != TRUE,
                        "WSAResetEvent error. %s",
                        SocketUtils::getLastSocketErrorMessage().c_str());
                        */

                    WSANETWORKEVENTS NetworkEvents = { 0 };
                    //int nReturnCode = WSAWaitForMultipleEvents(1, &lphEvents[0], false, WSA_INFINITE, false);
                    //if (nReturnCode==WSA_WAIT_FAILED) 
                        //throw MyException("WSA__WAIT_FAILED.\n");
                    ITK_ABORT(
                        WSAEnumNetworkEvents(fd, wsa_accept_event, &NetworkEvents) == SOCKET_ERROR,
                        "WSAEnumNetworkEvents error. %s",
                        SocketUtils::getLastSocketErrorMessage().c_str());

                    //read event...
                    //printf("ACCEPT EVENT DETECTED!!!\n");

                    struct sockaddr_in client_addr;
                    socklen_t addrlen = sizeof(struct sockaddr_in);

                    SOCKET client_sockfd = ::accept(fd, (struct sockaddr *) &client_addr, &addrlen);

                    if (client_sockfd == INVALID_SOCKET) {
                        printf("accept failed: %s\n", SocketUtils::getLastSocketErrorMessage().c_str());

                        //signaled = true;
                        semaphore.release();
                        return false;
                    }
                    else {
                        //valid client socket
                        result->initializeWithNewConnection(client_sockfd, client_addr);

                        semaphore.release();
                        return true;
                    }

                }
            }
            else {
                //non-blocking mode...

                struct sockaddr_in client_addr;
                socklen_t addrlen = sizeof(struct sockaddr_in);

                SOCKET client_sockfd = ::accept(fd, (struct sockaddr *) &client_addr, &addrlen);

                if (client_sockfd  != INVALID_SOCKET) {
                    //valid client socket
                    result->initializeWithNewConnection(client_sockfd, client_addr);

                    return true;
                }
                else if (WSAGetLastError() == WSAEWOULDBLOCK) {
                    // non-block result without any connection

                    return false;
                }
                else {
                    printf("accept failed: %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                    return false;
                }

            }
#else

            if (blocking) {

                struct sockaddr_in client_addr;
                socklen_t addrlen = sizeof(struct sockaddr_in);

                Platform::Thread *currentThread = Platform::Thread::getCurrentThread();
                //force count the socket as a semaphore 
                // per thread signal logic
                currentThread->semaphoreLock();
                if (isSignaled()) {
                    //signaled = true;
                    currentThread->semaphoreUnLock();
                    semaphore.release();
                    return false;
                }
                else {
                    currentThread->semaphoreWaitBegin(NULL);
                    currentThread->semaphoreUnLock();

                    int client_sockfd = ::accept(fd, (struct sockaddr *) &client_addr, &addrlen);

                    currentThread->semaphoreWaitDone(NULL);

                    if (client_sockfd >= 0) {
                        //valid client socket
                        result->initializeWithNewConnection(client_sockfd, client_addr);
                        semaphore.release();
                        return true;
                    }
                    else if (errno == EWOULDBLOCK || errno == EAGAIN) {
                        // non-block result without any connection
                        semaphore.release();
                        return false;
                    }
                    else {
                        printf("accept failed: %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                        semaphore.release();
                        return false;
                    }

                }

            }
            else {

                struct sockaddr_in client_addr;
                socklen_t addrlen = sizeof(struct sockaddr_in);

                int client_sockfd = ::accept(fd, (struct sockaddr *) &client_addr, &addrlen);

                if (client_sockfd >= 0) {
                    //valid client socket
                    result->initializeWithNewConnection(client_sockfd, client_addr);
                    return true;
                }
                else if (errno == EWOULDBLOCK || errno == EAGAIN) {
                    // non-block result without any connection
                    return false;
                }
                else {
                    printf("accept failed: %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                    return false;
                }

            }



#endif

            if (blocking)
                semaphore.release();

            return false;
        }

        bool isSignaled() {
            return signaled || Platform::Thread::isCurrentThreadInterrupted();//semaphore.isSignaled();
        }

    };

        }
