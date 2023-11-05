#pragma once

#include "ThreadDataSet.h"
#include "../Thread.h"

namespace Platform
{

    void ThreadDataSet::unregisterThread(Thread *thread)
    {
        ThreadIdentifier tid = GetCurrentThreadId_Custom();
        MapT::iterator it;

        register_mutex.lock();

        // grab all semaphores
        semaphore.acquireCount(threadData.size());

        Thread *result = nullptr;
        if (threadData.size() > 0)
        {
            it = threadData.find(tid);
            if (it != threadData.end())
            {
                result = it->second;
                threadData.erase(it);
            }
        }

        // release all semaphores
        semaphore.releaseCount(threadData.size());

        register_mutex.unlock();

        // mark thread exited
        if (result != nullptr)
            result->exited = true;
    }

}