#pragma once

// #include "platform_common.h"
#include "../common.h"
#include "Sleep.h"

namespace Platform
{

    /// \brief Mutex implementation.
    ///
    /// Example:
    ///
    /// \code
    ///
    /// Platform::Mutex mutex;
    ///
    /// // critical section begin
    /// mutex.lock();
    /// ...
    /// // critical section end
    /// mutex.unlock();
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    class Mutex
    {
    private:
#if defined(_WIN32)
        CRITICAL_SECTION mLock;
#elif defined(__APPLE__) || defined(__linux__)
        pthread_mutex_t mLock;
#endif

    public:

        //deleted copy constructor and assign operator, to avoid copy...
        Mutex(const Mutex &v) = delete;
        Mutex& operator=(const Mutex &v) = delete;
        
        /// \brief Construct the mutex in the current platform (windows, linux, mac)
        ///
        /// \author Alessandro Ribeiro
        ///
        Mutex()
        {
#if defined(_WIN32)
            InitializeCriticalSection(&mLock);
#elif defined(__APPLE__) || defined(__linux__)
            // Set it recursive
            pthread_mutexattr_t attributes;
            pthread_mutexattr_init(&attributes);
            pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);

            pthread_mutex_init(&mLock, &attributes);
#endif
        }

        /// \brief Destroy the mutex
        ///
        /// \author Alessandro Ribeiro
        ///
        ~Mutex()
        {
#if defined(_WIN32)
            DeleteCriticalSection(&mLock);
#elif defined(__APPLE__) || defined(__linux__)
            pthread_mutex_destroy(&mLock);
#endif
        }

        /// \brief Lock the critical section
        ///
        /// \author Alessandro Ribeiro
        ///
        void lock();
//         {
// #if defined(_WIN32)
//             EnterCriticalSection(&mLock);
// #elif defined(__APPLE__) || defined(__linux__)
//             int max_tries = 0;
//             while (pthread_mutex_lock(&mLock) != 0)
//             {
//                 Platform::Sleep::millis(1);
//                 max_tries++;
//                 if (max_tries > 1000)
//                 {
//                     ITK_ABORT(true, "ERROR TO LOCK A MUTEX... MAX TRIES REACHED...\n");
//                 }
//             }
// #endif
//         }

        /// \brief Unlock the critical section
        ///
        /// \author Alessandro Ribeiro
        ///
        void unlock()
        {
#if defined(_WIN32)
            LeaveCriticalSection(&mLock);
#elif defined(__APPLE__) || defined(__linux__)
            pthread_mutex_unlock(&mLock);
#endif
        }
    };

}

#include "../ITKCommon/ITKAbort.h"
#ifdef ITK_ABORT
#include "Mutex.inl"
#endif
