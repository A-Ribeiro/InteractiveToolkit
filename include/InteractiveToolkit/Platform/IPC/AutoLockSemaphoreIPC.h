#pragma once

#include "../platform_common.h"
#include "SemaphoreIPC.h"

namespace Platform
{

    namespace IPC
    {

        class AutoLockSemaphoreIPC
        {
            SemaphoreIPC *semaphore;

        public:
            bool signaled;
            AutoLockSemaphoreIPC(SemaphoreIPC *semaphore, bool ignore_signal = false)
            {
                signaled = false;
                this->semaphore = semaphore;
                signaled = !this->semaphore->blockingAcquire(ignore_signal);
            }
            ~AutoLockSemaphoreIPC()
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

}