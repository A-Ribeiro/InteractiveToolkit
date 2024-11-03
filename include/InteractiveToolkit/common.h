#pragma once

#include "buildFlags.h"

// #include <stdlib.h>
#include <cstdlib>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <limits.h>
#include <float.h>
#include <cmath> //para sqrtf
#include <string>
#include <string.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <algorithm>
#include <functional>
#include <type_traits>
#include <memory>
#include <stdexcept>
#include <stdarg.h> //va_start
#include <wchar.h>  // wprintf
#include <cctype>   // tolower
#include <inttypes.h> // PRIu64

#include <list>

#if _MSC_VER
#define ITK_INLINE __forceinline
#else
// #define ITK_INLINE inline __attribute__((always_inline))
#define ITK_INLINE inline __attribute__((always_inline))
#endif

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <wchar.h>
#ifndef swprintf
#define swprintf _snwprintf
#endif

#define ITK_SYS_ALIGNED_ALLOC(alignment, size) _aligned_malloc(size, alignment)
#define ITK_SYS_ALIGNED_FREE(data) _aligned_free(data)

#include <direct.h>

#include <mmsystem.h>
#include <Shlwapi.h>
#include <Iphlpapi.h>
#include <tchar.h>
#include <locale.h>

#include <inttypes.h>

#include <sys/types.h>

#elif defined(__APPLE__) || defined(__linux__)

#include <sys/errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <pwd.h>

#include <sys/time.h>
#include <time.h>

#include <sys/stat.h>
#include <dirent.h>

#if defined(__APPLE__)

static void* ITK_SYS_ALIGNED_ALLOC(size_t alignment, size_t size)
{
    size_t prt_plus_size = size + sizeof(intptr_t) + alignment;
    intptr_t real_alloc = (intptr_t)malloc(prt_plus_size);
    //starts after 1 ptr_t size
    intptr_t alligned_block = real_alloc + sizeof(intptr_t);
    intptr_t complete_16bytes = (alignment - alligned_block % alignment) % alignment;
    alligned_block += complete_16bytes;
    intptr_t* allocated_block_ref = (intptr_t*)alligned_block - 1;
    allocated_block_ref[0] = real_alloc;
    return (void*)alligned_block;
}
static void ITK_SYS_ALIGNED_FREE(void* data)
{
    intptr_t* real_alloc = (intptr_t*)data - 1;
    free((void*)real_alloc[0]);
}

#else

#define ITK_SYS_ALIGNED_ALLOC(alignment, size) (::aligned_alloc)(alignment, size)
#define ITK_SYS_ALIGNED_FREE(data) (::free)(data)

#endif

#else

#error Platform not supported...

#endif

//
// Include SSE/NEON headers
//
#if defined(ITK_SSE2)

// #include <xmmintrin.h> // SSE1
// #include <emmintrin.h> // SSE2

#ifdef _MSC_VER
//  VisualStudio  Intrinsics
#include <intrin.h>
#define _mm_f32_(v, i) (v).m128_f32[i]
#define _mm_i32_(v, i) (v).m128i_i32[i]
#define _mm_u32_(v, i) (v).m128i_u32[i]
#if defined(ITK_AVX2)
    #define _mm256_u32_(v, i) (v).m256i_u32[i]
#endif
#else
//  GCC Intrinsics
#include <x86intrin.h>

#define _mm_f32_(v, i) (v)[i]

static ITK_INLINE int32_t &_mm_i32_(const __m128i &v, int i) noexcept
{
    return ((int32_t *)&v)[i];
}
static ITK_INLINE uint32_t &_mm_u32_(const __m128i &v, int i) noexcept
{
    return ((uint32_t *)&v)[i];
}

#if defined(ITK_AVX2)
static ITK_INLINE uint32_t &_mm256_u32_(const __m256i &v, int i) noexcept
{
    return ((uint32_t *)&v)[i];
}
#endif

#endif

static inline __m128i _sse2_mm_min_epi32(__m128i a, __m128i b)
{
    __m128i cmp = _mm_cmpgt_epi32(a, b);
    __m128i a_small = _mm_andnot_si128(cmp, a);
    __m128i b_small = _mm_and_si128(cmp, b);
    return _mm_or_si128(a_small, b_small);
}

static inline __m128i _sse2_mm_max_epi32(__m128i a, __m128i b)
{
    __m128i cmp = _mm_cmplt_epi32(a, b);
    __m128i a_gt = _mm_andnot_si128(cmp, a);
    __m128i b_gt = _mm_and_si128(cmp, b);
    return _mm_or_si128(a_gt, b_gt);
}

