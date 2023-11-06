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
            if (Instance()->handle_signal_fnc != nullptr)
                Instance()->handle_signal_fnc(_sngl);
        }

    private:

        Signal(){
            signal_set = false;
        }

        bool signal_set;
#if defined(_WIN32)
        Callback<void(int)> signalFunction;

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
        static void Set(const Callback<void(int)> &sgnl_fnc)
        {
            Instance()->handle_signal_fnc = sgnl_fnc;

            if (Instance()->signal_set)
                return;

            Instance()->signal_set = true;

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
            Instance()->handle_signal_fnc = nullptr;

            if (!Instance()->signal_set)
                return;

            Instance()->signal_set = false;

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
