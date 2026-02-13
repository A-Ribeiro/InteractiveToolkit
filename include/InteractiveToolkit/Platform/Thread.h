#pragma once

//#include "platform_common.h"
#include "../common.h"
#include "Mutex.h"
#include "Sleep.h"

#include "../EventCore/Callback.h"
#include "../ITKCommon/ITKAbort.h"
#include "../ITKCommon/STLSemaphore.h"

#include "Core/ThreadDataSet.h"

namespace Platform
{

    //
    // Thread common properties and OPs
    //

    // like errno, that has a value per thread
    class PerThreadData
    {
    public:
#if defined(_WIN32)

        Mutex mutex;

#else

        std::condition_variable_any interrupt_cv;
        std::recursive_mutex interrupt_m;

        int opened_semaphores;
#endif

        std::condition_variable wait_cv;
        std::mutex wait_m;

        void init()
        {
#if defined(_WIN32)
#else
            opened_semaphores = 0;
#endif
        }
    };

    //     struct ThreadFinalizeStruct
    //     {
    //         NativeThreadHandle handle;
    // #if defined(_WIN32)
    //         HANDLE interrupt_event_handle;
    // #endif
    //         int32_t timeout_ms;
    //     };

    /// \brief Manager a thread in several platforms. You can use functions, structs and classes.
    ///
    /// \author Alessandro Ribeiro
    ///

    class Thread
    {
    public:
        friend class ThreadDataSet;

        PerThreadData perThreadData;

    private:
        //
        // OS SPECIFIC ATTRIBUTES
        //
#if defined(_WIN32)

// Fix for unaligned stack with clang and GCC on Windows XP 32-bit
#if (defined(__clang__) || defined(__GNUC__))
        __attribute__((__force_align_arg_pointer__))
#endif
        static unsigned int __stdcall entryPoint(void *userData)
        {
            ThreadPriority priority = ThreadDataSet::Instance()->getGlobalThreadPriority();

            if (priority == ThreadPriority::Normal)
            {
                if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL))
                    printf("SetThreadPriority ChildThread ERROR...\n");
            }
            else if (priority == ThreadPriority::High)
            {
                if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST))
                    printf("SetThreadPriority ChildThread ERROR...\n");
            }
            else if (priority == ThreadPriority::Realtime)
            {
                if (!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL))
                    printf("SetThreadPriority ChildThread ERROR...\n");
            }

            // The Thread instance is stored in the user data
            Thread *owner = reinterpret_cast<Thread *>(userData);
            ThreadDataSet::Instance()->registerThread(owner);
            // Forward to the owner
            owner->runEntryPoint();
            ThreadDataSet::Instance()->unregisterThread(owner);
            // Optional, but it is cleaner
            _endthreadex(0);
            return 0;
        }

        HANDLE m_thread;
        unsigned int m_threadId;

    public:
        HANDLE m_thread_interrupt_event;
#else
        static void *entryPoint(void *userData)
        {
            // The Thread instance is stored in the user data
            Thread *owner = reinterpret_cast<Thread *>(userData);

#ifndef OS_ANDROID
            // Tell the thread to handle cancel requests immediately
            pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);
#endif
            ThreadDataSet::Instance()->registerThread(owner);
            // Forward to the owner
            owner->runEntryPoint();
            ThreadDataSet::Instance()->unregisterThread(owner);
            return nullptr;
        }
        pthread_t m_thread;
        bool m_isActive;
        bool m_isMain;

        Thread *interrupt_thread;

        bool skip_interrupt;

    public:
        pthread_t *getNativeThread()
        {
            return &m_thread;
        }

        // int opened_semaphore;

        void semaphoreLock()
        {
            perThreadData.interrupt_m.lock();
        }

        void semaphoreUnLock()
        {
            perThreadData.interrupt_m.unlock();
        }

        void semaphoreWaitBegin(sem_t *semaphore)
        {
            std::lock_guard<decltype(perThreadData.interrupt_m)> lock(perThreadData.interrupt_m);
            perThreadData.opened_semaphores++;
        }

        void semaphoreWaitDone(sem_t *semaphore)
        {
            std::lock_guard<decltype(perThreadData.interrupt_m)> lock(perThreadData.interrupt_m);
            perThreadData.opened_semaphores--;
            if (perThreadData.opened_semaphores <= 0)
                perThreadData.interrupt_cv.notify_all();
        }

