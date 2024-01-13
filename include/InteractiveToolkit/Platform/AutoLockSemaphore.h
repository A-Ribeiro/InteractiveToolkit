#pragma once

//#include "platform_common.h"
#include "../common.h"
#include "Semaphore.h"

namespace Platform
{

    class AutoLockSemaphore
    {
        Semaphore *semaphore;

    public:
        bool signaled;
        AutoLockSemaphore(Semaphore *semaphore, bool ignore_signal = false)
        {
            signaled = false;
            this->semaphore = semaphore;
            signaled = !this->semaphore->blockingAcquire(ignore_signal);
        }
        ~AutoLockSemaphore()
        {
            if (signaled)
                return;
            this->semaphore->release();
        }
        void cancelAutoRelease() {
            signaled = true;
        }
    };

}