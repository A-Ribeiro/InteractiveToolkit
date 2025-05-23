#pragma once

#include "platform_common.h"

#include "Mutex.h"
#include "Semaphore.h"
#include "Sleep.h"
#include "AutoLock.h"
#include "Core/NetworkConstants.h"
#include "Core/SocketUtils.h"

namespace Platform
{

    const uint16_t INPORT_ANY = 0;

    class SocketUDP
    {
#if defined(_WIN32)
        SOCKET fd;
#else
        int fd;
#endif

        int ttl;
        bool blocking;
        bool reuseAddress;
        bool sendBroadcast;

        uint32_t read_timeout_ms;
        uint32_t write_timeout_ms;

        struct sockaddr_in addr_in;

        Platform::Mutex mutex;

#if defined(_WIN32)
        HANDLE wsa_read_event;
#endif

        void initialize(bool blocking, bool reuseAddress, int ttl)
        {
            ITK_ABORT(this->fd != ITK_INVALID_SOCKET, "Cannot initialize a new connection with an already initialized socked.\n");

            fd = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            ITK_ABORT(fd == ITK_INVALID_SOCKET, "Error to create Socket. Message: %s", SocketUtils::getLastSocketErrorMessage().c_str());

#if defined(_WIN32)
            disableUDPWrongConnectionReset();
#endif
            setBlocking(blocking);
            setReuseAddress(reuseAddress);
            setTTL(ttl);
            setSendBroadcast(true);

            read_timeout_ms = 0xffffffff;  // INFINITE;
            write_timeout_ms = 0xffffffff; // INFINITE;

#if defined(_WIN32)
            wsa_read_event = WSACreateEvent();
            ITK_ABORT(wsa_read_event == WSA_INVALID_EVENT, "Error to create WSAEvent. Message: %s", SocketUtils::getLastSocketErrorMessage().c_str());

            ITK_ABORT(
                ::WSAEventSelect(fd, wsa_read_event, FD_READ) == SOCKET_ERROR, // FD_READ | FD_WRITE | FD_CLOSE
                "WSAEventSelect error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str());
#endif
        }

    public:

        //deleted copy constructor and assign operator, to avoid copy...
        SocketUDP(const SocketUDP &v) = delete;
        SocketUDP& operator=(const SocketUDP &v) = delete;
        
        bool isSignaled() const
        {
            return Platform::Thread::isCurrentThreadInterrupted();
        }

        SocketUDP()
        {
            SocketUtils::Instance()->InitSockets();

            fd = ITK_INVALID_SOCKET;
            ttl = -1;
            blocking = false;
            reuseAddress = false;
            sendBroadcast = false;
            memset(&addr_in, 0, sizeof(struct sockaddr_in));

            read_timeout_ms = 0xffffffff;  // INFINITE;
            write_timeout_ms = 0xffffffff; // INFINITE;

#if defined(_WIN32)
            wsa_read_event = WSA_INVALID_EVENT;
#endif
        }

#if defined(_WIN32)
        SOCKET getNativeFD()
#else
        int getNativeFD()
#endif
        {
            return fd;
        }

        const struct sockaddr_in &getAddr()
        {
            return addr_in;
        }

        void close()
        {
            Platform::AutoLock auto_lock(&mutex);

            if (fd != ITK_INVALID_SOCKET)
            {
                printf("PlatformUDPSocket Close...\n");

                ITK_ABORT(
                    ::closesocket(fd) != 0,
                    "closesocket error. %s",
                    SocketUtils::getLastSocketErrorMessage().c_str());

                fd = ITK_INVALID_SOCKET;
            }

#if defined(_WIN32)
            if (wsa_read_event != WSA_INVALID_EVENT)
            {
                ::WSACloseEvent(wsa_read_event);
                wsa_read_event = WSA_INVALID_EVENT;
            }
#endif

            ttl = -1;
            blocking = false;
            reuseAddress = false;
            sendBroadcast = false;
            memset(&addr_in, 0, sizeof(struct sockaddr_in));
        }

        void setBlocking(bool blocking)
        {
            Platform::AutoLock auto_lock(&mutex);
            ITK_ABORT(this->fd == ITK_INVALID_SOCKET, "Socket not initialized.\n");

            this->blocking = blocking;
            SocketUtils::SetSocketBlockingEnabled(fd, this->blocking);
        }

        void setTTL(int ttl)
        {
            Platform::AutoLock auto_lock(&mutex);
            ITK_ABORT(this->fd == ITK_INVALID_SOCKET, "Socket not initialized.\n");

            // set default ttl
            if (ttl == -1)
                ttl = 64;

            this->ttl = ttl;

            ITK_ABORT(
                ::setsockopt(fd, IPPROTO_IP, IP_TTL, (char *)&(this->ttl), sizeof(int)) == -1,
                "setsockopt SO_REUSEADDR error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str());
        }

        void setReuseAddress(bool reuseAddress)
        {
            Platform::AutoLock auto_lock(&mutex);
            ITK_ABORT(this->fd == ITK_INVALID_SOCKET, "Socket not initialized.\n");

            this->reuseAddress = reuseAddress;

            int aux = (this->reuseAddress) ? 1 : 0;
            ITK_ABORT(
                ::setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&aux, sizeof(int)) == -1,
                "setsockopt SO_REUSEADDR error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str());
        }

        void setWriteTimeout(uint32_t timeout_ms)
        {
            write_timeout_ms = timeout_ms;
            Platform::AutoLock auto_lock(&mutex);
            struct timeval timeout;
            timeout.tv_sec = timeout_ms / 1000;
            timeout.tv_usec = (timeout_ms % 1000) * 1000;
            ITK_ABORT(
                ::setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval)) == -1,
                "setsockopt SO_SNDTIMEO error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str());
        }

        void setReadTimeout(uint32_t timeout_ms)
        {
            read_timeout_ms = timeout_ms;
            Platform::AutoLock auto_lock(&mutex);
            struct timeval timeout;
            timeout.tv_sec = timeout_ms / 1000;
            timeout.tv_usec = (timeout_ms % 1000) * 1000;
            ITK_ABORT(
                ::setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval)) == -1,
                "setsockopt SO_RCVTIMEO error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str());
        }

