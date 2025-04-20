#pragma once

#include "../platform_common.h"
#include "../Core/ObjectBuffer.h"
#include "../Thread.h"
#include "../Sleep.h"
#include "../../ITKCommon/Path.h"
#include "../../ITKCommon/ITKAbort.h"
#include "../Signal.h"

namespace Platform
{

    namespace IPC
    {

        // On linux the Semaphore IPC are persistent...
        // you need to create a logic with file_lock (lockf)
        // to ensure the correct initialization
        class SemaphoreIPC: public EventCore::HandleCallback
        {

            Mutex aquireMutex;
            int aquired_count;

            // bool signaled;

#if defined(_WIN32)
            HANDLE semaphore;

            Platform::Mutex close_mutex;

#elif defined(__linux__) || defined(__APPLE__)
            sem_t *semaphore;
#endif

            void OnAbort_SemaphoreIPC(const char *file, int line, const char *message){
                Platform::AutoLock lock(&aquireMutex);
                for(int i=0;i<aquired_count;i++)
                    release();
            }

        public:

            //deleted copy constructor and assign operator, to avoid copy...
            SemaphoreIPC(const SemaphoreIPC &v) = delete;
            SemaphoreIPC& operator=(const SemaphoreIPC &v) = delete;

#if defined(__linux__) || defined(__APPLE__)
            // unlink all resources
            static void force_shm_unlink(const std::string &name)
            {
                //printf("[SemaphoreIPC] force_shm_unlink: %s\n", name.c_str());
                std::string semaphore_name = std::string("/") + std::string(name);
                sem_unlink(semaphore_name.c_str());
            }
#endif

            std::string name;