#endif

    protected:
        // Mutex mutex;

        void setExited()
        {
#if defined(_WIN32)

            std::lock_guard<decltype(perThreadData.wait_m)> lock(perThreadData.wait_m);
            exited = true;
            perThreadData.wait_cv.notify_all();

#elif defined(__linux__) || defined(__APPLE__)

            std::lock_guard<decltype(perThreadData.wait_m)> lock(perThreadData.wait_m);
            exited = true;
            perThreadData.wait_cv.notify_all();

#endif
        }

        volatile bool interrupted;
        volatile bool exited;
        volatile bool started;
        volatile bool shouldReleaseThreadID_byItself;

        volatile int waitCalls;

        EventCore::Callback<void()> runEntryPoint;

        // virtual void runEntryPointWithParams() = 0;

        //
        // OnAbort
        //
        static void OnAbort_ForceFinishAllThreads(const char *file, int line, const char *message)
        {
            printf("OnAbort_ForceFinishAllThreads ... \n");

            Thread::getMainThread()->interrupt();

            Thread *currentThread = Thread::getCurrentThread();

            bool allThreadsFinalized = false;

            Time time;

            time.update();
            float time_acc = 0;
            while (!allThreadsFinalized)
            {
                allThreadsFinalized = true;

                ThreadDataSet::Instance()->semaphore.acquire();

                // printf("Interrupting %u threads...\n", ThreadDataSet::Instance()->threadData.size());
                std::unordered_map<ThreadIdentifier, Thread *>::iterator it;
                for (it = ThreadDataSet::Instance()->threadData.begin(); it != ThreadDataSet::Instance()->threadData.end(); it++)
                {
                    Thread *thread = it->second;
                    // if (thread != currentThread)
                    {
                        // printf("interrupt\n");
                        thread->interrupt();
                        if (thread != currentThread)
                            allThreadsFinalized = false;
                    }
                }

                ThreadDataSet::Instance()->semaphore.release();

                if (!allThreadsFinalized)
                {
                    // printf(".^_^."); fflush(stdout);
                    Platform::Sleep::millis(300);
                }
                time.update();
                time_acc += time.deltaTime;
                if (time_acc > 30.0f)
                {
                    printf("[FORCING EXIT] 30 seconds timeout trying to interrupt opened threads.\n");
                    break;
                }
            }
        }

    public:
        std::string name;

        /// \brief Check if the current thread is interrupted
        ///
        /// A thread can be interrupted by calling interrupt() method from the parent thread.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// void thread_example() {
        ///     // the thread or main thread could call #interrupt() to change this state
        ///     while (!Thread::isCurrentThreadInterrupted()) {
        ///         ...
        ///         //avoid 100% CPU using by this thread
        ///         PlatformPlatform::Sleep::millis(1);
        ///     }
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return true if the thread is interrupted
        ///
        static ITK_INLINE bool isCurrentThreadInterrupted()
        {
            Thread *thread = getCurrentThread();
            if (thread != nullptr)
                return thread->interrupted;
            return false;
        }

        /// \brief Get the current Thread
        ///
        /// Get the current thread. If the thread is not spawned by the Thread, it will return nullptr.
        ///
        /// \author Alessandro Ribeiro
        /// \return A reference to the current thread spawned.
        ///
        static ITK_INLINE Thread *getCurrentThread()
        {
            Thread *result = ThreadDataSet::Instance()->getThreadByID(GetCurrentThreadId_Custom());
            if (result == nullptr)
                return Thread::getMainThread();
            return result;
        }
        static void staticInitialization()
        {
            getMainThread();
        }
        static Thread *getMainThread()
        {
            // force set the global thread priority
            ThreadDataSet::Instance()->getGlobalThreadPriority();
            // printf("[Thread] Get main thread");
            // return main thread
            // force to instanciate the opened thread manager before the main thread
            //   Fix finalization issues related to the main thread...
            ThreadDataSet::Instance();
            static Thread mainThread;
            if (mainThread.name.size() == 0)
            {

                ITKCommon::ITKAbort::Instance()->OnAbort.remove(Thread::OnAbort_ForceFinishAllThreads);
                ITKCommon::ITKAbort::Instance()->OnAbort.add(Thread::OnAbort_ForceFinishAllThreads);

                // OnAbortBeforeExit = _thread_at_exit;
                mainThread.name = "Main Thread";

                ThreadDataSet::Instance()->registerThread(&mainThread);
            }
            return &mainThread;
        }

        virtual ~Thread()
        {
#if defined(_WIN32)

            interrupt();
            wait();

            // // printf("[%s] Thread::~Thread() shouldReleaseThreadID_byItself = true\n", name.c_str());
            // Platform::Sleep::millis(10);
            // while (waitCalls > 0)
            //     Platform::Sleep::millis(10);
            {
                std::unique_lock<decltype(perThreadData.wait_m)> lock(perThreadData.wait_m);
                while (waitCalls > 0)
                {
                    perThreadData.wait_cv.wait(lock);
                }
            }

            if (m_thread)
            {
                // A thread cannot wait for itself!
                if (m_threadId != GetCurrentThreadId_Custom())
                    WaitForSingleObject(m_thread, INFINITE);

                // CLOSE THREAD
                if (m_thread)
                    CloseHandle(m_thread);
                m_thread = nullptr;
            }

            // CLOSE EVENT
            if (m_thread_interrupt_event != nullptr)
                CloseHandle(m_thread_interrupt_event);
            m_thread_interrupt_event = nullptr;

#elif defined(__linux__) || defined(__APPLE__)

            perThreadData.interrupt_m.lock();
            if (interrupt_thread != nullptr)
            {
                perThreadData.interrupt_m.unlock();
                interrupt_thread->wait();
                perThreadData.interrupt_m.lock();
                if (interrupt_thread != nullptr)
                    delete interrupt_thread;
                interrupt_thread = nullptr;
            }
            // avoid call interrupt in the static main thread instance...
            m_isMain = false;

            perThreadData.interrupt_m.unlock();

            interrupt();
            wait();

            // the same code again, to avoid leave interrupt thread alive
            perThreadData.interrupt_m.lock();
            if (interrupt_thread != nullptr)
            {
                perThreadData.interrupt_m.unlock();
                interrupt_thread->wait();
                perThreadData.interrupt_m.lock();
                if (interrupt_thread != nullptr)
                    delete interrupt_thread;
                interrupt_thread = nullptr;
            }
            perThreadData.interrupt_m.unlock();



            {
                std::unique_lock<decltype(perThreadData.wait_m)> lock(perThreadData.wait_m);
                while (waitCalls > 0)
                {
                    perThreadData.wait_cv.wait(lock);
                }
            }

            if (m_isActive)
            {
                // A thread cannot wait for itself!
                if (pthread_equal(pthread_self(), m_thread) == 0)
                    pthread_join(m_thread, nullptr);

                // m_thread = nullptr;
            }

#endif
        }

        /// \brief Set a flag to the thread indicating that it is interrupted.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// void thread_example() {
        ///     // the thread or main thread could call #Thread::interrupt() to change this state
        ///     while (!Thread::isCurrentThreadInterrupted()) {
        ///         ...
        ///         //avoid 100% CPU using by this thread
        ///         PlatformPlatform::Sleep::millis(1);
        ///     }
        /// }
        ///
        /// Thread thread( &thread_example );
        /// // start the thread
        /// thread.start();
        /// ...
        /// // inside the thread implementation, it is necessary to check the #Thread::isCurrentThreadInterrupted()
        /// thread.interrupt();
        /// // wait thread execution finish
        /// thread.wait();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void interrupt(bool create_interrupt_thread = false)
        {

#if defined(_WIN32)
            Platform::AutoLock lock(&perThreadData.mutex);

            if (interrupted)
                return;

            interrupted = true;

            SetEvent(m_thread_interrupt_event);

#elif defined(__linux__) || defined(__APPLE__)

            std::unique_lock<decltype(perThreadData.interrupt_m)> lock(perThreadData.interrupt_m);

            if (skip_interrupt || interrupted)
                return;

            interrupted = true;

            if (m_isMain || m_isActive)
            {

                //
                // Generate EINTR on wait state of semaphores and similar wait objects
                //

                if (create_interrupt_thread || pthread_equal(pthread_self(), m_thread))
                {

                    //printf("Creating interrupt thread. Name: %s\n", name.c_str());

                    if (interrupt_thread == nullptr)
                    {
                        Thread *_instance = this;
                        interrupt_thread = new Thread(
                            [_instance]()
                            {
                                struct sigaction sa;
                                sigemptyset(&sa.sa_mask);
                                sa.sa_handler = platform_thread_signal_handler_usr1;
                                sa.sa_flags = 0;
                                sigaction(SIGUSR1, &sa, 0);

                                {
                                    std::unique_lock<decltype(_instance->perThreadData.interrupt_m)> lock(_instance->perThreadData.interrupt_m);
                                    //printf("Trying to signal a thread with USR1. Name: %s\n", _instance->name.c_str());
                                    pthread_kill(*_instance->getNativeThread(), SIGUSR1);
                                    while (_instance->perThreadData.opened_semaphores > 0)
                                    {
                                        if (_instance->perThreadData.interrupt_cv.wait_for(lock, std::chrono::milliseconds(200)) == std::cv_status::timeout){
                                            // printf("Finalizing...\n");
                                            // Platform::Sleep::millis(1000);
                                            pthread_kill(*_instance->getNativeThread(), SIGUSR1);
                                        }
                                    }
                                }

                                
                            });
                        interrupt_thread->name = "Interrupt Thread";
                        interrupt_thread->setShouldDisposeThreadByItself(true);
                        interrupt_thread->skip_interrupt = true;
                        interrupt_thread->start();
                    }
                }
                else
                {

                    // raise(SIGUSR1);//raise on local thread

                    struct sigaction sa;
                    sigemptyset(&sa.sa_mask);
                    sa.sa_handler = platform_thread_signal_handler_usr1;
                    sa.sa_flags = 0;
                    sigaction(SIGUSR1, &sa, 0);

                    //printf("[Thread.h] Trying to signal a thread with USR1...\n");
                    //printf("perThreadData.opened_semaphores: %i\n", perThreadData.opened_semaphores);

                    pthread_kill(m_thread, SIGUSR1);
                    while (perThreadData.opened_semaphores > 0)
                    { // spurious wake-ups
                        if (perThreadData.interrupt_cv.wait_for(lock, std::chrono::milliseconds(200)) == std::cv_status::timeout)
                        {
                            // timeout
                            // printf("timeout...\n");
                            // printf("Finalizing...\n");
                            // Platform::Sleep::millis(1000);
                            pthread_kill(m_thread, SIGUSR1);
                        }
                    }
                }
            }
#endif
        }

        /// \brief Spawn the thread. It can be called just once.
        ///
        /// \brief Set a flag to the thread indicating that it is interrupted.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// void thread_example() {
        ///     // the thread or main thread could call #Thread::interrupt() to change this state
        ///     while (!Thread::isCurrentThreadInterrupted()) {
        ///         ...
        ///         //avoid 100% CPU using by this thread
        ///         PlatformPlatform::Sleep::millis(1);
        ///     }
        /// }
        ///
        /// Thread thread( &thread_example );
        /// // start the thread
        /// thread.start();
        /// ...
        /// // inside the thread implementation, it is necessary to check the #Thread::isCurrentThreadInterrupted()
        /// thread.interrupt();
        /// // wait thread execution finish
        /// thread.wait();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void start()
        {

            {
                #if defined(_WIN32)
                    Platform::AutoLock lock( &perThreadData.mutex );
                #elif defined(__linux__) || defined(__APPLE__)
                    std::lock_guard<decltype(perThreadData.interrupt_m)> lock(perThreadData.interrupt_m);
                #endif

                if (started)
                {
                    printf("thread already started...\n");
                    return;
                }
                started = true;
            }

            // force register the main thread...
            Thread::getMainThread();

            // create OS thread
#if defined(_WIN32)
            m_thread = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, 0, &Thread::entryPoint, this, 0, &m_threadId));

            if (!m_thread)
            {
                printf("Erro ao abrir thread...\n");
            }

