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
#include <stdarg.h>   //va_start
#include <wchar.h>    // wprintf
#include <cctype>     // tolower
#include <inttypes.h> // PRIu64
#include <regex>

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

#if !defined(__CYGWIN__)
typedef SSIZE_T ssize_t;
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

static void *ITK_SYS_ALIGNED_ALLOC(size_t alignment, size_t size)
{
    size_t prt_plus_size = size + sizeof(intptr_t) + alignment;
    intptr_t real_alloc = (intptr_t)malloc(prt_plus_size);
    // starts after 1 ptr_t size
    intptr_t alligned_block = real_alloc + sizeof(intptr_t);
    intptr_t complete_16bytes = (alignment - alligned_block % alignment) % alignment;
    alligned_block += complete_16bytes;
    intptr_t *allocated_block_ref = (intptr_t *)alligned_block - 1;
    allocated_block_ref[0] = real_alloc;
    return (void *)alligned_block;
}
static void ITK_SYS_ALIGNED_FREE(void *data)
{
    intptr_t *real_alloc = (intptr_t *)data - 1;
    free((void *)real_alloc[0]);
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
#define _mm_i64_(v, i) (v).m128i_i64[i]
#define _mm_u64_(v, i) (v).m128i_u64[i]

#define _mm_i32_read_0(vec) _mm_cvtsi128_si32(vec)
#define _mm_u32_read_0(vec) static_cast<uint32_t>(_mm_cvtsi128_si32(vec))

#define _mm_f32_read_0(vec) _mm_cvtss_f32(vec)
#define _mm_f32_read(vec, index) _mm_cvtss_f32(_mm_shuffle_ps(vec, vec, _MM_SHUFFLE(index, index, index, index)))

#if defined(ITK_AVX2)
#define _mm256_u32_(v, i) (v).m256i_u32[i]
#define _mm256_u64_(v, i) (v).m256i_u64[i]
#endif
#else
//  GCC Intrinsics
#include <x86intrin.h>

#define _mm_f32_(v, i) (v)[i]

#define _mm_i32_read_0(vec) _mm_cvtsi128_si32(vec)
#define _mm_u32_read_0(vec) static_cast<uint32_t>(_mm_cvtsi128_si32(vec))

#define _mm_f32_read_0(vec) _mm_cvtss_f32(vec)
#define _mm_f32_read(vec, index) _mm_cvtss_f32(_mm_shuffle_ps(vec, vec, _MM_SHUFFLE(index, index, index, index)))

static ITK_INLINE int32_t &_mm_i32_(const __m128i &v, int i) noexcept
{
    return ((int32_t *)&v)[i];
}
static ITK_INLINE uint32_t &_mm_u32_(const __m128i &v, int i) noexcept
{
    return ((uint32_t *)&v)[i];
}

static ITK_INLINE int64_t &_mm_i64_(const __m128i &v, int i) noexcept
{
    return ((int64_t *)&v)[i];
}
static ITK_INLINE uint64_t &_mm_u64_(const __m128i &v, int i) noexcept
{
    return ((uint64_t *)&v)[i];
}

#if defined(ITK_AVX2)
static ITK_INLINE uint32_t &_mm256_u32_(const __m256i &v, int i) noexcept
{
    return ((uint32_t *)&v)[i];
}
static ITK_INLINE uint64_t &_mm256_u64_(const __m256i &v, int i) noexcept
{
    return ((uint64_t *)&v)[i];
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
    // const __m128 _sign_bit = _mm_set1_ps(-0.f);
    //__m128 r = _mm_xor_ps(_sse2_mm_floor_ps(_mm_xor_ps(f, _sign_bit)), _sign_bit);
    // return r;

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
    __m128 _half_signed = _mm_or_ps(_mm_and_ps(_mm_set1_ps(-0.f), input), _mm_set1_ps(.5f));
    __m128 f = _mm_add_ps(input, _half_signed);

    // r = (float)(int)f;
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

const __m128 _float_info_all_bits_set = _mm_castsi128_ps(_mm_set1_epi32((int)0xffffffff));
const __m128i _float_info_mantissa_bit_u = _mm_set1_epi32((int)0x007FFFFF);
const __m128i _float_info_mantissa_min_u = _mm_set1_epi32((int)1);
const __m128i _float_info_sign_bit_u = _mm_set1_epi32((int)0x80000000);
const __m128i _float_info_minus_one_u = _mm_set1_epi32((int)0xFFFFFFFF);
const __m128i _float_info_one_u = _mm_set1_epi32((int)1);
const __m128i _float_info_number_except_sign_bit_u = _mm_set1_epi32((int)0x7FFFFFFF);
const __m128i _float_info_expoent_bit_u = _mm_set1_epi32((int)0x7F800000);
const __m128i _float_info_inf_u = _mm_set1_epi32((int)0x7F800000);
const __m128i _float_info_max_float_u = _mm_set1_epi32((int)0x7F7FFFFF);
const __m128i _float_info_zero = _mm_set1_epi32((int)0);
const __m128i _float_info_q_nan = _mm_set1_epi32((int)0x7FC00000);

// v == v returns false on NaN
static inline __m128 _sse2_is_nan_ps(const __m128 &v)
{
    return _mm_xor_ps(_mm_cmpeq_ps(v, v), _float_info_all_bits_set);
}

static inline __m128 _sse2_nextafter_ps(const __m128 &x, const __m128 &y)
{
    __m128i bits_x = _mm_castps_si128(x);
    __m128i bits_y = _mm_castps_si128(y);

    // NaN check
    __m128i is_nan_mask = _mm_castps_si128(_mm_or_ps(_sse2_is_nan_ps(x), _sse2_is_nan_ps(y)));

    // x == y
    __m128i is_eq_mask = _mm_castps_si128(_mm_cmpeq_ps(x, y));

    // y < x (descending)
    __m128i is_descending = _mm_castps_si128(_mm_cmplt_ps(y, x));

    __m128i bits_number_only = _mm_and_si128(bits_x, _float_info_number_except_sign_bit_u);

    // Zero check
    __m128i is_zero = _mm_cmpeq_epi32(bits_number_only, _float_info_zero);
    __m128i zero_result = _mm_or_si128(_mm_and_si128(is_descending, _float_info_sign_bit_u), _float_info_mantissa_min_u);

    // Sign bit and its mask
    __m128i sign_x = _mm_and_si128(bits_x, _float_info_sign_bit_u);
    __m128i sign_mask = _mm_srai_epi32(sign_x, 31);

    // Increment direction: -1 if descending XOR negative, else +1
    __m128i increment = _mm_xor_si128(is_descending, sign_mask);
    increment = _mm_or_si128(_mm_and_si128(increment, _float_info_minus_one_u),
                             _mm_andnot_si128(increment, _float_info_one_u));

    // Inf check
    __m128i is_inf = _mm_cmpeq_epi32(bits_number_only, _float_info_inf_u);

    // For inf: use max_float with correct sign
    __m128i max_float = _mm_or_si128(sign_x, _float_info_max_float_u);

    // Increment or use max for inf
    __m128i result_number = _mm_add_epi32(bits_number_only, increment);
    result_number = _mm_or_si128(_mm_and_si128(is_inf, max_float),
                                 _mm_andnot_si128(is_inf, result_number));

    // Reconstruct with sign
    __m128i result = _mm_or_si128(sign_x, result_number);

    // Zero case
    result = _mm_or_si128(_mm_and_si128(is_zero, zero_result),
                          _mm_andnot_si128(is_zero, result));

    // x == y case
    result = _mm_or_si128(_mm_and_si128(is_eq_mask, bits_y),
                          _mm_andnot_si128(is_eq_mask, result));

    // NaN case
    result = _mm_or_si128(_mm_and_si128(is_nan_mask, _float_info_q_nan),
                          _mm_andnot_si128(is_nan_mask, result));

    return _mm_castsi128_ps(result);
}

#elif defined(ITK_NEON)

#include <arm_neon.h>

static inline float32x4_t _neon_mm_floor_ps(const float32x4_t &f)
{
    // r = (float)(int)f;
    float32x4_t r = vcvtq_f32_s32(vcvtq_s32_f32(f));

    // if (f < r) r -= 1;
    const uint32x4_t _one = vreinterpretq_u32_f32(vdupq_n_f32(1.f));
    r = vsubq_f32(r, vreinterpretq_f32_u32(vandq_u32(vcltq_f32(f, r), _one)));

    // two possible values:
    // - 8388608.f (23bits)
    // - 2147483648.f (31bits)
    // Any value greater than this, will have integral mantissa...
    // and no decimal part
    //
    // if ((abs(f) > 2**31 )) r = f;
    // const uint32x4_t _sign_bit = vreinterpretq_u32_f32(vdupq_n_f32(-0.f));
    const float32x4_t _max_f = vdupq_n_f32(8388608.f);
    uint32x4_t m = vcgtq_f32(_max_f, vabsq_f32(f));
    uint32x4_t r_u = vreinterpretq_u32_f32(r);
    uint32x4_t f_u = vreinterpretq_u32_f32(f);
    r_u = vorrq_u32(vandq_u32(m, r_u), vandq_u32(vmvnq_u32(m), f_u));

    return vreinterpretq_f32_u32(r_u);
}

// floor(-fp) = -ceiling(fp)
// ceiling(fp) = -floor(-fp)
static inline float32x4_t _neon_mm_ceil_ps(const float32x4_t &f)
{
    // const __m128 _sign_bit = _mm_set1_ps(-0.f);
    //__m128 r = _mm_xor_ps(_sse2_mm_floor_ps(_mm_xor_ps(f, _sign_bit)), _sign_bit);
    // return r;

    // r = (float)(int)f;
    float32x4_t r = vcvtq_f32_s32(vcvtq_s32_f32(f));

    // if (f < r) r -= 1;
    const uint32x4_t _one = vreinterpretq_u32_f32(vdupq_n_f32(-1.f));
    r = vsubq_f32(r, vreinterpretq_f32_u32(vandq_u32(vcgtq_f32(f, r), _one)));

    // two possible values:
    // - 8388608.f (23bits)
    // - 2147483648.f (31bits)
    // Any value greater than this, will have integral mantissa...
    // and no decimal part
    //
    // if ((abs(f) > 2**31 )) r = f;
    // const uint32x4_t _sign_bit = vreinterpretq_u32_f32(vdupq_n_f32(-0.f));
    const float32x4_t _max_f = vdupq_n_f32(8388608.f);
    uint32x4_t m = vcgtq_f32(_max_f, vabsq_f32(f));
    uint32x4_t r_u = vreinterpretq_u32_f32(r);
    uint32x4_t f_u = vreinterpretq_u32_f32(f);
    r_u = vorrq_u32(vandq_u32(m, r_u), vandq_u32(vmvnq_u32(m), f_u));

    return vreinterpretq_f32_u32(r_u);
}

static inline float32x4_t _neon_mm_round_ps(const float32x4_t &input)
{
    const uint32x4_t _sign_bit = vreinterpretq_u32_f32(vdupq_n_f32(-0.f));
    uint32x4_t input_sign = vandq_u32(_sign_bit, vreinterpretq_u32_f32(input));

    const uint32x4_t _half_positive = vreinterpretq_u32_f32(vdupq_n_f32(.5f));
    float32x4_t _half_signed = vreinterpretq_f32_u32(vorrq_u32(input_sign, _half_positive));

    float32x4_t f = vaddq_f32(input, _half_signed);

    // r = (float)(int)f;
    // float32x4_t r = vcvtq_f32_s32(vcvtq_s32_f32(f));
    uint32x4_t r = vreinterpretq_u32_f32(vcvtq_f32_s32(vcvtq_s32_f32(f)));

    // two possible values:
    // - 8388608.f (23bits)
    // - 2147483648.f (31bits)
    // Any value greater than this, will have integral mantissa...
    // and no decimal part
    //
    // if ((abs(f) > 2**31 )) r = f;
    // const __m128 _sign_bit = _mm_set1_ps(-0.f);
    const float32x4_t _max_f = vdupq_n_f32(8388608.f);
    uint32x4_t m = vcgtq_f32(_max_f, vabsq_f32(input));
    uint32x4_t input_u = vreinterpretq_u32_f32(input);
    r = vorrq_u32(vandq_u32(m, r), vandq_u32(vmvnq_u32(m), input_u));

    return vreinterpretq_f32_u32(r);
}

const uint32x4_t _float_info_all_bits_set = vdupq_n_u32(0xffffffff);
const uint32x4_t _float_info_mantissa_bit_u = vdupq_n_u32(0x007FFFFF);
const uint32x4_t _float_info_mantissa_min_u = vdupq_n_u32(1);
const uint32x4_t _float_info_sign_bit_u = vdupq_n_u32(0x80000000);
const uint32x4_t _float_info_minus_one_u = vdupq_n_u32(0xFFFFFFFF);
const uint32x4_t _float_info_one_u = vdupq_n_u32(1);
const uint32x4_t _float_info_number_except_sign_bit_u = vdupq_n_u32(0x7FFFFFFF);
const uint32x4_t _float_info_expoent_bit_u = vdupq_n_u32(0x7F800000);
const uint32x4_t _float_info_inf_u = vdupq_n_u32(0x7F800000);
const uint32x4_t _float_info_max_float_u = vdupq_n_u32(0x7F7FFFFF);
const uint32x4_t _float_info_zero = vdupq_n_u32(0);
const uint32x4_t _float_info_q_nan = vdupq_n_u32(0x7FC00000);

// v == v returns false on NaN
static inline uint32x4_t _neon_is_nan_ps(const float32x4_t &v)
{
    return veorq_u32(vceqq_f32(v, v), _float_info_all_bits_set);
}

static inline float32x4_t _neon_nextafter_ps(const float32x4_t &x, const float32x4_t &y)
{
    uint32x4_t bits_x = vreinterpretq_u32_f32(x);
    uint32x4_t bits_y = vreinterpretq_u32_f32(y);

    // NaN check
    uint32x4_t is_nan_mask = veorq_u32(_neon_is_nan_ps(x), _neon_is_nan_ps(y));

    // x == y
    uint32x4_t is_eq_mask = vceqq_f32(x, y);

    // y < x (descending)
    uint32x4_t is_descending = vcltq_f32(y, x);

    uint32x4_t bits_number_only = vandq_u32(bits_x, _float_info_number_except_sign_bit_u);

    // Zero check
    uint32x4_t is_zero = vceqq_u32(bits_number_only, _float_info_zero);
    uint32x4_t zero_result = vorrq_u32(vandq_u32(is_descending, _float_info_sign_bit_u), _float_info_mantissa_min_u);

    // Sign bit and its mask
    uint32x4_t sign_x = vandq_u32(bits_x, _float_info_sign_bit_u);
    uint32x4_t sign_mask = vreinterpretq_u32_s32(vshrq_n_s32(vreinterpretq_s32_u32(sign_x), 31));

    // Increment direction: -1 if descending XOR negative, else +1
    uint32x4_t increment = veorq_u32(is_descending, sign_mask);
    increment = vorrq_u32(vandq_u32(increment, _float_info_minus_one_u),
                          vandq_u32(vmvnq_u32(increment), _float_info_one_u));

    // Inf check
    uint32x4_t is_inf = vceqq_u32(bits_number_only, _float_info_inf_u);

    // For inf: use max_float with correct sign
    uint32x4_t max_float = vorrq_u32(sign_x, _float_info_max_float_u);

    // Increment or use max for inf
    uint32x4_t result_number = vaddq_u32(bits_number_only, increment);
    result_number = vorrq_u32(vandq_u32(is_inf, max_float),
                              vandq_u32(vmvnq_u32(is_inf), result_number));

    // Reconstruct with sign
    uint32x4_t result = vorrq_u32(sign_x, result_number);

    // Zero case
    result = vorrq_u32(vandq_u32(is_zero, zero_result),
                       vandq_u32(vmvnq_u32(is_zero), result));

    // x == y case
    result = vorrq_u32(vandq_u32(is_eq_mask, bits_y),
                       vandq_u32(vmvnq_u32(is_eq_mask), result));

    // NaN case
    result = vorrq_u32(vandq_u32(is_nan_mask, _float_info_q_nan),
                       vandq_u32(vmvnq_u32(is_nan_mask), result));

    return vreinterpretq_f32_u32(result);
}

#else

#endif

namespace ITKCommon
{

    static inline std::string PrintfToStdString(const char *format, ...)
    {

        std::vector<char> char_buffer;

        va_list args;

        va_start(args, format);
        char_buffer.resize(vsnprintf(nullptr, 0, format, args) + 1);
        va_end(args);

        va_start(args, format);
        int len = vsnprintf(char_buffer.data(), char_buffer.size(), format, args);
        va_end(args);

        return char_buffer.data();
    }

}

#define ITK_DECLARE_CREATE_SHARED(ClassName)                                                         \
private:                                                                                             \
    std::weak_ptr<ClassName> mSelf;                                                                  \
                                                                                                     \
public:                                                                                              \
    template <typename... _param_args>                                                               \
    static inline std::shared_ptr<ClassName> CreateShared(_param_args &&...args)                     \
    {                                                                                                \
        auto result = std::shared_ptr<ClassName>(new ClassName(std::forward<_param_args>(args)...)); \
        result->mSelf = std::weak_ptr<ClassName>(result);                                            \
        return result;                                                                               \
    }                                                                                                \
    inline std::shared_ptr<ClassName> self() const                                                   \
    {                                                                                                \
        return mSelf.lock();                                                                         \
    }                                                                                                \
    template <typename _ChildClassType>                                                              \
    inline typename std::enable_if<                                                                  \
        !std::is_same<_ChildClassType, ClassName>::value &&                                          \
            std::is_base_of<ClassName, _ChildClassType>::value,                                      \
        std::shared_ptr<_ChildClassType>>::type                                                      \
    self() const                                                                                     \
    {                                                                                                \
        return std::dynamic_pointer_cast<_ChildClassType>(self());                                   \
    }                                                                                                \
    template <typename _ParentClassType>                                                             \
    inline typename std::enable_if<                                                                  \
        !std::is_same<_ParentClassType, ClassName>::value &&                                         \
            std::is_base_of<_ParentClassType, ClassName>::value,                                     \
        std::shared_ptr<_ParentClassType>>::type                                                     \
    self() const                                                                                     \
    {                                                                                                \
        return std::shared_ptr<_ParentClassType>(self());                                            \
    }                                                                                                \
    template <typename _SameClassType>                                                               \
    inline typename std::enable_if<                                                                  \
        std::is_same<_SameClassType, ClassName>::value,                                              \
        std::shared_ptr<ClassName>>::type                                                            \
    self() const                                                                                     \
    {                                                                                                \
        return mSelf.lock();                                                                         \
    }

// static inline std::shared_ptr<ClassName> CreateShared()
// {
//     auto result = std::shared_ptr<ClassName>(new ClassName());
//     result->mSelf = std::weak_ptr<ClassName>(result);
//     return result;
// }

namespace ITKCommon
{
    // Created to allow reference any kind object
    // that inherits from this class
    class AttachableObject
    {
    public:
        virtual ~AttachableObject() = default;

        template <typename _ChildClassType>
        inline typename std::enable_if<
            !std::is_same<_ChildClassType, AttachableObject>::value &&
                std::is_base_of<AttachableObject, _ChildClassType>::value,
            _ChildClassType *>::type
        attached_cast_to() const { return (_ChildClassType *)(this); }

        template <typename _ParentClassType>
        inline typename std::enable_if<
            !std::is_same<_ParentClassType, AttachableObject>::value &&
                std::is_base_of<_ParentClassType, AttachableObject>::value,
            _ParentClassType *>::type
        attached_cast_to() const { return (_ParentClassType *)(this); }

        template <typename _SameClass>
        inline typename std::enable_if<
            std::is_same<_SameClass, AttachableObject>::value,
            _SameClass *>::type
        attached_cast_to() const { return this; }

        template <typename _AnyOtherClass>
        inline typename std::enable_if<
            (!std::is_same<_AnyOtherClass, AttachableObject>::value &&
             !std::is_base_of<_AnyOtherClass, AttachableObject>::value &&
             !std::is_base_of<AttachableObject, _AnyOtherClass>::value),
            _AnyOtherClass *>::type
        attached_cast_to() const { return nullptr; }

        template <typename _ClassType>
        static inline typename std::enable_if<
            std::is_same<_ClassType, AttachableObject>::value ||
                std::is_base_of<_ClassType, AttachableObject>::value ||
                std::is_base_of<AttachableObject, _ClassType>::value,
            bool>::type
        attached_can_cast() noexcept { return true; }

        template <typename _ClassType>
        static inline typename std::enable_if<
            !std::is_same<_ClassType, AttachableObject>::value &&
                !std::is_base_of<_ClassType, AttachableObject>::value &&
                !std::is_base_of<AttachableObject, _ClassType>::value,
            bool>::type
        attached_can_cast() noexcept { return false; }
    };

    class AttachedWeakObject
    {
    public:
        std::weak_ptr<ITKCommon::AttachableObject> value;
        template <typename _ChildClassType>
        inline typename std::enable_if<
            !std::is_same<_ChildClassType, ITKCommon::AttachableObject>::value &&
                std::is_base_of<ITKCommon::AttachableObject, _ChildClassType>::value,
            std::shared_ptr<_ChildClassType>>::type
        weakObjectAs() const { return std::dynamic_pointer_cast<_ChildClassType>(value.lock()); }
        template <typename _ParentClassType>
        inline typename std::enable_if<
            !std::is_same<_ParentClassType, ITKCommon::AttachableObject>::value &&
                std::is_base_of<_ParentClassType, ITKCommon::AttachableObject>::value,
            std::shared_ptr<_ParentClassType>>::type
        weakObjectAs() const { return std::shared_ptr<_ParentClassType>(value.lock()); }
        template <typename _SameClassType>
        inline typename std::enable_if<
            std::is_same<_SameClassType, ITKCommon::AttachableObject>::value,
            std::shared_ptr<ITKCommon::AttachableObject>>::type
        weakObjectAs() const { return value.lock(); }
        template <typename _AnyOtherClass>
        inline typename std::enable_if<
            (!std::is_same<_AnyOtherClass, ITKCommon::AttachableObject>::value &&
             !std::is_base_of<_AnyOtherClass, ITKCommon::AttachableObject>::value &&
             !std::is_base_of<ITKCommon::AttachableObject, _AnyOtherClass>::value),
            std::shared_ptr<_AnyOtherClass>>::type
        weakObjectAs() const { return nullptr; }
    };

    class AttachedSharedObject
    {
    public:
        std::shared_ptr<ITKCommon::AttachableObject> value;
        template <typename _ChildClassType>
        inline typename std::enable_if<
            !std::is_same<_ChildClassType, ITKCommon::AttachableObject>::value &&
                std::is_base_of<ITKCommon::AttachableObject, _ChildClassType>::value,
            std::shared_ptr<_ChildClassType>>::type
        sharedObjectAs() const { return std::dynamic_pointer_cast<_ChildClassType>(value); }
        template <typename _ParentClassType>
        inline typename std::enable_if<
            !std::is_same<_ParentClassType, ITKCommon::AttachableObject>::value &&
                std::is_base_of<_ParentClassType, ITKCommon::AttachableObject>::value,
            std::shared_ptr<_ParentClassType>>::type
        sharedObjectAs() const { return std::shared_ptr<_ParentClassType>(value); }
        template <typename _SameClassType>
        inline typename std::enable_if<
            std::is_same<_SameClassType, ITKCommon::AttachableObject>::value,
            std::shared_ptr<ITKCommon::AttachableObject>>::type
        sharedObjectAs() const { return value; }
        template <typename _AnyOtherClass>
        inline typename std::enable_if<
            (!std::is_same<_AnyOtherClass, ITKCommon::AttachableObject>::value &&
             !std::is_base_of<_AnyOtherClass, ITKCommon::AttachableObject>::value &&
             !std::is_base_of<ITKCommon::AttachableObject, _AnyOtherClass>::value),
            std::shared_ptr<_AnyOtherClass>>::type
        sharedObjectAs() const { return nullptr; }
    };

}
