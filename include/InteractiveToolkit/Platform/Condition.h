#pragma once

//#include "platform_common.h"
#include "../common.h"
#include "AutoLock.h"
#include "Semaphore.h"
#include "Mutex.h"
#include "Core/ObjectPool.h"

namespace Platform
{

    class Condition
    {
        Mutex internal_lock;
        std::vector<Semaphore *> notifiable;
        std::vector<Semaphore *> released;
        Mutex *mtx;

        Platform::ObjectPool<Semaphore> local_semaphores;

    public:
        Condition()
        {
            mtx = NULL;
        }

        void wait(Mutex *mutex, bool *_signaled = NULL)
        {
            //Semaphore *this_semaphore = new Semaphore(0);

            Semaphore *this_semaphore = local_semaphores.create(true);

            {
                AutoLock lk(&internal_lock);
                mtx = mutex;
                notifiable.push_back(this_semaphore);
            }

            mutex->unlock();

            bool signaled = !this_semaphore->blockingAcquire();

            mutex->lock();

            {
                AutoLock lk(&internal_lock);

                auto it = std::find(released.begin(), released.end(), this_semaphore);
                if (it != released.end())
                {
                    //the semaphore was released before the thread interrupt/signaling...
                    released.erase(it);
                    
                    //force blocking aquire to make semaphore state to 0
                    if (signaled)
                        this_semaphore->blockingAcquire(true);

                    signaled = false;
                }

                if (_signaled != NULL)
                    *_signaled = signaled;

                if (signaled)
                {
                    auto it = std::find(notifiable.begin(), notifiable.end(), this_semaphore);
                    if (it != notifiable.end())
                        notifiable.erase(it);

                    if (notifiable.size() == 0)
                        mtx = NULL;
                }
            }

            //delete this_semaphore;
            local_semaphores.release(this_semaphore);
        }

        void wait_for(Mutex *mutex, uint32_t timeout_ms, bool *_signaled = NULL)
        {

            Semaphore *this_semaphore = new Semaphore(0);

            {
                AutoLock lk(&internal_lock);
                mtx = mutex;
                notifiable.push_back(this_semaphore);
            }

            mutex->unlock();

            bool aquired = this_semaphore->tryToAcquire(timeout_ms);
            bool signaled = false;
            if (!aquired)
                signaled = this_semaphore->isSignaled();

            mutex->lock();

            {
                AutoLock lk(&internal_lock);

                auto it = std::find(released.begin(), released.end(), this_semaphore);
                if (it != released.end())
                {
                    //the semaphore was released before the thread interrupt/signaling...
                    released.erase(it);
                    signaled = false;
                }

                if (_signaled != NULL)
                    *_signaled = signaled;

                if (signaled || !aquired)
                {
                    auto it = std::find(notifiable.begin(), notifiable.end(), this_semaphore);
                    if (it != notifiable.end())
                        notifiable.erase(it);

                    if (notifiable.size() == 0)
                        mtx = NULL;
                }
            }

            delete this_semaphore;
        }

        void notify()
        {
            AutoLock lk(&internal_lock);

            if (mtx == NULL)
                return;

            AutoLock lock(mtx);

            if (notifiable.size() == 0)
            {
                mtx = NULL;
                return;
            }

            auto sem_to_notify = *notifiable.begin();
            notifiable.erase(notifiable.begin());
            released.push_back(sem_to_notify);
            sem_to_notify->release();

            if (notifiable.size() == 0)
                mtx = NULL;
        }

        void notify_all()
        {
            AutoLock lk(&internal_lock);

            if (mtx == NULL)
                return;

            AutoLock lock(mtx);

            if (notifiable.size() == 0)
            {
                mtx = NULL;
                return;
            }

            for (auto sem_to_notify : notifiable)
            {
                sem_to_notify->release();
                released.push_back(sem_to_notify);
            }
            notifiable.clear();

            mtx = NULL;
        }
    };

}