static inline __m128i _sse2_mm_cmplt_epu32(__m128i a, __m128i b)
{
    const __m128i _min_epi32 = _mm_set1_epi32(0x80000000);
    return _mm_cmplt_epi32(
        _mm_xor_si128(a, _min_epi32),
        _mm_xor_si128(b, _min_epi32));
}
static inline __m128i _sse2_mm_cmpgt_epu32(__m128i a, __m128i b)
{
    const __m128i _min_epi32 = _mm_set1_epi32(0x80000000);
    return _mm_cmpgt_epi32(
        _mm_xor_si128(a, _min_epi32),
        _mm_xor_si128(b, _min_epi32));
}

static inline __m128i _sse2_mm_min_epu32(__m128i a, __m128i b)
{
    __m128i cmp = _sse2_mm_cmpgt_epu32(a, b);
    __m128i a_small = _mm_andnot_si128(cmp, a);
    __m128i b_small = _mm_and_si128(cmp, b);
    return _mm_or_si128(a_small, b_small);
}

static inline __m128i _sse2_mm_max_epu32(__m128i a, __m128i b)
{
    __m128i cmp = _sse2_mm_cmplt_epu32(a, b);
    __m128i a_small = _mm_andnot_si128(cmp, a);
    __m128i b_small = _mm_and_si128(cmp, b);
    return _mm_or_si128(a_small, b_small);
}

static inline __m128 _sse2_mm_floor_ps(const __m128 &f)
{
    // r = (float)(int)f;
    __m128 r = _mm_cvtepi32_ps(_mm_cvttps_epi32(f));

    // if (f < r) r -= 1;
    const __m128 _one = _mm_set1_ps(1.f);
    r = _mm_sub_ps(r, _mm_and_ps(_mm_cmplt_ps(f, r), _one));

    // two possible values:
    // - 8388608.f (23bits)
    // - 2147483648.f (31bits)
    // Any value greater than this, will have integral mantissa... 
    // and no decimal part
    //
    // if ((abs(f) > 2**31 )) r = f;
    const __m128 _sign_bit = _mm_set1_ps(-0.f);
    const __m128 _max_f = _mm_set1_ps(8388608.f);
    __m128 m = _mm_cmpgt_ps(_max_f, _mm_andnot_ps(_sign_bit, f));
    r = _mm_or_ps(_mm_and_ps(m, r), _mm_andnot_ps(m, f));

    return r;
}

// floor(-fp) = -ceiling(fp)
// ceiling(fp) = -floor(-fp)
static inline __m128 _sse2_mm_ceil_ps(const __m128 &f)
{
    //const __m128 _sign_bit = _mm_set1_ps(-0.f);
    //__m128 r = _mm_xor_ps(_sse2_mm_floor_ps(_mm_xor_ps(f, _sign_bit)), _sign_bit);
    //return r;

    // r = (float)(int)f;
    __m128 r = _mm_cvtepi32_ps(_mm_cvttps_epi32(f));

    // if (f < r) r -= 1;
    const __m128 _one = _mm_set1_ps(-1.f);
    r = _mm_sub_ps(r, _mm_and_ps(_mm_cmpgt_ps(f, r), _one));

    // two possible values:
    // - 8388608.f (23bits)
    // - 2147483648.f (31bits)
    // Any value greater than this, will have integral mantissa... 
    // and no decimal part
    //
    // if ((abs(f) > 2**31 )) r = f;
    const __m128 _sign_bit = _mm_set1_ps(-0.f);
    const __m128 _max_f = _mm_set1_ps(8388608.f);
    __m128 m = _mm_cmpgt_ps(_max_f, _mm_andnot_ps(_sign_bit, f));
    r = _mm_or_ps(_mm_and_ps(m, r), _mm_andnot_ps(m, f));

    return r;
}

static inline __m128 _sse2_mm_round_ps(const __m128 &input)
{
    __m128 _half_signed = _mm_or_ps( _mm_and_ps(_mm_set1_ps(-0.f), input), _mm_set1_ps(.5f) );
    __m128 f = _mm_add_ps(input, _half_signed);
    
    //r = (float)(int)f;
    __m128 r = _mm_cvtepi32_ps(_mm_cvttps_epi32(f));

    // two possible values:
    // - 8388608.f (23bits)
    // - 2147483648.f (31bits)
    // Any value greater than this, will have integral mantissa... 
    // and no decimal part
    //
    // if ((abs(f) > 2**31 )) r = f;
    const __m128 _sign_bit = _mm_set1_ps(-0.f);
    const __m128 _max_f = _mm_set1_ps(8388608.f);
    __m128 m = _mm_cmpgt_ps(_max_f, _mm_andnot_ps(_sign_bit, input));
    r = _mm_or_ps(_mm_and_ps(m, r), _mm_andnot_ps(m, input));

    return r;
}

#elif defined(ITK_NEON)

#include <arm_neon.h>

#else

#endif
