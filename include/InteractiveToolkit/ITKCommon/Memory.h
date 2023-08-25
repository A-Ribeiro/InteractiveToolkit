#pragma once

#include "../common.h"

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
    }

}