            SemaphoreIPC(const std::string &name, int count, bool truncate = false, bool on_abort_release_aquired = true)
            {
                aquired_count = 0;
                if (on_abort_release_aquired)
                    ITKCommon::ITKAbort::Instance()->OnAbort.add(&SemaphoreIPC::OnAbort_SemaphoreIPC, this);

                this->name = name;
                // signaled = false;
#if defined(_WIN32)
                SECURITY_DESCRIPTOR sd;
                InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
                SetSecurityDescriptorDacl(&sd, TRUE, (PACL)0, FALSE);

                SECURITY_ATTRIBUTES sa;
                sa.nLength = sizeof(sa);
                sa.lpSecurityDescriptor = &sd;
                sa.bInheritHandle = FALSE;

                semaphore = CreateSemaphoreA(
                    &sa,               // default security attributes
                    count,             // initial count
                    LONG_MAX,          // count,  // maximum count
                    this->name.c_str() // unnamed semaphore
                );
                ITK_ABORT(semaphore == nullptr, "CreateSemaphore error: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#elif defined(__linux__) || defined(__APPLE__)
                // sem_init(&semaphore, 0, count);// 0 means is a semaphore bound to threads

                this->name = std::string("/") + this->name;

                if (truncate)
                {
                    sem_unlink(this->name.c_str());
                }

                semaphore = sem_open(
                    this->name.c_str(),
                    O_CREAT,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
                    count);

                /*
                int v;
                sem_getvalue(semaphore, &v);

                if(truncate) {
                    while (v > count) {
                        sem_wait(semaphore);
                        sem_getvalue(semaphore, &v);
                    }
                    while (v < count) {
                        sem_post(semaphore);
                        sem_getvalue(semaphore, &v);
                    }
                }*/

                // printf("SEMAPHORE VALUE: %i\n",v);

                ITK_ABORT(semaphore == SEM_FAILED, "Error to create global semaphore. Error code: %s\n", strerror(errno));
#endif
            }
            ~SemaphoreIPC()
            {
                ITKCommon::ITKAbort::Instance()->OnAbort.remove(&SemaphoreIPC::OnAbort_SemaphoreIPC, this);

#if defined(_WIN32)
                Platform::AutoLock autoLock(&close_mutex);
                if (semaphore != nullptr)
                    CloseHandle(semaphore);
                semaphore = nullptr;
#elif defined(__linux__) || defined(__APPLE__)
                // sem_destroy(&semaphore);
                if (semaphore != nullptr)
                    sem_close(semaphore);
                // sem_unlink(this->name.c_str());
                semaphore = nullptr;
#endif
            }

            bool tryToAcquire(uint32_t timeout_ms = 0, bool ignore_signal = false)
            {
                Platform::AutoLock lock(&aquireMutex);

                // printf("[Semaphore] tryToAquire...\n");

                Platform::Thread *currentThread = Platform::Thread::getCurrentThread();

#if defined(__linux__) || defined(__APPLE__)
                currentThread->semaphoreLock();
#endif

                // if (signaled || currentThread->isCurrentThreadInterrupted()) {
                if ((!ignore_signal) && isSignaled())
                {
                    // signaled = true;
#if defined(__linux__) || defined(__APPLE__)
                    currentThread->semaphoreUnLock();
#endif

                    bool aquired = false;
                    return aquired;
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
                    bool aquired = false;
                    return false;
                }

                // true if the semaphore is signaled (might have the interrupt or not...)
                bool aquired = (dwWaitResult == WAIT_OBJECT_0 + 0);
                if (aquired)
                    aquired_count++;
                return aquired;
#elif defined(__linux__)
                if (timeout_ms == 0)
                {
                    currentThread->semaphoreUnLock();
                    bool aquired = sem_trywait(semaphore) == 0;
                    if (aquired)
                        aquired_count++;
                    return aquired;
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

                currentThread->semaphoreWaitBegin(semaphore);
                currentThread->semaphoreUnLock();

                // printf("wait for 10 s\n");
                // Platform::Sleep::millis(10000);
                // printf("10 s done...\n");

                int s = sem_timedwait(semaphore, &ts);

                if (ignore_signal){
                    while ((s == -1 && errno != ETIMEDOUT))
                        s = sem_timedwait(semaphore, &ts);
                }

                currentThread->semaphoreWaitDone(semaphore);

                // currentThread->isCurrentThreadInterrupted() ||
                if ((s == -1 && errno != ETIMEDOUT))
                {
                    // printf(". s: %s \n", strerror(errno));

                    // interrupt signaled
                    // signaled = true;
                    bool aquired = false;
                    return aquired;
                }

                bool aquired = (s == 0);
                if (aquired)
                    aquired_count++;
                return aquired;
#elif defined(__APPLE__)
                if (timeout_ms == 0)
                {
                    currentThread->semaphoreUnLock();
                    bool aquired = sem_trywait(semaphore) == 0;
                    if (aquired)
                        aquired_count++;
                    return aquired;
                }

                currentThread->semaphoreWaitBegin(semaphore);
                currentThread->semaphoreUnLock();

                int s = -1;
                int timeout_int = (int)timeout_ms;
                while (
                    (ignore_signal || !currentThread->isCurrentThreadInterrupted())
                    && timeout_int > 0)
                {
                    s = sem_trywait(semaphore);
                    if (s == 0)
                        break;
                    timeout_int -= 1;
                    Platform::Sleep::millis(1);
                }

                currentThread->semaphoreWaitDone(semaphore);

                bool aquired = (s == 0);
                if (aquired)
                    aquired_count++;
                return aquired;

                // if (timeout_ms == 0)
                // {
                //     currentThread->semaphoreUnLock();
                //     bool aquired = sem_trywait(semaphore) == 0;
                //     if (aquired)
                //         aquired_count++;
                //     return aquired;
                // }

                // struct timespec ts;
                // if (clock_gettime(CLOCK_REALTIME, &ts))
                // {
                //     currentThread->semaphoreUnLock();
                //     ITK_ABORT(true, "clock_gettime error\n");
                // }

                // struct timespec ts_increment;
                // ts_increment.tv_sec = timeout_ms / 1000;
                // ts_increment.tv_nsec = ((long)timeout_ms % 1000L) * 1000000L;

                // ts.tv_nsec += ts_increment.tv_nsec;
                // ts.tv_sec += ts.tv_nsec / 1000000000L;
                // ts.tv_nsec = ts.tv_nsec % 1000000000L;

                // ts.tv_sec += ts_increment.tv_sec;

                // currentThread->semaphoreWaitBegin(semaphore);
                // currentThread->semaphoreUnLock();

                // // printf("wait for 10 s\n");
                // // Platform::Sleep::millis(10000);
                // // printf("10 s done...\n");

                // int s = sem_timedwait(semaphore, &ts);

                // if (ignore_signal){
                //     while ((s == -1 && errno != ETIMEDOUT))
                //         s = sem_timedwait(semaphore, &ts);
                // }

                // currentThread->semaphoreWaitDone(semaphore);

                // // currentThread->isCurrentThreadInterrupted() ||
                // if ((s == -1 && errno != ETIMEDOUT))
                // {
                //     // printf(". s: %s \n", strerror(errno));

                //     // interrupt signaled
                //     // signaled = true;
                //     bool aquired = false;
                //     return aquired;
                // }

                // bool aquired = (s == 0);
                // if (aquired)
                //     aquired_count++;
                // return aquired;
#endif
            }

            bool blockingAcquire(bool ignore_signal = false)
            {
                Platform::AutoLock lock(&aquireMutex);

#if defined(_WIN32)
                bool signaled = (!ignore_signal) && isSignaled();
                while (!signaled && !tryToAcquire(UINT32_MAX, ignore_signal))
                {
                    signaled = (!ignore_signal) && isSignaled();
                }

                bool aquired = !signaled;
                if (aquired)
                    aquired_count++;
                return aquired;
                /*
                if (signaled || Platform::Thread::getCurrentThread()->isCurrentThreadInterrupted())
                    signaled = true;
                else
                    while (!signaled && !tryToAcquire(UINT32_MAX));
                return !signaled;
                */
#elif defined(__linux__) || defined(__APPLE__)
                Platform::Thread *currentThread = Platform::Thread::getCurrentThread();

                /*
                if (signaled || Platform::Thread::getCurrentThread()->isCurrentThreadInterrupted())
                    signaled = true;
                else {
                    while (!signaled && !tryToAcquire( 60000 )) { //UINT32_MAX
                        //Platform::Sleep::millis(1);
                        //printf(". signaled: %i \n", signaled);
                    }
                }
                // */

                /*

                currentThread->semaphoreLock();
                if (signaled || currentThread->isCurrentThreadInterrupted()) {
                    signaled = true;
                    currentThread->semaphoreUnLock();
                } else {
                    currentThread->semaphoreWaitBegin(semaphore);
                    currentThread->semaphoreUnLock();
                    signaled = signaled || (sem_wait(semaphore) != 0);
                    currentThread->semaphoreWaitDone(semaphore);
                    //signaled = signaled || currentThread->isCurrentThreadInterrupted();
                }

                // */

                currentThread->semaphoreLock();

                bool signaled = (!ignore_signal) && isSignaled();

                if (signaled)
                {
                    // signaled = true;
                    currentThread->semaphoreUnLock();
                }
                else
                {
                    currentThread->semaphoreWaitBegin(semaphore);
                    currentThread->semaphoreUnLock();
                    
                    //signaled = signaled || (sem_wait(semaphore) != 0);
                    signaled = (sem_wait(semaphore) != 0);
                    if (ignore_signal){
                        while(signaled)
                            signaled = (sem_wait(semaphore) != 0);
                    }

                    currentThread->semaphoreWaitDone(semaphore);
                    // signaled = signaled || currentThread->isCurrentThreadInterrupted();
                }

                bool aquired = !signaled;
                if (aquired)
                    aquired_count++;
                return aquired;
#endif
            }

            void release()
            {
                Platform::AutoLock lock(&aquireMutex);

                if (aquired_count > 0)
                    aquired_count--;

#if defined(_WIN32)
                // printf("[Semaphore] release...\n");
                BOOL result = ReleaseSemaphore(semaphore, 1, nullptr);
                ITK_ABORT(!result, "ReleaseSemaphore error: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#elif defined(__linux__) || defined(__APPLE__)
                sem_post(semaphore);
#endif
            }

            // only check if this queue is signaled for the current thread...
            // it may be active in another thread...
            bool isSignaled() const
            {
                return Platform::Thread::isCurrentThreadInterrupted();
                // return signaled;
            }

            void forceUnlinkLinux()
            {

#if !defined(_WIN32)
                if (semaphore != nullptr)
                    sem_unlink(name.c_str());
#endif
            }

            void forceCloseWindows()
            {

#if defined(_WIN32)
                /*Platform::AutoLock autoLock(&close_mutex);
                if (semaphore != nullptr) {
                    CloseHandle(semaphore);
                    semaphore = nullptr;
                }*/
#endif
            }
        };

    }

}