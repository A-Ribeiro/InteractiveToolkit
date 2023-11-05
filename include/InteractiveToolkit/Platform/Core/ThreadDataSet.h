#pragma once

#include "../platform_common.h"
#include "../Mutex.h"

#include "../../EventCore/Callback.h"
#include "../../ITKCommon/ITKAbort.h"
#include "../../ITKCommon/STLSemaphore.h"

namespace Platform
{

#if defined(_WIN32)
    typedef DWORD ThreadIdentifier;
    typedef HANDLE NativeThreadHandle;

    static ITK_INLINE DWORD GetCurrentThreadId_Custom()
    {
        return GetCurrentThreadId();
    }

#elif defined(__APPLE__)
    typedef uint64_t ThreadIdentifier;
    typedef pthread_t NativeThreadHandle;

    static ITK_INLINE uint64_t GetCurrentThreadId_Custom()
    {
        uint64_t tid;
        pthread_threadid_np(NULL, &tid);
        return tid;
    }

    void platform_thread_signal_handler_usr1(int signal) {}
#elif defined(__linux__)
    typedef pid_t ThreadIdentifier;
    typedef pthread_t NativeThreadHandle;

    static ITK_INLINE ThreadIdentifier GetCurrentThreadId_Custom()
    {
        return syscall(SYS_gettid);
    }
    void platform_thread_signal_handler_usr1(int signal) {}
#endif

    enum class ThreadPriority : uint8_t
    {
        None,
        Normal,
        High,
        Realtime
    };

    // opaque pointer...
    class Thread;

    class ThreadDataSet
    {

        ThreadPriority __thread_priority = ThreadPriority::None;

        Mutex register_mutex;
    public:
        using MapT = typename std::map<ThreadIdentifier, Thread *>;
        ITKCommon::STLSemaphore semaphore;
        

        MapT threadData;

        void registerThread(Thread *thread)
        {
            ThreadIdentifier tid = GetCurrentThreadId_Custom();

            register_mutex.lock();

            // grab all semaphores
            semaphore.acquireCount(threadData.size());

            threadData[tid] = (Thread *)(thread);

            // release all semaphores
            semaphore.releaseCount(threadData.size());

            register_mutex.unlock();

            
        }

        void unregisterThread(Thread *thread);

        Thread *getThreadByID(const ThreadIdentifier &tid)
        {
            Thread *result = NULL;

            // threadDataLock.lock();
            semaphore.acquire();

            if (threadData.size() > 0)
            {
                MapT::iterator it;
                it = threadData.find(tid);
                if (it != threadData.end())
                    result = it->second;
            }

            semaphore.release();
            // threadDataLock.unlock();

            return result;
        }

        ThreadPriority getGlobalThreadPriority()
        {
            if (__thread_priority == ThreadPriority::None)
                setGlobalThreadPriority(ThreadPriority::Normal);
            return __thread_priority;
        }

        void setGlobalThreadPriority(ThreadPriority priority)
        {
            /*ARIBEIRO_ABORT(
                Thread::getCurrentThread() != Thread::getMainThread(),
                "You can set the thread priority just from the main thread.\n"
            );*/

            __thread_priority = priority;

#if defined(_WIN32)

            if (__thread_priority == ThreadPriority::Normal)
            {
                if (!SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS))
                    printf("SetPriorityClass  MainThread ERROR...\n");
                if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL))
                    printf("SetThreadPriority MainThread ERROR...\n");
            }
            else if (__thread_priority == ThreadPriority::High)
            {
                if (!SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS))
                    printf("SetPriorityClass  MainThread ERROR...\n");
                if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST))
                    printf("SetThreadPriority MainThread ERROR...\n");
            }
            else if (__thread_priority == ThreadPriority::Realtime)
            {
                if (!SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS))
                    printf("SetPriorityClass  MainThread ERROR...\n");
                if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
                    printf("SetThreadPriority MainThread ERROR...\n");
            }

#elif defined(__linux__) || defined(__APPLE__)

#endif

            if (__thread_priority == ThreadPriority::Normal)
            {
                printf("setGlobalThreadPriority -> ThreadPriority::Normal\n");
            }
            else if (__thread_priority == ThreadPriority::High)
            {
                printf("setGlobalThreadPriority -> ThreadPriority::High\n");
            }
            else if (__thread_priority == ThreadPriority::Realtime)
            {
                printf("setGlobalThreadPriority -> ThreadPriority::Realtime\n");
            }
        }

        static ThreadDataSet *Instance()
        {
            static ThreadDataSet manager;
            return &manager;
        }
    };

}