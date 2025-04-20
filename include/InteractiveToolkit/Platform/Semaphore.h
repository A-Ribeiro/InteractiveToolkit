#pragma once

//#include "platform_common.h"
#include "../common.h"
#include "Thread.h"
#include "Sleep.h"

#ifdef __APPLE__
    #include "Core/unamed_fake_sem.h"
#endif

namespace Platform
{

    class Semaphore
    {

#if defined(_WIN32)
        HANDLE semaphore;
#elif defined(__linux__)
        sem_t semaphore;
#elif defined(__APPLE__)
        fake_sem_t semaphore;
#endif

    public:
        //deleted copy constructor and assign operator, to avoid copy...
        Semaphore(const Semaphore &v) = delete;
        Semaphore& operator=(const Semaphore &v) = delete;
        
        Semaphore(int count = 0)
        {
            // signaled = false;
#if defined(_WIN32)
            semaphore = CreateSemaphore(
                nullptr,     // default security attributes
                count,    // initial count
                LONG_MAX, // count,  // maximum count
                nullptr      // unnamed semaphore
            );
            ITK_ABORT(semaphore == nullptr, "CreateSemaphore error: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#elif defined(__linux__)
            sem_init(&semaphore, 0, count); // 0 means is a semaphore bound to threads
#elif defined(__APPLE__)
            fake_sem_init(&semaphore, 0, count);
#endif
        }
        ~Semaphore()
        {
#if defined(_WIN32)
            if (semaphore != nullptr)
                CloseHandle(semaphore);
            semaphore = nullptr;
#elif defined(__linux__)
            sem_destroy(&semaphore);
#elif defined(__APPLE__)
            fake_sem_destroy(&semaphore);
#endif
        }

        bool tryToAcquire(uint32_t timeout_ms = 0, bool ignore_signal = false)
        {
            Platform::Thread *currentThread = Platform::Thread::getCurrentThread();

#if defined(__linux__) || defined(__APPLE__)
            currentThread->semaphoreLock();
#endif

            if ((!ignore_signal) && isSignaled())
            {
#if defined(__linux__) || defined(__APPLE__)
                currentThread->semaphoreUnLock();
#endif
                return false;
            }

#if defined(_WIN32)

            DWORD dwWaitResult;

            // dwWaitResult = WaitForSingleObject( semaphore, (DWORD)timeout_ms );
            // signaled = signaled || dwWaitResult == WAIT_FAILED;

            HANDLE handles_threadInterrupt_sem[2] = {
                semaphore,                              // WAIT_OBJECT_0 + 0
                currentThread->m_thread_interrupt_event // WAIT_OBJECT_0 + 1
            };

            int handle_to_test = 2;
            if (ignore_signal)
                handle_to_test = 1;

            dwWaitResult = WaitForMultipleObjects(
                handle_to_test,              // number of handles in array
                handles_threadInterrupt_sem, // array of thread handles
                FALSE,                       // wait until all are signaled
                (DWORD)timeout_ms            // INFINITE
            );

            // true if the interrupt is signaled (and only the interrupt...)
            if (dwWaitResult == WAIT_OBJECT_0 + 1)
            {
                // signaled = true;
                return false;
            }

            // true if the semaphore is signaled (might have the interrupt or not...)
            return dwWaitResult == WAIT_OBJECT_0 + 0;
#elif defined(__linux__)
            if (timeout_ms == 0)
            {
                currentThread->semaphoreUnLock();
                return sem_trywait(&semaphore) == 0;
            }

            struct timespec ts;
            if (clock_gettime(CLOCK_REALTIME, &ts))
            {
                currentThread->semaphoreUnLock();
                ITK_ABORT(true, "clock_gettime error\n");
            }

            struct timespec ts_increment;
            ts_increment.tv_sec = timeout_ms / 1000;
            ts_increment.tv_nsec = ((long)timeout_ms % 1000L) * 1000000L;

            ts.tv_nsec += ts_increment.tv_nsec;
            ts.tv_sec += ts.tv_nsec / 1000000000L;
            ts.tv_nsec = ts.tv_nsec % 1000000000L;

            ts.tv_sec += ts_increment.tv_sec;

            currentThread->semaphoreWaitBegin(&semaphore);
            currentThread->semaphoreUnLock();
            int s = sem_timedwait(&semaphore, &ts);

            if (ignore_signal){
                while ((s == -1 && errno != ETIMEDOUT))
                    s = sem_timedwait(&semaphore, &ts);
            }

            currentThread->semaphoreWaitDone(&semaphore);

            // currentThread->isCurrentThreadInterrupted() ||
            if ((s == -1 && errno != ETIMEDOUT))
            {
                // interrupt signaled
                // signaled = true;
                return false;
            }

            return s == 0;
#elif defined(__APPLE__)
            if (timeout_ms == 0)
            {
                currentThread->semaphoreUnLock();
                return fake_sem_trywait(&semaphore) == 0;
            }

            struct timespec ts;
            if (clock_gettime(CLOCK_REALTIME, &ts))
            {
                currentThread->semaphoreUnLock();
                ITK_ABORT(true, "clock_gettime error\n");
            }

            struct timespec ts_increment;
            ts_increment.tv_sec = timeout_ms / 1000;
            ts_increment.tv_nsec = ((long)timeout_ms % 1000L) * 1000000L;

            ts.tv_nsec += ts_increment.tv_nsec;
            ts.tv_sec += ts.tv_nsec / 1000000000L;
            ts.tv_nsec = ts.tv_nsec % 1000000000L;

            ts.tv_sec += ts_increment.tv_sec;

            currentThread->semaphoreWaitBegin(nullptr);
            currentThread->semaphoreUnLock();
            int s = fake_sem_timedwait(&semaphore, &ts, ignore_signal);
            if (ignore_signal){
                while ((s == -1 && errno != ETIMEDOUT))
                    s = fake_sem_timedwait(&semaphore, &ts, ignore_signal);
            }
            currentThread->semaphoreWaitDone(nullptr);

            // currentThread->isCurrentThreadInterrupted() ||
            if ((s == -1 && errno != ETIMEDOUT))
            {
                // interrupt signaled
                // signaled = true;
                return false;
            }

            return s == 0;
#endif
        }

        bool blockingAcquire(bool ignore_signal = false)
        {
#if defined(_WIN32)
            bool signaled = (!ignore_signal) && isSignaled();
            while (!signaled && !tryToAcquire(UINT32_MAX, ignore_signal))
            {
                signaled = (!ignore_signal) && isSignaled();
            }
            return !signaled;

#elif defined(__linux__)

            Platform::Thread *currentThread = Platform::Thread::getCurrentThread();

            currentThread->semaphoreLock();

            bool signaled = (!ignore_signal) && isSignaled();

            if (signaled)
            {
                currentThread->semaphoreUnLock();
            }
            else
            {
                currentThread->semaphoreWaitBegin(&semaphore);
                currentThread->semaphoreUnLock();

                // signaled = signaled || (sem_wait(&semaphore) != 0);
                signaled = (sem_wait(&semaphore) != 0);
                if (ignore_signal)
                {
                    while (signaled)
                        signaled = (sem_wait(&semaphore) != 0);
                }

                currentThread->semaphoreWaitDone(&semaphore);
            }

            return !signaled;

#elif defined(__APPLE__)
            Platform::Thread *currentThread = Platform::Thread::getCurrentThread();

            currentThread->semaphoreLock();

            bool signaled = (!ignore_signal) && isSignaled();

            if (signaled)
            {
                currentThread->semaphoreUnLock();
            }
            else
            {
                currentThread->semaphoreWaitBegin(nullptr);
                currentThread->semaphoreUnLock();
                signaled = (fake_sem_wait(&semaphore, ignore_signal) != 0);

                if (ignore_signal)
                {
                    while (signaled)
                        signaled = (fake_sem_wait(&semaphore, ignore_signal) != 0);
                }

                currentThread->semaphoreWaitDone(nullptr);
            }

            return !signaled;
#endif
        }

        void release()
        {
#if defined(_WIN32)
            BOOL result = ReleaseSemaphore(semaphore, 1, nullptr);
            ITK_ABORT(!result, "ReleaseSemaphore error: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#elif defined(__linux__)
            sem_post(&semaphore);
#elif defined(__APPLE__)
            fake_sem_post(&semaphore);
#endif
        }

        // only check if this queue is signaled for the current thread...
        // it may be active in another thread...
        bool isSignaled() const
        {
            return Platform::Thread::isCurrentThreadInterrupted();
        }
    };

}