#else
            bool error;

            pthread_attr_t attrs;
            pthread_attr_init(&attrs);

            error = pthread_attr_setdetachstate(&attrs, PTHREAD_CREATE_JOINABLE) != 0;
            ITK_ABORT(error, "Error set Thread Detach Scope\n");

            error = pthread_attr_setscope(&attrs, PTHREAD_SCOPE_SYSTEM) != 0;
            ITK_ABORT(error, "Error set Thread Scope\n");

            // error = pthread_attr_setschedpolicy(&attrs, SCHED_FIFO);
            error = pthread_attr_setschedpolicy(&attrs, SCHED_RR) != 0;
            ITK_ABORT(error, "Error set Thread Policy\n");

            // when set RoundRobin or FIFO, the priority can be 1 .. 99
            struct sched_param param = {0};
            param.sched_priority = 1;

            ThreadPriority priority = ThreadDataSet::Instance()->getGlobalThreadPriority();
            if (priority == ThreadPriority::Normal)
            {
                param.sched_priority = 33;
            }
            else if (priority == ThreadPriority::High)
            {
                param.sched_priority = 66;
            }
            else if (priority == ThreadPriority::Realtime)
            {
                param.sched_priority = 99;
            }

            error = pthread_attr_setschedparam(&attrs, &param) != 0;
            ITK_ABORT(error, "Error set Thread Priority\n");

            m_isActive = pthread_create(&m_thread, &attrs, &Thread::entryPoint, this) == 0;
            if (!m_isActive)
            {
                printf("Erro ao abrir thread...\n");
            }
