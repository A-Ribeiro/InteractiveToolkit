#pragma once

#include "Callback.h"

namespace EventCore
{
    class ExecuteOnScopeEnd {
    public:
        EventCore::Callback<void()> callback;
        ExecuteOnScopeEnd(const EventCore::Callback<void()> &callback){
            this->callback = callback;
        }
        ~ExecuteOnScopeEnd() {
            if (this->callback != nullptr)
                this->callback();
        }
    };

}