        void setSendBroadcast(bool sendBroadcast)
        {
            Platform::AutoLock auto_lock(&mutex);
            ITK_ABORT(this->fd == ITK_INVALID_SOCKET, "Socket not initialized.\n");

            this->sendBroadcast = sendBroadcast;

            int aux = (this->sendBroadcast) ? 1 : 0;
            ITK_ABORT(
                ::setsockopt(fd, SOL_SOCKET, SO_BROADCAST, (char *)&aux, sizeof(int)) == -1,
                "setsockopt SO_BROADCAST error. %s",
                SocketUtils::getLastSocketErrorMessage().c_str());
        }

        // multicast group registration
        // Only affect the recvfrom
        bool multicastAddMembership(const std::string &multicast_address_ip, const std::string &interface_address_ip = "INADDR_ANY")
        {
            Platform::AutoLock auto_lock(&mutex);
            ITK_ABORT(this->fd == ITK_INVALID_SOCKET, "Socket not initialized.\n");

            struct ip_mreq mreq;

            mreq.imr_interface.s_addr = SocketUtils::ipv4_address_to_nl(interface_address_ip);
            mreq.imr_multiaddr.s_addr = inet_addr(multicast_address_ip.c_str());

            if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(struct ip_mreq)) == -1)
            {
                printf("setsockopt IP_ADD_MEMBERSHIP error. %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                return false;
            }

            return true;
        }

        // multicast group registration
        // Only affect the recvfrom
        bool multicastDropMembership(const std::string &multicast_address_ip, const std::string &interface_address_ip = "INADDR_ANY")
        {
            Platform::AutoLock auto_lock(&mutex);
            ITK_ABORT(this->fd == ITK_INVALID_SOCKET, "Socket not initialized.\n");

            struct ip_mreq mreq;

            mreq.imr_interface.s_addr = SocketUtils::ipv4_address_to_nl(interface_address_ip);
            mreq.imr_multiaddr.s_addr = inet_addr(multicast_address_ip.c_str());

            if (setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(struct ip_mreq)) == -1)
            {
                printf("setsockopt IP_DROP_MEMBERSHIP error. %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                return false;
            }

            return true;
        }

#if defined(_WIN32)