#endif
        }

        /// \brief Wait the current thread to terminate their execution scope. Can be called once.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// void thread_example() {
        ///     // the thread or main thread could call #Thread::interrupt() to change this state
        ///     while (!Thread::isCurrentThreadInterrupted()) {
        ///         ...
        ///         //avoid 100% CPU using by this thread
        ///         PlatformPlatform::Sleep::millis(1);
        ///     }
        /// }
        ///
        /// Thread thread( &thread_example );
        /// // start the thread
        /// thread.start();
        /// ...
        /// // inside the thread implementation, it is necessary to check the #Thread::isCurrentThreadInterrupted()
        /// thread.interrupt();
        /// // wait thread execution finish
        /// thread.wait();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void wait()
        {
            // self thread test
#if defined(_WIN32)

            std::unique_lock<decltype(perThreadData.wait_m)> lock(perThreadData.wait_m);

            if (!m_thread)
                return;
            if (m_threadId == GetCurrentThreadId_Custom())
                return;

            // waitCalls++;
            // while (isAlive())
            //     Platform::Sleep::millis(1);
            // waitCalls--;

            waitCalls++;
            while (isAlive())
            {
                perThreadData.wait_cv.wait(lock);
            }
            waitCalls--;
            if (waitCalls <= 0)
                perThreadData.wait_cv.notify_all();

#elif defined(__linux__) || defined(__APPLE__)

            std::unique_lock<decltype(perThreadData.wait_m)> lock(perThreadData.wait_m);

            if (!m_isActive)
                return;
            if (pthread_equal(pthread_self(), m_thread) != 0)
                return;

            waitCalls++;
            while (isAlive())
            {
                perThreadData.wait_cv.wait(lock);
            }
            waitCalls--;
            if (waitCalls <= 0)
                perThreadData.wait_cv.notify_all();
#endif
        }

        bool isAlive()
        {
#if defined(_WIN32)
            return !exited && m_thread != nullptr;
#else
            return !exited && m_isActive;
#endif
        }

        bool isStarted()
        {
            return started;
        }
        void setShouldDisposeThreadByItself(bool v)
        {
            shouldReleaseThreadID_byItself = v;
        }

        /// \brief Force the target thread to terminate. It does not garantee the execution scope to terminate.
        ///
        /// \warning It is dangerous to call this method.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// void thread_example() {
        ///     ...
        /// }
        ///
        /// Thread thread( &thread_example );
        /// // start the thread
        /// thread.start();
        /// ...
        /// thread.terminate();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void terminate()
        {
            /*
            if (m_impl)
            {
                m_impl->terminate();
                delete m_impl;
                m_impl = nullptr;
            }*/

#if defined(_WIN32)
            if (m_thread)
                TerminateThread(m_thread, 0);
            // CLOSE THREAD
            if (m_thread)
                CloseHandle(m_thread);
            m_thread = nullptr;
#else
            if (m_isActive)
            {
#ifndef OS_ANDROID
                pthread_cancel(m_thread);
#else
                // See http://stackoverflow.com/questions/4610086/pthread-cancel-al
                pthread_kill(m_thread, SIGUSR1);
#endif
            }
#endif
        }

        /// \brief Constructor (function)
        ///
        /// Creates a thread associated with a function as its run point.
        ///
        /// example:
        ///
        /// \code
        ///
        /// void thread_function()
        /// {
        ///     ...
        /// }
        ///
        /// Thread thread( &thread_function );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        Thread(const EventCore::Callback<void()> &_runEntryPoint)
        {
            perThreadData.init();

            runEntryPoint = _runEntryPoint;
            interrupted = false;
            exited = false;
            started = false;
            shouldReleaseThreadID_byItself = false;
            waitCalls = 0;
#if defined(_WIN32)
            m_thread = nullptr;
            m_thread_interrupt_event = CreateEvent(
                nullptr,  // default security attributes
                TRUE,  // manual-reset event
                FALSE, // initial state is nonsignaled
                nullptr   // TEXT("WriteEvent")  // object name
            );
            ITK_ABORT(m_thread_interrupt_event == nullptr, "CreateEvent error: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#else
            m_isActive = false;
            m_isMain = false;
            interrupt_thread = nullptr;
            skip_interrupt = false;
#endif
            getMainThread();
        }

        Thread(EventCore::Callback<void()> &&_runEntryPoint)
        {
            perThreadData.init();

            runEntryPoint = std::move(_runEntryPoint);
            interrupted = false;
            exited = false;
            started = false;
            shouldReleaseThreadID_byItself = false;
            waitCalls = 0;
#if defined(_WIN32)
            m_thread = nullptr;
            m_thread_interrupt_event = CreateEvent(
                nullptr,  // default security attributes
                TRUE,  // manual-reset event
                FALSE, // initial state is nonsignaled
                nullptr   // TEXT("WriteEvent")  // object name
            );
            ITK_ABORT(m_thread_interrupt_event == nullptr, "CreateEvent error: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#else
            m_isActive = false;
            m_isMain = false;
            interrupt_thread = nullptr;
            skip_interrupt = false;
#endif
            getMainThread();
        }

    private:
        // constructor for main thread
        Thread()
        {
            perThreadData.init();

            runEntryPoint = nullptr;
            interrupted = false;
            exited = false;
            started = false;
            shouldReleaseThreadID_byItself = false;
            waitCalls = 0;
#if defined(_WIN32)
            m_threadId = GetCurrentThreadId_Custom();
            m_thread = nullptr;
            m_thread_interrupt_event = CreateEvent(
                nullptr,  // default security attributes
                TRUE,  // manual-reset event
                FALSE, // initial state is nonsignaled
                nullptr   // TEXT("WriteEvent")  // object name
            );
            ITK_ABORT(m_thread_interrupt_event == nullptr, "CreateEvent error: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#else
            m_thread = pthread_self();
            m_isActive = false;
            m_isMain = true;
            interrupt_thread = nullptr;
            skip_interrupt = false;

            setShouldDisposeThreadByItself(true);
#endif
        }

    public:

        static int QueryNumberOfSystemThreads()
        {
// https://stackoverflow.com/questions/150355/programmatically-find-the-number-of-cores-on-a-machine
#if defined(_WIN32)
            SYSTEM_INFO sysinfo;
            GetSystemInfo(&sysinfo);
            int numCPU = sysinfo.dwNumberOfProcessors;
            return numCPU;
#elif defined(__linux__)
            int numCPU = sysconf(_SC_NPROCESSORS_ONLN);
            return numCPU;
#elif defined(__APPLE__)
            int mib[4];
            int numCPU;
            std::size_t len = sizeof(numCPU);

            // alternative:
            // sysctlbyname("hw.physicalcpu", &numCPU, &len, 0, 0);
            sysctlbyname("hw.logicalcpu", &numCPU, &len, 0, 0);

            return numCPU;
            /*

            // set the mib for hw.ncpu
            mib[0] = CTL_HW;
            mib[1] = HW_AVAILCPU;  // alternatively, try HW_NCPU;

            // get the number of CPUs from the system
            sysctl(mib, 2, &numCPU, &len, nullptr, 0);

            if (numCPU < 1)
            {
                mib[1] = HW_NCPU;
                sysctl(mib, 2, &numCPU, &len, nullptr, 0);
                if (numCPU < 1)
                    numCPU = 1;
            }
            return numCPU;
            */
#else
#error "QueryNumberOfSystemCores not implemented in the current system"
#endif
            /*
                iOS or newer Macs
                NSUInteger a = [[NSProcessInfo processInfo] processorCount];
                NSUInteger b = [[NSProcessInfo processInfo] activeProcessorCount];
            */
        }
    };

}

#include "Core/ThreadDataSet.inl"
#ifdef __APPLE__
#include "Core/unamed_fake_sem.inl"
#endif
