#pragma once

#include "../common.h"

namespace ITKCommon
{

    class STLSemaphore
    {
        std::mutex mtx;
        std::condition_variable contition;
        uint32_t count = 0;

    public:
        void release()
        {
            std::lock_guard<decltype(mtx)> lock(mtx);
            count++;
            contition.notify_one();
        }

        void acquire()
        {
            std::unique_lock<decltype(mtx)> lock(mtx);
            while (!count) // spurious wake-ups
                contition.wait(lock);
            count--;
        }

        bool try_acquire()
        {
            std::lock_guard<decltype(mtx)> lock(mtx);
            if (count)
            {
                count--;
                return true;
            }
            return false;
        }

        void releaseCount(uint32_t amount)
        {
            std::lock_guard<decltype(mtx)> lock(mtx);
            count += amount;
            if (amount > 1)
                contition.notify_all();
            else
                contition.notify_one();
        }

        void acquireCount(uint32_t amount)
        {
            std::unique_lock<decltype(mtx)> lock(mtx);
            while (amount > 0){
                while (!count) // spurious wake-ups
                    contition.wait(lock);
                
                if (count >= amount)
                {
                    count -= amount;
                    amount = 0;
                    break;
                } 
                else
                {
                    amount -= count;
                    count = 0;
                }
            }
        }
    };

}