        void disableUDPWrongConnectionReset()
        {
            Platform::AutoLock auto_lock(&mutex);
            ITK_ABORT(this->fd == ITK_INVALID_SOCKET, "Socket not initialized.\n");
            DWORD dwBytesReturned = 0;
            BOOL bNewBehavior = FALSE;
// causes WSAECONNRESET
#define SIO_UDP_CONNRESET _WSAIOW(IOC_VENDOR, 12)
            int rc = WSAIoctl(fd, SIO_UDP_CONNRESET,
                              &bNewBehavior, sizeof(BOOL),
                              nullptr, 0, &dwBytesReturned,
                              nullptr, nullptr);
        }

#endif

        void createFD(bool blocking = true, bool reuseAddress = true, int ttl = 64)
        {
            Platform::AutoLock auto_lock(&mutex);
            initialize(blocking, reuseAddress, ttl);
        }

        bool bind(const std::string &address_ip = "INADDR_ANY", uint16_t port = INPORT_ANY)
        {

            Platform::AutoLock auto_lock(&mutex);
            ITK_ABORT(this->fd == ITK_INVALID_SOCKET, "Socket not initialized.\n");

            addr_in.sin_family = AF_INET;
            addr_in.sin_addr.s_addr = SocketUtils::ipv4_address_to_nl(address_ip);
            addr_in.sin_port = htons(port);

            if (::bind(fd, (struct sockaddr *)&addr_in, sizeof(struct sockaddr_in)) == -1)
            {
                printf("Failed to bind socket. %s", SocketUtils::getLastSocketErrorMessage().c_str());
                return false;
            }

            // get ephemeral port info
            socklen_t len = sizeof(struct sockaddr_in);
            getsockname(fd, (sockaddr *)&addr_in, &len);

            // print stats
            printf("[SocketUDP] Bind OK\n");
            printf("          UDP Addr: %s\n", inet_ntoa(addr_in.sin_addr));
            printf("              Port: %u\n", ntohs(addr_in.sin_port));

            return true;
        }

