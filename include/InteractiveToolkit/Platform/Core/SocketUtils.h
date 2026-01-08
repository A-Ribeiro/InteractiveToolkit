#pragma once

//#include "../platform_common.h"
#include "../../common.h"
#include "../Mutex.h"
#include "../Semaphore.h"
#include "../Sleep.h"
#include "../AutoLock.h"
#include "NetworkConstants.h"

#include "../../ITKCommon/StringUtil.h"

#if defined(__APPLE__) || defined(__linux__)

#endif

#if defined(_WIN32)
#define ITK_INVALID_SOCKET INVALID_SOCKET
#define ITK_SOCKET_SSIZE_T int
#else
#define ITK_INVALID_SOCKET -1
#define ITK_SOCKET_SSIZE_T ssize_t
#endif


namespace Platform
{

    // Class used in windows to initialize and finalize sockets
    class SocketUtils
    {

#if defined(_WIN32)
        bool startup_ok;
        Platform::Mutex mutex;
#endif

        SocketUtils()
        {
        }

    public:
        void InitSockets()
        {
#if defined(_WIN32)
            Platform::AutoLock auto_lock(&mutex);
            if (startup_ok)
                return;
            WORD version;
            WSADATA wsaData;
            version = MAKEWORD(2, 2);
            startup_ok = (WSAStartup(version, &wsaData) == 0);
#endif
        }

        ~SocketUtils()
        {
#if defined(_WIN32)
            Platform::AutoLock auto_lock(&mutex);
            if (startup_ok)
            {
                WSACleanup();
                startup_ok = false;
            }
#endif
        }

        static SocketUtils *Instance()
        {
            static SocketUtils result;
            return &result;
        }

        static std::string getLastSocketErrorMessage()
        {
            std::string result;

#if defined(_WIN32)
            wchar_t *s = nullptr;
            FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                           nullptr, WSAGetLastError(),
                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                           (LPWSTR)&s, 0, nullptr);

            // WCHAR TO CONSOLE CP (Code Page)
            if (s)
            {
                /*
                wchar_t *aux = new wchar_t[lstrlenW(s)+1];
                memset(aux, 0, (lstrlenW(s)+1) * sizeof(wchar_t));

                //MultiByteToWideChar(CP_ACP, 0L, s, lstrlenA(s) + 1, aux, strlen(s));
                MultiByteToWideChar(
                    CP_OEMCP //GetConsoleCP()
                    , MB_PRECOMPOSED | MB_ERR_INVALID_CHARS , s, lstrlenA(s) + 1, aux, lstrlenA(s));
                    result = StringUtil::toString(aux);
                */

                char *aux = new char[lstrlenW(s) + 1];
                memset(aux, 0, (lstrlenW(s) + 1) * sizeof(char));
                WideCharToMultiByte(
                    GetConsoleOutputCP(), // GetConsoleCP(),
                    WC_COMPOSITECHECK,
                    s, lstrlenW(s) + 1,
                    aux, lstrlenW(s),
                    nullptr, nullptr);
                result = aux;
                delete[] aux;
                LocalFree(s);
            }
#else
            result = strerror(errno);
#endif

            return result;
        }

        static std::string getLastErrorMessage()
        {
            std::string result;

#if defined(_WIN32)
            wchar_t *s = nullptr;
            FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                           nullptr, GetLastError(),
                           MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                           (LPWSTR)&s, 0, nullptr);

            // WCHAR TO CONSOLE CP (Code Page)
            if (s)
            {
                /*
                wchar_t *aux = new wchar_t[lstrlenW(s)+1];
                memset(aux, 0, (lstrlenW(s)+1) * sizeof(wchar_t));

                //MultiByteToWideChar(CP_ACP, 0L, s, lstrlenA(s) + 1, aux, strlen(s));
                MultiByteToWideChar(
                    CP_OEMCP //GetConsoleCP()
                    , MB_PRECOMPOSED | MB_ERR_INVALID_CHARS , s, lstrlenA(s) + 1, aux, lstrlenA(s));
                    result = StringUtil::toString(aux);
                */

                char *aux = new char[lstrlenW(s) + 1];
                memset(aux, 0, (lstrlenW(s) + 1) * sizeof(char));
                WideCharToMultiByte(
                    GetConsoleOutputCP(), // GetConsoleCP(),
                    WC_COMPOSITECHECK,
                    s, lstrlenW(s) + 1,
                    aux, lstrlenW(s),
                    nullptr, nullptr);
                result = aux;
                delete[] aux;
                LocalFree(s);
            }
#else
            result = strerror(errno);
#endif

            return result;
        }

#if defined(__APPLE__)
        static void osxDisableSigPipe(int fd)
        {
            int aux = 1;
            ::setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (char *)&aux, sizeof(int));
            // ITK_ABORT(
            //     ::setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, (char *)&aux, sizeof(int)) == -1,
            //     "setsockopt SO_NOSIGPIPE error. %s",
            //     SocketUtils::getLastSocketErrorMessage().c_str());
        }
#endif

        // Returns true on success, or false if there was an error
#if defined(_WIN32)
        static bool SetSocketBlockingEnabled(const SOCKET &fd, bool blocking)
#else
        static bool SetSocketBlockingEnabled(int fd, bool blocking)
#endif
        {
            if (fd == ITK_INVALID_SOCKET)
                return false;

#if defined(_WIN32)
            unsigned long mode = blocking ? 0 : 1;
            return (ioctlsocket(fd, FIONBIO, &mode) == 0) ? true : false;
#else
            int flags = fcntl(fd, F_GETFL, 0);
            if (flags == -1)
                return false;
            flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
            return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
#endif
        }

    };

}
