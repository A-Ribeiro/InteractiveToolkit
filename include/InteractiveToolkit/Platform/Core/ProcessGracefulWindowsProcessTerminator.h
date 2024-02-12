#pragma once

#if !defined(_WIN32)
#error ERROR: Including Windows Gracefull Process Terminator in a Non-Windows OS
#endif

//#include "platform_common.h"
#include "../../common.h"
#include "../Thread.h"
#include "../IPC/LowLatencyQueueIPC.h"


namespace Platform {

    //
    // This Class is needed only in windows systems.... 
    // that does not have a way to signaling a process...
    //
    class ProcessGracefulWindowsProcessTerminator : public EventCore::HandleCallback {

        DWORD dwProcessId;
        std::string pid_str;

        Platform::IPC::LowLatencyQueueIPC* queue;

        Platform::Thread thread;

        void(*fnc)(int);

        void threadRun() {

            ObjectBuffer signal;
            bool signaled = false;
            queue->read(&signal, &signaled);
            if (signaled)
                return;

            int signal_int = SIGINT;

            if (signal.size >= 4)
                signal_int = *(int*)signal.data;

            fnc(signal_int);
            //raise(signal_int);
        }

    public:
        ProcessGracefulWindowsProcessTerminator(void(*_fnc)(int)) :
            thread( EventCore::CallbackWrapper(&ProcessGracefulWindowsProcessTerminator::threadRun, this) ),
            fnc(_fnc) {
            queue = NULL;
            dwProcessId = GetCurrentProcessId();

            char aux[64];
            snprintf(aux, 64, "p%u", dwProcessId);
            pid_str = aux;

            queue = new Platform::IPC::LowLatencyQueueIPC(pid_str.c_str(), Platform::IPC::QueueIPC_READ, 16, sizeof(int));
            thread.setShouldDisposeThreadByItself(true);
            thread.start();
        }

        ~ProcessGracefulWindowsProcessTerminator() {
            thread.interrupt();
            thread.wait();
        }

    };

}