#pragma once

// #include "platform_common.h"
#include "../common.h"
#include "Time.h"

namespace Platform
{

#if defined(_WIN32)
    class UseWindowsHighResolutionClock
    {
        UseWindowsHighResolutionClock()
        {
            timeBeginPeriod(1);
        }

    public:
        ~UseWindowsHighResolutionClock()
        {
            timeEndPeriod(1);
        }
        static void sleep(int millis)
        {
            static UseWindowsHighResolutionClock useWindowsHighResolutionClock;
            Sleep(millis);
        }
    };
#elif defined(__APPLE__) || defined(__linux__)

#else
#error Platform Not Supported!!!
#endif

    /// \brief Sleep implementation in several platforms.
    ///
    /// Can be used with PlatformThread to make some thread sleep and avoid 100% CPU usage for example.
    ///
    /// \author Alessandro Ribeiro
    ///
    class Sleep
    {
    public:
        /// \brief Sleep for an amount of seconds.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_function() {
        ///     ...
        ///     PlatformSleep::sleepSec( 1 );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param secs seconds
        ///
        static void sec(int secs)
        {
            millis(secs * 1000);
        }

        /// \brief Sleep for an amount of milliseconds.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_function() {
        ///     ...
        ///     PlatformSleep::sleepMillis( 1 );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param millis milliseconds
        ///
        static void millis(int millis)
        {
#if defined(_WIN32)
            UseWindowsHighResolutionClock::sleep(millis);
#elif defined(__APPLE__) || defined(__linux__)
            usleep(static_cast<useconds_t>(millis * 1000));
#endif
        }

        /// \brief Sleep for an amount of microseconds.
        ///
        /// \warning This method consumes 100% CPU while sleeping when it is in windows OS.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// void thread_function() {
        ///     ...
        ///     // 0.1 millisecond sleep
        ///     PlatformSleep::busySleepMicro( 100 );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param micros microseconds
        ///
        static void microBusy(int64_t micros)
        {
#if defined(_WIN32)
            w32PerformanceCounter counter;
            while (counter.GetCounterMicro(false) < micros)
            {
            }
#elif defined(__APPLE__) || defined(__linux__)
            usleep(static_cast<useconds_t>(micros));
#endif
        }

        static void yield()
        {
#if defined(_WIN32)
            if (!SwitchToThread())
                UseWindowsHighResolutionClock::sleep(0);
#elif defined(__APPLE__) || defined(__linux__)
            // usleep(0) << same effect...
            //int rc = sched_yield();
            //ITK_ABORT(rc != 0, "Error: %s", strerror(errno));
            sched_yield();
#endif
        }
    };

}
