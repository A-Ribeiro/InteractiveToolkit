#pragma once

#include "../platform_common.h"
#include "BufferIPC.h"
#include "SemaphoreIPC.h"
#include "AutoLockSemaphoreIPC.h"

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

namespace Platform
{
    namespace IPC
    {
        namespace Internal
        {
            const int CONDITION_IPC_MAX_WAITS = 8;

            struct ConditionIPC_SharedBuffer
            {
                int notifiable_count;
                int released_count;

                int notifiable_list[CONDITION_IPC_MAX_WAITS];
                int released_list[CONDITION_IPC_MAX_WAITS];

                // char selected_semaphore_name[64];
                // bool has_any_semaphore_selected;

                int circular_count;
            };

            class ConditionIPC_Controller
            {

                // buffer operations

                bool notifiable_can_push()
                {
                    return buffer_ptr->notifiable_count < CONDITION_IPC_MAX_WAITS;
                }

                void notifiable_push(int sem_position)
                {
                    buffer_ptr->notifiable_list[buffer_ptr->notifiable_count] = sem_position;
                    buffer_ptr->notifiable_count++;
                }

                int notifiable_get_first()
                {
                    if (!notifiable_has_any())
                        return -1;
                    int result = buffer_ptr->notifiable_list[0];
                    notifiable_remove_at(0);
                    return result;
                }

                bool notifiable_has_any()
                {
                    return buffer_ptr->notifiable_count > 0;
                }

                int notifiable_find(int sem_position)
                {
                    for (int i = 0; i < buffer_ptr->notifiable_count; i++)
                    {
                        if (buffer_ptr->notifiable_list[i] == sem_position)
                            return i;
                    }
                    return -1;
                }

                void notifiable_remove_at(int index)
                {
                    for (int i = index; i < buffer_ptr->notifiable_count - 1; i++)
                        buffer_ptr->notifiable_list[i] = buffer_ptr->notifiable_list[i + 1];
                    buffer_ptr->notifiable_count--;
                }

                bool released_can_push()
                {
                    return buffer_ptr->released_count < CONDITION_IPC_MAX_WAITS;
                }

                void released_push(int sem_position)
                {
                    buffer_ptr->released_list[buffer_ptr->released_count] = sem_position;
                    buffer_ptr->released_count++;
                }

                int released_find(int sem_position)
                {
                    for (int i = 0; i < buffer_ptr->released_count; i++)
                    {
                        if (buffer_ptr->released_list[i] == sem_position)
                            return i;
                    }
                    return -1;
                }

                void released_remove_at(int index)
                {
                    for (int i = index; i < buffer_ptr->released_count - 1; i++)
                        buffer_ptr->released_list[i] = buffer_ptr->released_list[i + 1];
                    buffer_ptr->released_count--;
                }

            public:
                BufferIPC bufferIPC;
                ConditionIPC_SharedBuffer *buffer_ptr;
                std::vector<SemaphoreIPC *> semaphoresIPC;

                // SemaphoreIPC *selectedSemaphore;

                Mutex mtx;

                ConditionIPC_Controller(const std::string &name) : bufferIPC(
                                                                       name.c_str(),
                                                                       sizeof(ConditionIPC_SharedBuffer))
                {
                    buffer_ptr = (ConditionIPC_SharedBuffer *)bufferIPC.data;
                    if (bufferIPC.isFirstProcess())
                    {
                        // initialize the global data
                        memset(buffer_ptr, 0, sizeof(ConditionIPC_SharedBuffer));

                        // creating needed semaphores and truncate it
                        for (int i = 0; i < CONDITION_IPC_MAX_WAITS; i++)
                        {
                            char aux[64];
                            snprintf(aux, 64, "%s_s_%i", name.c_str(), i);
                            semaphoresIPC.push_back(new SemaphoreIPC(aux, 0, true));
                        }
                    }
                    else
                    {
                        // creating needed semaphores and keep the original value
                        for (int i = 0; i < CONDITION_IPC_MAX_WAITS; i++)
                        {
                            char aux[64];
                            snprintf(aux, 64, "%s_s_%i", name.c_str(), i);
                            semaphoresIPC.push_back(new SemaphoreIPC(aux, 0, false));
                        }
                    }
                    bufferIPC.finishInitialization();

#if defined(__linux__) || defined(__APPLE__)
                    bufferIPC.OnLastBufferFree.add([](BufferIPC *bufferIPC)
                                                   {
                            for (int i = 0; i < CONDITION_IPC_MAX_WAITS; i++)
                            {
                                char aux[64];
                                snprintf(aux, 64, "%s_s_%i", bufferIPC->name.c_str(), i);
                                SemaphoreIPC::force_shm_unlink(aux);
                            } });
#endif
                }

