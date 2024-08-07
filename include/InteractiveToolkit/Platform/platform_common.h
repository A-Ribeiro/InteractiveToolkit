#pragma once

#include "../common.h"
//#include "../ITKCommon/ITKCommon.h"

#if defined(_WIN32)
// Sleep
#include <timeapi.h>
#include <processthreadsapi.h>

// Time
#include <inttypes.h>
#include <time.h>

// Thread
#include <process.h>

#include <signal.h>

// #include <comdef.h>
#ifndef MSG_DONTWAIT
#define MSG_DONTWAIT 0
#endif

namespace ITKPlatformUtil
{

    // returns the error in UTF8 codepage
    static std::string win32_GetLastErrorToString()
    {
        std::string result;

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

            auto _needed_size_bytes = WideCharToMultiByte(
                CP_UTF8, // GetConsoleOutputCP(), // GetConsoleCP(),
                WC_COMPOSITECHECK,
                s, -1,
                nullptr, 0,
                nullptr, nullptr
            );

            {
                char *aux = new char[_needed_size_bytes + 1];
                memset(aux, 0, (_needed_size_bytes + 1) * sizeof(char));
                WideCharToMultiByte(
                    CP_UTF8, // GetConsoleOutputCP(), // GetConsoleCP(),
                    WC_COMPOSITECHECK,
                    s, -1,
                    aux, _needed_size_bytes + 1,
                    nullptr, nullptr
                );
                result = aux;
                delete[] aux;
            }
            LocalFree(s);
        }

        return result;
    }

}

#elif defined(__linux__) || defined(__APPLE__)

// Sleep
#include <sched.h>
// #include <errno.h>

// Time
// #include <unistd.h>
// #include <sys/time.h>
// #include <time.h>

// Mutex
#include <pthread.h>

// Semaphore
#include <semaphore.h>
#include <signal.h>
// #include <errno.h>

// Thread
#include <sys/syscall.h>
// #include <sys/types.h>

#include <fcntl.h>

// Process
#include <sys/wait.h>

// Socket
#include <sys/socket.h>
#include <sys/times.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include <ifaddrs.h>

#if defined(__linux__)
    #include <netpacket/packet.h>
    #include <net/ethernet.h>
#elif defined(__APPLE__)
    #include <net/if_dl.h>
#endif

#ifndef LLADDR
#define LLADDR
#endif

// shared memory
#include <sys/mman.h>

#undef closesocket
#define closesocket(s) close(s)

// environment variables location
extern char **environ;

#else
#error Platform Not Supported!!!
#endif


#if defined(__APPLE__)

// for unamed semaphores on mac
//#include "Core/unamed_fake_sem.h"

#include <mach/mach_time.h>

// Thread
#include <sys/sysctl.h>

#endif

//
// IPC Common Defs
//
#if defined(_WIN32)
#define BUFFER_HANDLE_nullptr nullptr
#elif defined(__linux__) || defined(__APPLE__)
#define BUFFER_HANDLE_nullptr -1
#endif

namespace ITKPlatformUtil
{

    // last error from errno or GetLastError()...
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

            auto _needed_size_bytes = WideCharToMultiByte(
                CP_UTF8, // GetConsoleOutputCP(), // GetConsoleCP(),
                WC_COMPOSITECHECK,
                s, -1,
                nullptr, 0,
                nullptr, nullptr
            );

            {
                char *aux = new char[_needed_size_bytes + 1];
                memset(aux, 0, (_needed_size_bytes + 1) * sizeof(char));    
                WideCharToMultiByte(
                    CP_UTF8, // GetConsoleOutputCP(), // GetConsoleCP(),
                    WC_COMPOSITECHECK,
                    s, -1, // lstrlenW(s) + 1,
                    aux, _needed_size_bytes + 1,
                    nullptr, nullptr
                );
                result = aux;
                delete[] aux;
            }
            LocalFree(s);
        }
#else
        result = strerror(errno);
#endif

        return result;
    }
}
