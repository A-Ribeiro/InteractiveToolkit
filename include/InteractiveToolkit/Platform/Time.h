#pragma once

// #include "platform_common.h"
#include "../common.h"

#ifdef __APPLE__

// for unamed semaphores on mac
//#include "Core/unamed_fake_sem.h"

#include <mach/mach_time.h>

// Thread
#include <sys/sysctl.h>

#endif

namespace Platform
{

#if defined(_WIN32)

    class w32PerformanceCounterData
    {
    public:
        double freqMicro;
        double freqMillis;
        int64_t freqSec;
        // bool filledFreqConstants;

    private:
        w32PerformanceCounterData()
        {
            LARGE_INTEGER PCFreq;

            QueryPerformanceFrequency(&PCFreq);
            freqSec = PCFreq.QuadPart;
            freqMicro = (double)freqSec / 1000000.0;
            freqMillis = (double)freqSec / 1000.0;

            // printf("frequencySeconds: %I64d\n", freqSec);

            // filledFreqConstants = true;
        }

    public:
        static w32PerformanceCounterData* Instance()
        {
            static w32PerformanceCounterData dt;
            return &dt;
        }
    };

    /// \private
    class w32PerformanceCounter
    {
    private:
        LARGE_INTEGER CounterStart;
        LARGE_INTEGER CounterStartBackup;

    public:
        w32PerformanceCounter()
        {
            QueryPerformanceCounter(&CounterStart);
            CounterStartBackup = CounterStart;
        }
        void UndoReset()
        {
            CounterStart = CounterStartBackup;
        }
        void Reset()
        {
            CounterStartBackup = CounterStart;
            QueryPerformanceCounter(&CounterStart);
        }

        // uint64_t GetCounterMillis(bool reset = false);
        int64_t GetCounterMicro(bool reset = false)
        {
        https://stackoverflow.com/questions/5404277/porting-clock-gettime-to-windows

            const uint64_t MS_PER_SEC = UINT64_C(1000);     // MS = milliseconds
            const uint64_t US_PER_MS = UINT64_C(1000);     // US = microseconds
            const uint64_t HNS_PER_US = UINT64_C(10);       // HNS = hundred-nanoseconds (e.g., 1 hns = 100 ns)
            const uint64_t NS_PER_US = UINT64_C(1000);

            const uint64_t HNS_PER_SEC = (MS_PER_SEC * US_PER_MS * HNS_PER_US);
            const uint64_t NS_PER_HNS = UINT64_C(100);    // NS = nanoseconds
            const uint64_t NS_PER_SEC = (MS_PER_SEC * US_PER_MS * NS_PER_US);
            const uint64_t US_PER_SEC = (MS_PER_SEC * US_PER_MS);

            const w32PerformanceCounterData *p_dt = w32PerformanceCounterData::Instance();

            LARGE_INTEGER li;
            QueryPerformanceCounter(&li);

            int64_t result = li.QuadPart - CounterStart.QuadPart;

            int64_t tv_sec = (result / p_dt->freqSec);
            //int64_t tv_nsec = (((result % p_dt->freqSec) * NS_PER_SEC) / p_dt->freqSec);
            
            int64_t tv_sec_modulus_int = tv_sec * p_dt->freqSec;
            int64_t tv_sec_modulus = result - tv_sec_modulus_int;

            //int64_t tv_usec = (((result % p_dt->freqSec) * US_PER_SEC) / p_dt->freqSec);
            int64_t tv_usec = ((tv_sec_modulus * US_PER_SEC) / p_dt->freqSec);

            int64_t micros = tv_sec * US_PER_SEC + tv_usec;

            if (reset)
            {
                CounterStartBackup = CounterStart;
                // CounterStart = li;

                int64_t micros_to_quadpart = tv_sec_modulus_int + (tv_usec * p_dt->freqSec) / US_PER_SEC;
                CounterStart.QuadPart += micros_to_quadpart;
            }

            //const w32PerformanceCounterData *p_dt = w32PerformanceCounterData::Instance();

            //LARGE_INTEGER li;
            //QueryPerformanceCounter(&li);

            //int64_t result = li.QuadPart - CounterStart.QuadPart;

            //int64_t micros = (int64_t)((double)result / p_dt->freqMicro);

            //if (reset)
            //{
            //    CounterStartBackup = CounterStart;
            //    // CounterStart = li;

            //    CounterStart.QuadPart += (int64_t)((double)micros * p_dt->freqMicro);
            //}

            return micros;
        }
        //__int64 GetCounterSec(bool reset = false);
    };

#elif defined(__APPLE__) || defined(__linux__)

