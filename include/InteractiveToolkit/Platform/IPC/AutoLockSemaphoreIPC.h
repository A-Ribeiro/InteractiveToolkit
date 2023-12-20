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
            AutoLockSemaphoreIPC(SemaphoreIPC *semaphore)
            {
                signaled = false;
                this->semaphore = semaphore;
                signaled = !this->semaphore->blockingAcquire();
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