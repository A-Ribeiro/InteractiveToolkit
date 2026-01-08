#pragma once

#include "../EventCore/Callback.h"
#include "Core/ObjectQueue.h"
#include "Thread.h"

namespace Platform
{

    class ThreadPool : public EventCore::HandleCallback
    {
    public:
        using CallbackType = typename EventCore::Callback<void()>;

    private:
        std::vector<Platform::Thread *> threads;
        Platform::ObjectQueue<CallbackType> tasks;

        bool finish_all_tasks_before_finish;

        void run()
        {
            if (finish_all_tasks_before_finish)
            {
                bool value_not_readed = false;
                while (!Platform::Thread::isCurrentThreadInterrupted() || tasks.size() > 0)
                {
                    CallbackType task_callback = tasks.dequeue(&value_not_readed, true);
                    if (value_not_readed)
                        break;
                    task_callback();
                }
            }
            else
            {
                bool is_signaled = false;
                while (!Platform::Thread::isCurrentThreadInterrupted())
                {
                    CallbackType task_callback = tasks.dequeue(&is_signaled);
                    if (is_signaled)
                        break;
                    task_callback();
                }
            }
        }

    public:
        ThreadPool(int count = -1, bool finish_all_tasks_before_finish = true)
        {
            this->finish_all_tasks_before_finish = finish_all_tasks_before_finish;
            // avoid lock entire OS
            if (count == -1)
                count = Platform::Thread::QueryNumberOfSystemThreads() - 1;
            if (count <= 0)
                count = 1;

            for (int i = 0; i < count; i++)
            {
                Platform::Thread *thread = new Platform::Thread(
                    EventCore::CallbackWrapper(&ThreadPool::run, this));
                threads.push_back(thread);
                thread->start();
            }

            printf("ThreadPool created with: %i threads\n", count);
        }

        ~ThreadPool()
        {
            finish();
        }

        void postTask(const CallbackType &task)
        {
            tasks.enqueue(task);
        }

        uint32_t taskInQueue()
        {
            return tasks.size();
        }

        void finish()
        {
            // printf("interrupting threads\n");
            for (auto thread : threads)
                thread->interrupt();

            // printf("deleting threads\n");
            for (auto thread : threads)
                delete thread;
            threads.clear();
        }

        int threadCount() const
        {
            return (int)threads.size();
        }
    };

}
