#pragma once

#include "platform_common.h"
#include "../EventCore/Callback.h"

namespace Platform
{

    class Signal
    {

    protected:
        static Signal *Instance()
        {
            static Signal _instance;
            return &_instance;
        }

        static void handle_signal_fnc(int _sngl)
        {
            Signal *sngl = Signal::Instance();
            if (sngl->signalFunction != nullptr)
                sngl->signalFunction(_sngl);
        }

    private:

        Signal(){
            signal_set = false;
        }

        bool signal_set;
        EventCore::Callback<void(int)> signalFunction;

#if defined(_WIN32)
        static BOOL WINAPI HandlerRoutine_exit(_In_ DWORD dwCtrlType)
        {
            switch (dwCtrlType)
            {
                // Handle the CTRL-C signal.
            case CTRL_C_EVENT:
                Platform::Signal::handle_signal_fnc(0);
                Sleep(5000);
                return TRUE;

                // CTRL-CLOSE: confirm that the user wants to exit.
            case CTRL_CLOSE_EVENT:
                Platform::Signal::handle_signal_fnc(0);
                Sleep(5000);
                return TRUE;

            case CTRL_BREAK_EVENT:
                // Beep(900, 200);
                // printf("Ctrl-Break event\n\n");
                Platform::Signal::handle_signal_fnc(0);
                Sleep(5000);
                return TRUE;

                // Pass other signals to the next handler.
            case CTRL_LOGOFF_EVENT:
                // Beep(1000, 200);
                // printf("Ctrl-Logoff event\n\n");
                Platform::Signal::handle_signal_fnc(0);
                Sleep(5000);
                return TRUE;

            case CTRL_SHUTDOWN_EVENT:
                // Beep(750, 500);
                // printf("Ctrl-Shutdown event\n\n");
                Platform::Signal::handle_signal_fnc(0);
                Sleep(5000);
                return TRUE;
            default:
                return FALSE;
            }
        }

#endif

    public:
        static void Set(const EventCore::Callback<void(int)> &sgnl_fnc)
        {
            Signal *sngl = Signal::Instance();
            sngl->signalFunction = sgnl_fnc;

            if (sngl->signal_set)
                return;

            sngl->signal_set = true;

            signal(SIGINT, handle_signal_fnc);
            signal(SIGTERM, handle_signal_fnc);

#if defined(_WIN32)
            //SetConsoleCtrlHandler(Platform::Signal::HandlerRoutine_exit, false);
            SetConsoleCtrlHandler(Platform::Signal::HandlerRoutine_exit, true);
#else
            signal(SIGQUIT, handle_signal_fnc);
#endif
        }
        static void Reset()
        {
            Signal *sngl = Signal::Instance();
            

            if (!sngl->signal_set)
                return;

            sngl->signal_set = false;
            sngl->signalFunction = nullptr;

            signal(SIGINT, SIG_DFL);
            signal(SIGTERM, SIG_DFL);

#if defined(_WIN32)
            SetConsoleCtrlHandler(Platform::Signal::HandlerRoutine_exit, false);
#else
            signal(SIGQUIT, SIG_DFL);
#endif
        }
    };

}