                ~ConditionIPC_Controller()
                {
                    for (auto semaphoreIPC : semaphoresIPC)
                        delete semaphoreIPC;
                    semaphoresIPC.clear();
                }

                bool inUse(int semaphore_index)
                {
                    for (int i = 0; i < buffer_ptr->released_count; i++)
                        if (buffer_ptr->released_list[i] == semaphore_index)
                            return true;
                    for (int i = 0; i < buffer_ptr->notifiable_count; i++)
                        if (buffer_ptr->notifiable_list[i] == semaphore_index)
                            return true;
                    return false;
                }

                int getNextSemaphore()
                {
                    AutoLock lk(&mtx);

                    bufferIPC.lock();

                    if (!notifiable_can_push())
                    {
                        bufferIPC.unlock();
                        ITK_ABORT(true, "ConditionIPC - Max waits reached: %i", buffer_ptr->notifiable_count);
                    }

                    int next_to_return = buffer_ptr->circular_count;
                    while (inUse(next_to_return))
                        next_to_return = (next_to_return + 1) % CONDITION_IPC_MAX_WAITS;

                    // buffer_ptr->circular_count = (buffer_ptr->circular_count + 1) % CONDITION_IPC_MAX_WAITS;
                    buffer_ptr->circular_count = (next_to_return + 1) % CONDITION_IPC_MAX_WAITS;

                    // add to notifiable list
                    // buffer_ptr->notifiable_list[buffer_ptr->notifiable_count] = next_to_return;
                    // buffer_ptr->notifiable_count++;
                    notifiable_push(next_to_return);

                    bufferIPC.unlock();

                    // printf("(+)next semaphore: %i\n", next_to_return);

                    return next_to_return;
                }

                void releaseSemaphore(int semaphore_index, bool *signaled, bool external_mutex_signaled)
                {
                    // printf("(+)releasing semaphore: %i\n", semaphore_index);

                    bool _aquired = !(*signaled);

                    AutoLock lk(&mtx);

                    bufferIPC.lock();

                    // check semaphore index is at released
                    int released_index = released_find(semaphore_index);
                    if (released_index != -1)
                    {
                        released_remove_at(released_index);
                        *signaled = false;
                        if (!_aquired)
                            semaphoresIPC[semaphore_index]->blockingAcquire(true);
                    }

                    {
                        // check semaphore index is at notifiable and release it
                        int notifiable_index = notifiable_find(semaphore_index);
                        if (notifiable_index != -1)
                        {
                            notifiable_remove_at(notifiable_index);
                        }
                    }

                    if (!(*signaled) && external_mutex_signaled)
                    {
                        // in case not signaled...
                        //  we need to notify one removed the released and notifiable

                        if (notifiable_has_any())
                        {
                            // notify one
                            {
                                int sem_to_notify = notifiable_get_first();

                                if (!released_can_push())
                                {
                                    ITK_ABORT(true, "ConditionIPC - Max released reached: %i", buffer_ptr->released_count);
                                    bufferIPC.unlock();
                                    return;
                                }

                                released_push(sem_to_notify);
                                semaphoresIPC[sem_to_notify]->release();
                            }
                        }
                    }

                    bufferIPC.unlock();
                }

                void notify()
                {
                    // printf("notify\n");
                    AutoLock lk(&mtx);

                    bufferIPC.lock();

                    // check has any active notifiable
                    if (!notifiable_has_any())
                    {
                        bufferIPC.unlock();
                        return;
                    }

                    // notify one
                    {
                        int sem_to_notify = notifiable_get_first();

                        if (!released_can_push())
                        {
                            ITK_ABORT(true, "ConditionIPC - Max released reached: %i", buffer_ptr->released_count);
                            bufferIPC.unlock();
                            return;
                        }

                        released_push(sem_to_notify);
                        semaphoresIPC[sem_to_notify]->release();
                    }
                    // selectedSemaphore->release();
                    bufferIPC.unlock();
                }

