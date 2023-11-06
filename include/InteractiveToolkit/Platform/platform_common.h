#pragma once

#include "../common.h"
#include "../ITKCommon/ITKCommon.h"

#if defined(_WIN32)
    // Sleep
    #include <timeapi.h>
    #include <processthreadsapi.h>

    // Time
    #include <inttypes.h>
    #include <time.h>

    // Thread
    #include <process.h>

    namespace ITKPlatformUtil {

        static std::string win32_GetLastErrorToString() {
            std::string result;

            wchar_t *s = NULL;
            FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                NULL, GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                (LPWSTR)&s, 0, NULL);

            // WCHAR TO CONSOLE CP (Code Page)
            if (s) {
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
                    GetConsoleOutputCP(), //GetConsoleCP(),
                    WC_COMPOSITECHECK,
                    s, lstrlenW(s) + 1,
                    aux, lstrlenW(s),
                    NULL, NULL
                );
                result = aux;
                delete[] aux;
                LocalFree(s);
            }

            return result;
        }

    }
    

#elif defined(__APPLE__)

    // Sleep
    #include <sched.h>
    //#include <errno.h>

    // Time
    //#include <unistd.h>
    //#include <sys/time.h>
    //#include <time.h>

    //#include <stdint.h> // uint64_t on iPhone
    #include <mach/mach_time.h>

    // Mutex
    #include <pthread.h>

    // Semaphore
    #include <semaphore.h>
    #include <signal.h>
    //#include <errno.h>

    // for unamed semaphores on mac
    #include "Core/unamed_fake_sem.h"

    // Thread
    #include <sys/syscall.h>
    #include <sys/sysctl.h>
    //#include <sys/types.h>

    #include <fcntl.h>

    // Process
    #include <sys/wait.h>

    // environment variables location
    extern char **environ;

#elif defined(__linux__)

    // Sleep
    #include <sched.h>
    //#include <errno.h>

    // Time
    //#include <unistd.h>
    //#include <sys/time.h>
    //#include <time.h>

    // Mutex
    #include <pthread.h>

    // Semaphore
    #include <semaphore.h>
    #include <signal.h>
    //#include <errno.h>

    // Thread
    #include <sys/syscall.h>
    //#include <sys/types.h>

    #include <fcntl.h>

    // Process
    #include <sys/wait.h>

    // environment variables location
    extern char **environ;

#else
#error Platform Not Supported!!!
#endif

