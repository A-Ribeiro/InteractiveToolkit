#pragma once

#include "../platform_common.h"
#include "BufferIPC.h"
#include "SemaphoreIPC.h"
#include "AutoLockSemaphoreIPC.h"


#if defined(_WIN32)
#pragma warning( push )
#pragma warning( disable : 4996)
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

                char selected_semaphore_name[64];

                int circular_count;
            };

            class ConditionIPC_Controller
            {

                // void ensureSemaphoreCreation(const char *name)
                // {

                //     // already created and equals
                //     if (selectedSemaphore != NULL && strcmp(selectedSemaphore->name.c_str(), name) == 0)
                //         return;

                //     // else, needs to create a new one...
                //     if (selectedSemaphore != NULL)
                //     {
                //         delete selectedSemaphore;
                //         selectedSemaphore = NULL;
                //     }
                //     selectedSemaphore = new SemaphoreIPC(name, 0, false);
                // }

            public:
                BufferIPC bufferIPC;
                ConditionIPC_SharedBuffer *buffer_ptr;
                std::vector<SemaphoreIPC *> semaphoresIPC;

                //SemaphoreIPC *selectedSemaphore;

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

                        //selectedSemaphore = NULL;
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

                        // check has selected semaphore
                        // if (strlen(buffer_ptr->selected_semaphore_name) == 0)
                        //     selectedSemaphore = NULL;
                        // else
                        //     selectedSemaphore = new SemaphoreIPC(buffer_ptr->selected_semaphore_name, 0, false);
                    }
                    bufferIPC.finishInitialization();

                    #if defined(__linux__) || defined(__APPLE__)
                        bufferIPC.OnLastBufferFree.add([](BufferIPC*bufferIPC){
                            for (int i = 0; i < CONDITION_IPC_MAX_WAITS; i++)
                            {
                                char aux[64];
                                snprintf(aux, 64, "%s_s_%i", bufferIPC->name.c_str(), i);
                                SemaphoreIPC::force_shm_unlink(aux);
                            }
                        });
                    #endif
                }

                ~ConditionIPC_Controller()
                {
                    for (auto semaphoreIPC : semaphoresIPC)
                        delete semaphoreIPC;
                    semaphoresIPC.clear();

                    // if (selectedSemaphore != NULL)
                    // {
                    //     delete selectedSemaphore;
                    //     selectedSemaphore = NULL;
                    // }
                }

                bool inUse(int semaphore_index) {
                    for (int i = 0; i < buffer_ptr->released_count; i++)
                        if (buffer_ptr->released_list[i] == semaphore_index)
                            return true;
                    for (int i = 0; i < buffer_ptr->notifiable_count; i++)
                        if (buffer_ptr->notifiable_list[i] == semaphore_index)
                            return true;
                    return false;
                }

                int getNextSemaphore(SemaphoreIPC *semaphore_to_select)
                {
                    AutoLock lk(&mtx);

                    bufferIPC.lock();

                    if (buffer_ptr->notifiable_count >= CONDITION_IPC_MAX_WAITS)
                    {
                        bufferIPC.unlock();
                        ITK_ABORT(true, "ConditionIPC - Max waits reached: %i", buffer_ptr->notifiable_count);
                    }

                    int next_to_return = buffer_ptr->circular_count;
                    while (inUse(next_to_return))
                        next_to_return = (next_to_return + 1) % CONDITION_IPC_MAX_WAITS;

                    //buffer_ptr->circular_count = (buffer_ptr->circular_count + 1) % CONDITION_IPC_MAX_WAITS;
                    buffer_ptr->circular_count = (next_to_return + 1) % CONDITION_IPC_MAX_WAITS;

                    // add to notifiable list
                    buffer_ptr->notifiable_list[buffer_ptr->notifiable_count] = next_to_return;
                    buffer_ptr->notifiable_count++;

                    // select the semaphore
                    if (strcmp(buffer_ptr->selected_semaphore_name, semaphore_to_select->name.c_str()) != 0)
                        snprintf(buffer_ptr->selected_semaphore_name, 64 , "%s", semaphore_to_select->name.c_str());

                    //ensureSemaphoreCreation(buffer_ptr->selected_semaphore_name);

                    bufferIPC.unlock();

                    return next_to_return;
                }

                void releaseSemaphore(int semaphore_index, bool *signaled, bool force_notify_to_another_process)
                {
                    bool _aquired = !(*signaled);

                    AutoLock lk(&mtx);

                    bufferIPC.lock();

                    // check semaphore index is at released
                    for (int i = 0; i < buffer_ptr->released_count; i++)
                    {
                        if (buffer_ptr->released_list[i] == semaphore_index)
                        {
                            *signaled = false;
                            for (int j = i; j < buffer_ptr->released_count - 1; j++)
                                buffer_ptr->released_list[j] = buffer_ptr->released_list[j + 1];
                            buffer_ptr->released_count--;

                            // force aquire to keep semaphore with count 0
                            if (!_aquired){
                                semaphoresIPC[semaphore_index]->blockingAcquire(true);
                            }

                            break;
                        }
                    }

                    if (*signaled)
                    {

                        // check semaphore index is at notifiable
                        for (int i = 0; i < buffer_ptr->notifiable_count; i++)
                        {
                            if (buffer_ptr->notifiable_list[i] == semaphore_index)
                            {
                                for (int j = i; j < buffer_ptr->notifiable_count - 1; j++)
                                    buffer_ptr->notifiable_list[j] = buffer_ptr->notifiable_list[j + 1];
                                buffer_ptr->notifiable_count--;
                                break;
                            }
                        }

                        if (buffer_ptr->notifiable_count == 0){
                            //sprintf(buffer_ptr->selected_semaphore_name, "");
                            buffer_ptr->selected_semaphore_name[0] = 0;
                        }
                    } else if(force_notify_to_another_process) {

                        // forward notify to another process

                        if (buffer_ptr->notifiable_count == 0)
                        {
                            //sprintf(buffer_ptr->selected_semaphore_name, "");
                            buffer_ptr->selected_semaphore_name[0] = 0;
                            //selectedSemaphore->release();
                            bufferIPC.unlock();
                            return;
                        }

                        // notify one
                        {
                            int sem_to_notify = buffer_ptr->notifiable_list[0];

                            for (int i = 0; i < buffer_ptr->notifiable_count - 1; i++)
                                buffer_ptr->notifiable_list[i] = buffer_ptr->notifiable_list[i + 1];
                            buffer_ptr->notifiable_count--;

                            buffer_ptr->released_list[buffer_ptr->released_count] = sem_to_notify;
                            buffer_ptr->released_count++;

                            semaphoresIPC[sem_to_notify]->release();
                        }

                        if (buffer_ptr->notifiable_count == 0)
                        {
                            //sprintf(buffer_ptr->selected_semaphore_name, "");
                            buffer_ptr->selected_semaphore_name[0] = 0;
                            //selectedSemaphore->release();
                            bufferIPC.unlock();
                            return;
                        }


                    }

                    bufferIPC.unlock();
                }

                void notify()
                {
                    AutoLock lk(&mtx);

                    bufferIPC.lock();

                    // check has selected semaphore
                    if (strlen(buffer_ptr->selected_semaphore_name) == 0)
                    {
                        bufferIPC.unlock();
                        return;
                    }

                    // check semaphore creation
                    //ensureSemaphoreCreation(buffer_ptr->selected_semaphore_name);

                    // bool signaled = !selectedSemaphore->blockingAcquire();

                    // if (signaled)
                    // {
                    //     bufferIPC.unlock();
                    //     return;
                    // }

                    if (buffer_ptr->notifiable_count == 0)
                    {
                        //sprintf(buffer_ptr->selected_semaphore_name, "");
                        buffer_ptr->selected_semaphore_name[0] = 0;
                        //selectedSemaphore->release();
                        bufferIPC.unlock();
                        return;
                    }

                    // notify one
                    {
                        int sem_to_notify = buffer_ptr->notifiable_list[0];

                        for (int i = 0; i < buffer_ptr->notifiable_count - 1; i++)
                            buffer_ptr->notifiable_list[i] = buffer_ptr->notifiable_list[i + 1];
                        buffer_ptr->notifiable_count--;

                        buffer_ptr->released_list[buffer_ptr->released_count] = sem_to_notify;
                        buffer_ptr->released_count++;

                        semaphoresIPC[sem_to_notify]->release();
                    }

                    if (buffer_ptr->notifiable_count == 0)
                    {
                        //sprintf(buffer_ptr->selected_semaphore_name, "");
                        buffer_ptr->selected_semaphore_name[0] = 0;
                        //selectedSemaphore->release();
                        bufferIPC.unlock();
                        return;
                    }

                    //selectedSemaphore->release();
                    bufferIPC.unlock();
                }

                void notify_all()
                {
                    AutoLock lk(&mtx);

                    bufferIPC.lock();

                    // check has selected semaphore
                    if (strlen(buffer_ptr->selected_semaphore_name) == 0)
                    {
                        bufferIPC.unlock();
                        return;
                    }

                    // check semaphore creation
                    // ensureSemaphoreCreation(buffer_ptr->selected_semaphore_name);

                    // bool signaled = !selectedSemaphore->blockingAcquire();

                    // if (signaled)
                    // {
                    //     bufferIPC.unlock();
                    //     return;
                    // }

                    if (buffer_ptr->notifiable_count == 0)
                    {
                        //sprintf(buffer_ptr->selected_semaphore_name, "");
                        buffer_ptr->selected_semaphore_name[0] = 0;
                        //selectedSemaphore->release();
                        bufferIPC.unlock();
                        return;
                    }

                    // notify all
                    {
                        for (int i = 0; i < buffer_ptr->notifiable_count; i++)
                        {
                            int sem_to_notify = buffer_ptr->notifiable_list[i];
                            buffer_ptr->released_list[buffer_ptr->released_count] = sem_to_notify;
                            buffer_ptr->released_count++;
                            semaphoresIPC[sem_to_notify]->release();
                        }
                        buffer_ptr->notifiable_count = 0;
                    }

                    if (buffer_ptr->notifiable_count == 0)
                    {
                        //sprintf(buffer_ptr->selected_semaphore_name, "");
                        buffer_ptr->selected_semaphore_name[0] = 0;
                        //selectedSemaphore->release();
                        bufferIPC.unlock();
                        return;
                    }

                    //selectedSemaphore->release();
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

            BufferIPC* bufferIPC() {
                return &controller.bufferIPC;
            }

            ConditionIPC(const std::string &name) : controller(name + std::string("_c_v"))
            {
                this->name = name;
            }

            void wait(SemaphoreIPC *mutex_semaphore, bool *_signaled = NULL)
            {
                int this_semaphore = controller.getNextSemaphore(mutex_semaphore);

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
            }

            void wait_for(SemaphoreIPC *mutex_semaphore, uint32_t timeout_ms, bool *_signaled = NULL)
            {
                int this_semaphore = controller.getNextSemaphore(mutex_semaphore);

                mutex_semaphore->release();

                bool aquired = controller.semaphoresIPC[this_semaphore]->tryToAcquire(timeout_ms);
                bool signaled = false;
                if (!aquired)
                    signaled = controller.semaphoresIPC[this_semaphore]->isSignaled();

                bool mutex_signaled = !mutex_semaphore->blockingAcquire();

                if (mutex_signaled)
                {
                    //if (!signaled)
                    if (aquired)
                        controller.semaphoresIPC[this_semaphore]->release();

                    controller.releaseSemaphore(this_semaphore, &signaled, true);

                    if (_signaled != NULL)
                        *_signaled = true;
                    return;
                }

                controller.releaseSemaphore(this_semaphore, &signaled, false);

                if (_signaled != NULL)
                    *_signaled = signaled;
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
#pragma warning( pop )
#endif