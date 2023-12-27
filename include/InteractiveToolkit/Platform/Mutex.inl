#pragma once

#include "Mutex.h"

namespace Platform
{

        inline void Mutex::lock()
        {
#if defined(_WIN32)
            EnterCriticalSection(&mLock);
#elif defined(__APPLE__) || defined(__linux__)
            int max_tries = 0;
            while (pthread_mutex_lock(&mLock) != 0)
            {
                Platform::Sleep::millis(1);
                max_tries++;
                if (max_tries > 1000)
                {
                    ITK_ABORT(true, "ERROR TO LOCK A MUTEX... MAX TRIES REACHED...\n");
                }
            }
#endif
        }
}