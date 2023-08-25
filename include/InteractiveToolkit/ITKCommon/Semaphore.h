#pragma once

#include "../common.h"

namespace ITKCommon
{

    class Semaphore
    {
        std::mutex mtx;
        std::condition_variable contition;
        uint32_t count = 0;

    public:
        void release()
        {
            std::lock_guard<decltype(mtx)> lock(mtx);
            ++count;
            contition.notify_one();
        }

        void acquire()
        {
            std::unique_lock<decltype(mtx)> lock(mtx);
            while (!count) // spurious wake-ups
                contition.wait(lock);
            --count;
        }

        bool try_acquire()
        {
            std::lock_guard<decltype(mtx)> lock(mtx);
            if (count)
            {
                --count;
                return true;
            }
            return false;
        }
    };

}
