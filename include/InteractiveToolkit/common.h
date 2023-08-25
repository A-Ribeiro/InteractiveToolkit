#pragma once

#include "buildFlags.h"

//#include <stdlib.h>
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
#include <wchar.h> // wprintf
#include <cctype> // tolower

#if _MSC_VER
#define ITK_INLINE __forceinline
#else
//#define ITK_INLINE inline __attribute__((always_inline))
#define ITK_INLINE inline __attribute__((always_inline))
#endif

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <wchar.h>
#ifndef swprintf
#define swprintf _snwprintf
#endif

#define ITK_SYS_ALIGNED_ALLOC(alignment, size) _aligned_malloc(size,alignment)
#define ITK_SYS_ALIGNED_FREE(data) _aligned_free(data)

#include <direct.h>

#elif defined(__APPLE__) || defined(__linux__)

#include <sys/errno.h>
#include <unistd.h>

#define ITK_SYS_ALIGNED_ALLOC(alignment, size) (::aligned_alloc)(alignment,size)
#define ITK_SYS_ALIGNED_FREE(data) (::free)(data)

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
#else
//  GCC Intrinsics
#include <x86intrin.h>

#define _mm_f32_(v, i) (v)[i]

static ITK_INLINE int32_t & _mm_i32_(const __m128i &v, int i) noexcept {
	return ((int32_t*)&v)[i];
}
static ITK_INLINE uint32_t & _mm_u32_(const __m128i &v, int i) noexcept {
	return ((uint32_t*)&v)[i];
}

#endif

#elif defined(ITK_NEON)

#include <arm_neon.h>

#else

#endif