        bool write_buffer(
            const struct sockaddr_in &target_address,
            const uint8_t *data, uint32_t size,
            uint32_t *write_feedback = nullptr)
        {

            if (isSignaled() || fd == ITK_INVALID_SOCKET)
            {
                if (write_feedback != nullptr)
                    *write_feedback = 0;
                return false;
            }

            ITK_SOCKET_SSIZE_T iResult = ::sendto(
                fd,
                (const char *)data, size,
                0,
                (const struct sockaddr *)&target_address,
                sizeof(struct sockaddr_in));

            if (iResult >= 0)
            {

                if (write_feedback != nullptr)
                    *write_feedback = static_cast<uint32_t>(iResult);

                return true;
            }
            else
            {
#if defined(_WIN32)
                if (WSAGetLastError() == WSAEWOULDBLOCK)
                {
#else
                int _error = errno;
                if (errno == EWOULDBLOCK || errno == EAGAIN)
                {
#endif
                    // printf("non block write skip\n");

                    // non-blocking socket event
                    if (write_feedback != nullptr)
                        *write_feedback = 0;
                    return false;
                }
                else
                {
                    // socket error...
                    printf("sendto failed: %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                    if (write_feedback != nullptr)
                        *write_feedback = 0;
                    return false;
                }

                return false;
            }
        }

        bool read_buffer(struct sockaddr_in *source_address, uint8_t *data, uint32_t size, uint32_t *read_feedback)
        {

            if (isSignaled() || fd == ITK_INVALID_SOCKET)
            {
                if (read_feedback != nullptr)
                    *read_feedback = 0;
                return false;
            }

            if (blocking)
            {
                Platform::Thread *currentThread = Platform::Thread::getCurrentThread();

#if defined(_WIN32)
                DWORD dwWaitResult;
                HANDLE handles_threadInterrupt_sem[2] = {
                    wsa_read_event,                         // WAIT_OBJECT_0 + 0
                    currentThread->m_thread_interrupt_event // WAIT_OBJECT_0 + 1
                };

                DWORD dwWaitTime = INFINITE;

                if (read_timeout_ms != 0xffffffff)
                    dwWaitTime = read_timeout_ms;

                dwWaitResult = WaitForMultipleObjects(
                    2,                           // number of handles in array
                    handles_threadInterrupt_sem, // array of thread handles
                    FALSE,                       // wait until all are signaled
                    dwWaitTime                   // INFINITE //INFINITE
                );

                if (dwWaitResult == WAIT_TIMEOUT)
                {
                    if (read_feedback != nullptr)
                        *read_feedback = 0;
                    return false;
                }
                else

                    // true if the interrupt is signaled (and only the interrupt...)
                    if (dwWaitResult == WAIT_OBJECT_0 + 1)
                    {
                        // signaled = true;
                        if (read_feedback != nullptr)
                            *read_feedback = 0;
                        return false;
                    }
                    else

                        // true if the socket is signaled (might have the interrupt or not...)
                        if (dwWaitResult == WAIT_OBJECT_0 + 0)
                        {

                            WSANETWORKEVENTS NetworkEvents = {0};
                            // int nReturnCode = WSAWaitForMultipleEvents(1, &lphEvents[0], false, WSA_INFINITE, false);
                            // if (nReturnCode==WSA_WAIT_FAILED)
                            // throw MyException("WSA__WAIT_FAILED.\n");
                            ITK_ABORT(
                                WSAEnumNetworkEvents(fd, wsa_read_event, &NetworkEvents) == SOCKET_ERROR,
                                "WSAEnumNetworkEvents error. %s",
                                SocketUtils::getLastSocketErrorMessage().c_str());

                            // read event...
#else
                // force count the socket as a semaphore
                //  per thread signal logic
                currentThread->semaphoreLock();
                if (isSignaled())
                {
                    // signaled = true;
                    currentThread->semaphoreUnLock();

                    return false;
                }
                else
                {
                    currentThread->semaphoreWaitBegin(nullptr);
                    currentThread->semaphoreUnLock();
#endif

                            socklen_t addr_len = sizeof(struct sockaddr_in);
                            ITK_SOCKET_SSIZE_T iResult = ::recvfrom(
                                fd,
                                (char *)data, size,
                                0,
                                (struct sockaddr *)source_address,
                                &addr_len);

#if !defined(_WIN32)
                            currentThread->semaphoreWaitDone(nullptr);
#endif

                            if (iResult >= 0)
                            {

                                if (read_feedback != nullptr)
                                    *read_feedback = static_cast<uint32_t>(iResult);

                                return true;
                            }
                            else
                            {
#if defined(_WIN32)
                                if (WSAGetLastError() == WSAEWOULDBLOCK)
                                {
#else
                        // int _error = errno;
                        if (errno == EWOULDBLOCK || errno == EAGAIN)
                        {
#endif
                                    // printf("non block receive skip\n");

                                    // non-blocking socket event
                                    if (read_feedback != nullptr)
                                        *read_feedback = 0;
                                    return false;
                                }
                                else
                                {
                                    // socket error...
                                    printf("recv failed: %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                                    if (read_feedback != nullptr)
                                        *read_feedback = 0;
                                    return false;
                                }

                                if (read_feedback != nullptr)
                                    *read_feedback = 0;
                                return false;
                            }

                            // #if defined(_WIN32)
                            // #if !defined(_WIN32)
                        }
                // #endif
            }
            else
            {
                // non-blocking code

                socklen_t addr_len = sizeof(struct sockaddr_in);
                ITK_SOCKET_SSIZE_T iResult = ::recvfrom(
                    fd,
                    (char *)data, size,
                    0,
                    (struct sockaddr *)source_address,
                    &addr_len);

                if (iResult >= 0)
                {

                    if (read_feedback != nullptr)
                        *read_feedback = static_cast<uint32_t>(iResult);

                    return true;
                }
                else
                {
#if defined(_WIN32)
                    if (WSAGetLastError() == WSAEWOULDBLOCK)
                    {
#else
                    // int _error = errno;
                    if (errno == EWOULDBLOCK || errno == EAGAIN)
                    {
#endif
                        // printf("non block receive skip\n");

                        // non-blocking socket event
                        if (read_feedback != nullptr)
                            *read_feedback = 0;
                        return false;
                    }
                    else
                    {
                        // socket error...
                        printf("recv failed: %s\n", SocketUtils::getLastSocketErrorMessage().c_str());
                        if (read_feedback != nullptr)
                            *read_feedback = 0;
                        return false;
                    }

                    if (read_feedback != nullptr)
                        *read_feedback = 0;
                    return false;
                }
            }

            if (read_feedback != nullptr)
                *read_feedback = 0;
            return false;
        }
    };

}
