#pragma once

#include "platform_common.h"
#include "Semaphore.h"

namespace Platform
{

    class AutoLockSemaphore
    {
        Semaphore *semaphore;

    public:
        bool signaled;
        AutoLockSemaphore(Semaphore *semaphore)
        {
            signaled = false;
            this->semaphore = semaphore;
            signaled = !this->semaphore->blockingAcquire();
        }
        ~AutoLockSemaphore()
        {
            if (signaled)
                return;
            this->semaphore->release();
        }
    };

}