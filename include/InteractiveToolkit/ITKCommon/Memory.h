#pragma once

#include "../common.h"
#include "./StringUtil.h"

#if defined(__linux__)
#include <sys/sysinfo.h>
#elif defined(__APPLE__)
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/mach_port.h>
#include <mach/vm_statistics.h>
#include <sys/sysctl.h>
#elif defined(_WIN32)
#endif

namespace ITKCommon
{

    //
    // define malloc_aligned
    //

    namespace Memory
    {
        static ITK_INLINE void *malloc(size_t size, size_t N = 32) noexcept
        {
            size_t complete_16bytes = (N - size % N) % N;
#if defined(ITK_SSE2)
            return (void *)_mm_malloc(size + complete_16bytes, N);
#else
            return (void *)ITK_SYS_ALIGNED_ALLOC(N, size + complete_16bytes);
#endif
        }

        static ITK_INLINE void free(void *buffer) noexcept
        {
#if defined(ITK_SSE2)
            _mm_free(buffer);
#else
            ITK_SYS_ALIGNED_FREE(buffer);
#endif
        }

        static ITK_INLINE uint64_t total_ram()
        {
#if defined(__linux__)
            struct sysinfo si;
            if (sysinfo(&si) == 0)
                return (uint64_t)si.totalram;
            return 0;
#elif defined(__APPLE__)
            int mib[2] = {CTL_HW, HW_MEMSIZE};
            uint64_t physical_memory;
            size_t length = sizeof(physical_memory);

            if (sysctl(mib, 2, &physical_memory, &length, NULL, 0) == 0)
                return physical_memory;
            return 0;
#elif defined(_WIN32)
            MEMORYSTATUSEX memStatus;
            memStatus.dwLength = sizeof(memStatus);
            if (GlobalMemoryStatusEx(&memStatus))
                return (uint64_t)memStatus.ullTotalPhys;
            return 0;
#endif
        }

        static ITK_INLINE uint64_t available_ram()
        {
#if defined(__linux__)

#define _RAM_QUERY_SYSINFO_ 0
#define _RAM_QUERY_MEMINFO_ 1

#define _RAM_QUERY_USE_ _RAM_QUERY_MEMINFO_

#if _RAM_QUERY_USE_ == _RAM_QUERY_SYSINFO_
            struct sysinfo si;
            if (sysinfo(&si) == 0)
                return (uint64_t)(si.freeram * si.mem_unit);
            return 0;
#elif _RAM_QUERY_USE_ == _RAM_QUERY_MEMINFO_
            // Consider memory used by kernel and other processes
            std::unique_ptr<FILE, int (*)(FILE *)> meminfo(fopen("/proc/meminfo", "r"), fclose);
            if (!meminfo)
                throw std::runtime_error("error to read /proc/meminfo");

            uint64_t available_ram = 0;

            uint64_t total_ram = 0;
            uint64_t free_ram = 0;
            uint64_t buffers = 0;
            uint64_t cached = 0;

            char line[1024];
            while (fgets(line, 1024, meminfo.get()))
            {
                auto split = ITKCommon::StringUtil::tokenizer(line, ":");

                if (split[0].compare("MemAvailable") == 0)
                {
                    sscanf(split[1].c_str(), "%" PRIu64 " kB", &available_ram);
                    available_ram *= 1024;
                    return available_ram;
                }
                else if (split[0].compare("MemTotal") == 0)
                {
                    sscanf(split[1].c_str(), "%" PRIu64 " kB", &total_ram);
                    total_ram *= 1024;
                }
                else if (split[0].compare("MemFree") == 0)
                {
                    sscanf(split[1].c_str(), "%" PRIu64 " kB", &free_ram);
                    free_ram *= 1024;
                }
                else if (split[0].compare("Buffers") == 0)
                {
                    sscanf(split[1].c_str(), "%" PRIu64 " kB", &buffers);
                    buffers *= 1024;
                }
                else if (split[0].compare("Cached") == 0)
                {
                    sscanf(split[1].c_str(), "%" PRIu64 " kB", &cached);
                    cached *= 1024;
                }
            }

            return total_ram - free_ram - buffers - cached;
#endif
#elif defined(__APPLE__)
            vm_statistics_data_t vm_stats;
            mach_msg_type_number_t count = HOST_VM_INFO_COUNT;
            kern_return_t kr = host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vm_stats, &count);
            if (kr != KERN_SUCCESS)
                return 0;

            vm_size_t used_memory = (static_cast<uint64_t>(vm_stats.active_count) +
                                     // static_cast<uint64_t>(vm_stats.inactive_count) +
                                     static_cast<uint64_t>(vm_stats.wire_count)) *
                                    sysconf(_SC_PAGESIZE);
            ;
            return total_ram() - (uint64_t)used_memory;
#elif defined(_WIN32)
            MEMORYSTATUSEX memStatus;
            memStatus.dwLength = sizeof(memStatus);
            if (GlobalMemoryStatusEx(&memStatus))
                return (uint64_t)memStatus.ullAvailPhys;
            return 0;
#endif
        }
    }

}