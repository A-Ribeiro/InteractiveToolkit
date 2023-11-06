#pragma once

#if !defined(_WIN32)
#error ERROR: Including Windows Gracefull Process Terminator in a Non-Windows OS
#endif

#include "platform_common.h"
#include "../Thread.h"
#include "../IPC/LowLatencyQueueIPC.h"


namespace Platform {

    //
    // This Class is needed only in windows systems.... 
    // that does not have a way to signaling a process...
    //
    class PlatformProcessGracefulWindowsProcessTerminator {

        DWORD dwProcessId;
        std::string pid_str;

        Platform::LowLatencyQueueIPC* queue;

        Platform::Thread thread;

        void(*fnc)(int);

        void threadRun() {

            ObjectBuffer signal;
            queue->read(&signal);
            if (queue->isSignaled())
                return;

            int signal_int = SIGINT;

            if (signal.size >= 4)
                signal_int = *(int*)signal.data;

            fnc(signal_int);
            //raise(signal_int);
        }

    public:
        PlatformProcessGracefulWindowsProcessTerminator(void(*_fnc)(int)) :
            thread(this, &PlatformProcessGracefulWindowsProcessTerminator::threadRun),
            fnc(_fnc) {
            queue = NULL;
            dwProcessId = GetCurrentProcessId();

            char aux[64];
            sprintf(aux, "p%u", dwProcessId);
            pid_str = aux;

            queue = new Platform::LowLatencyQueueIPC(pid_str.c_str(), PlatformQueueIPC_READ, 16, sizeof(int));
            thread.setShouldDisposeThreadByItself(true);
            thread.start();
        }

        ~PlatformProcessGracefulWindowsProcessTerminator() {
            thread.interrupt();
            thread.wait();
        }

    };

}