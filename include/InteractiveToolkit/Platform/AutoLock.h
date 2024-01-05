#pragma once

// #include "platform_common.h"
#include "../common.h"
#include "Mutex.h"

namespace Platform
{

    class AutoLock {
        Mutex *mutex;
    public:
        AutoLock(Mutex *mutex){
            this->mutex = mutex;
            this->mutex->lock();
        }
        ~AutoLock() {
            this->mutex->unlock();
        }
    };

}