    /// \private
    class UnixMicroCounter
    {
    private:

#ifdef __APPLE__
        // && __iOS__
        // Get the timebase info
        mach_timebase_info_data_t info;
#endif

        double now_microseconds_double(void)
        {
#ifdef __APPLE__

            uint64_t result = mach_absolute_time();
            double resultd = (((double)result * (double)info.numer) / ((double)info.denom)) / 1000.0;
            return resultd;

#else

            struct timespec res;
            clock_gettime(CLOCK_MONOTONIC, &res);
            return (((double)res.tv_sec) * 1000000.0 + ((double)res.tv_nsec) * 0.001);

#endif
        }
        uint64_t now_microseconds_uint64_t(void)
        {
#ifdef __APPLE__

            uint64_t result = mach_absolute_time();
            result *= info.numer;
            result /= info.denom;
            result = result / 1000;
            return result;

#else

            struct timespec res;
            clock_gettime(CLOCK_MONOTONIC, &res);
            //return (((uint64_t)res.tv_sec) * 1000000ULL + (((uint64_t)res.tv_nsec) / 1000ULL) % 1000000ULL);
            return (((uint64_t)res.tv_sec) * 1000000ULL + (((uint64_t)res.tv_nsec) / 1000ULL) );

#endif
        }
        uint64_t base;

    public:
        UnixMicroCounter()
        {
#if __APPLE__
            mach_timebase_info(&info);
#endif
            base = now_microseconds_uint64_t();
        }
        int64_t GetDeltaMicro(bool reset)
        {
            uint64_t newbase = now_microseconds_uint64_t();
            int64_t delta = newbase - base;

            if (reset)
                base = newbase;

            return delta;
        }
    };

#else
#error Platform Not Supported!!!
#endif

    /// \brief Helper class to compute the inter frame delta time of an interactive application.
    ///
    /// \author Alessandro Ribeiro
    ///
    template<typename T>
    class TimeTemplate
    {

#if defined(_WIN32)
        w32PerformanceCounter win32_counter;
#elif defined(__APPLE__) || defined(__linux__)
        UnixMicroCounter unix_counter;
#endif

    public:
        TimeTemplate()
        {
            deltaTime = (T)0.0;
            unscaledDeltaTime = (T)0.0;
            timeScale = (T)1.0;
            deltaTimeMicro = 0;
        }

        /// \brief update
        ///
        /// Must be called to compute the delta time.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroPlatform/aRibeiroPlatform.h>
        /// using namespace aRibeiro;
        ///
        /// PlatformTime time;
        /// float elapsedSinceStartTimeSec = 0;
        ///
        /// // set the timer to be relative to this point of execution
        /// time.update();
        ///
        /// while (window.open()) {
        ///   time.update();
        ///
        ///   // use delta time to advance the interactive system
        ///   elapsedSinceStartTimeSec += time.deltaTime;
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        void update()
        {

#if defined(_WIN32)
            deltaTimeMicro = win32_counter.GetCounterMicro(true);
            if (deltaTimeMicro == 0)
            {
                win32_counter.UndoReset();
            }
#else
            deltaTimeMicro = unix_counter.GetDeltaMicro(true);
#endif

            if (deltaTimeMicro < 0)
            {
                printf("Time error...Negative Delta Time... \n");
                // fgetc(stdin);
            }

            double sec = (double)deltaTimeMicro / 1000000.0;

            unscaledDeltaTime = (T)(sec);
            deltaTime = (T)(sec * (double)timeScale);
        }

        /// \brief Force this time counter to reset all fields (deltaTime,unscaledDeltaTime,deltaTimeMicro)
        ///
        /// can be called when load some resource, to avoid cutout image sequences.
        ///
        /// \author Alessandro Ribeiro
        ///
        void reset()
        {
            unscaledDeltaTime = 0;
            deltaTime = 0;
            deltaTimeMicro = 0;

#if defined(_WIN32)
            win32_counter.GetCounterMicro(true);
#else
            unix_counter.GetDeltaMicro(true);
#endif
        }

        T deltaTime;         ///< time between two updates in seconds. The value is multiplied by timeScale.
        T unscaledDeltaTime; ///< time between two updates in seconds. Not affected by the timeScale.
        T timeScale;         ///< Value that is used to multiply the deltaTime

        int64_t deltaTimeMicro; ///< time between two updates in microseconds. Not affected by the timeScale.
    };

    using Timef32 = Platform::TimeTemplate<float>;
    using Timef64 = Platform::TimeTemplate<double>;

    using Time = Timef32;

}