                void notify_all()
                {
                    // printf("notify_all\n");
                    AutoLock lk(&mtx);

                    bufferIPC.lock();

                    // check has any active notifiable
                    if (!notifiable_has_any())
                    {
                        bufferIPC.unlock();
                        return;
                    }

                    // notify all
                    while (notifiable_has_any())
                    {
                        // printf("notifiable_count: %i\n", buffer_ptr->notifiable_count);
                        int sem_to_notify = notifiable_get_first();

                        if (!released_can_push())
                        {
                            ITK_ABORT(true, "ConditionIPC - Max released reached: %i", buffer_ptr->released_count);
                            bufferIPC.unlock();
                            return;
                        }

                        released_push(sem_to_notify);
                        semaphoresIPC[sem_to_notify]->release();
                    }

                    // selectedSemaphore->release();
                    bufferIPC.unlock();
                }
            };
        }

        class ConditionIPC
        {
            Internal::ConditionIPC_Controller controller;

        public:
#if defined(__linux__) || defined(__APPLE__)
            // unlink all resources
            static void force_shm_unlink(const std::string &name)
            {
                printf("[ConditionIPC] force_shm_unlink: %s\n", name.c_str());
                BufferIPC::force_shm_unlink(name + std::string("_c_v"));
            }
#endif

            std::string name;

            BufferIPC *bufferIPC()
            {
                return &controller.bufferIPC;
            }

            ConditionIPC(const std::string &name) : controller(name + std::string("_c_v"))
            {
                this->name = name;
            }

            void wait(SemaphoreIPC *mutex_semaphore, bool *_signaled = NULL)
            {
                int this_semaphore = controller.getNextSemaphore();

                mutex_semaphore->release();

                bool signaled = !controller.semaphoresIPC[this_semaphore]->blockingAcquire();
                bool mutex_signaled = !mutex_semaphore->blockingAcquire();
                if (mutex_signaled)
                {
                    // if (!signaled)
                    //     controller.semaphoresIPC[this_semaphore]->release();

                    controller.releaseSemaphore(this_semaphore, &signaled, true);

                    if (_signaled != NULL)
                        *_signaled = true;
                    return;
                }

                controller.releaseSemaphore(this_semaphore, &signaled, false);

                if (_signaled != NULL)
                    *_signaled = signaled;

                if (signaled){
                    // allow call cancelAutoRelease on external AutoLockSemaphoreIPC
                    mutex_semaphore->release();
                }
            }

            void wait_for(SemaphoreIPC *mutex_semaphore, uint32_t timeout_ms, bool *_signaled = NULL)
            {
                int this_semaphore = controller.getNextSemaphore();

                mutex_semaphore->release();

                bool aquired = controller.semaphoresIPC[this_semaphore]->tryToAcquire(timeout_ms);
                bool signaled = false;
                if (!aquired)
                    signaled = controller.semaphoresIPC[this_semaphore]->isSignaled();

                bool mutex_signaled = !mutex_semaphore->blockingAcquire();

                if (mutex_signaled)
                {
                    // if (!signaled)
                    // if (aquired && signaled)
                    //     controller.semaphoresIPC[this_semaphore]->release();

                    controller.releaseSemaphore(this_semaphore, &signaled, true);

                    if (_signaled != NULL)
                        *_signaled = true;
                    return;
                }

                controller.releaseSemaphore(this_semaphore, &signaled, false);

                if (_signaled != NULL)
                    *_signaled = signaled;
                
                if (signaled){
                    // allow call cancelAutoRelease on external AutoLockSemaphoreIPC
                    mutex_semaphore->release();
                }
            }

            void notify()
            {
                controller.notify();
            }

            void notify_all()
            {
                controller.notify_all();
            }
        };

    }

}

#if defined(_WIN32)
#pragma warning(pop)
#endif