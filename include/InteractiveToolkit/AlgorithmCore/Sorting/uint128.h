#pragma once

#include <stdint.h>
#include <utility>
#include <stdexcept>

namespace AlgorithmCore
{
#pragma push_macro("___need_to_define_uint128_type")
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__)
#if defined(__APPLE__) || defined(__linux__)
    typedef __uint128_t uint128;
// #define ___need_to_define_uint128_type
#elif defined(_MSC_VER)
// typedef unsigned __int128 uint128; // does not compile on all platforms
#define ___need_to_define_uint128_type
#elif defined(__GNUC__) || defined(__clang__)
    typedef __uint128_t uint128; // windows with gcc or clang
#else
#define ___need_to_define_uint128_type
#endif
#else
#define ___need_to_define_uint128_type
#endif

#if defined(___need_to_define_uint128_type)

    struct uint128;
    static inline uint128 operator*(const uint128 &a, const uint128 &b);
    static inline uint128 operator/(const uint128 &a, const uint128 &b);
    static inline uint128 operator+(const uint128 &a, const uint128 &b);
    static inline uint128 operator-(const uint128 &a, const uint128 &b);
    static inline uint128 operator<<(const uint128 &a, int shift);
    static inline uint128 operator>>(const uint128 &a, int shift);
    static inline uint128 operator|(const uint128 &a, const uint128 &b);
    static inline uint128 operator&(const uint128 &a, const uint128 &b);
    static inline bool operator>=(const uint128 &a, const uint128 &b);
    static inline bool operator>(const uint128 &a, const uint128 &b);
    static inline bool operator<=(const uint128 &a, const uint128 &b);
    static inline bool operator<(const uint128 &a, const uint128 &b);
    static inline bool operator==(const uint128 &a, const uint128 &b);
    static inline bool operator!=(const uint128 &a, const uint128 &b);

#if defined(ITK_SSE2)

    static ITK_INLINE uint16_t &_mm_u16_c(const __m128i &v, int i) noexcept
    {
        return ((uint16_t *)&v)[i];
    }

    static ITK_INLINE uint64_t &_mm_u64_c(const __m128i &v, int i) noexcept
    {
        return ((uint64_t *)&v)[i];
    }

// static ITK_INLINE void debug_print_c(const char* name, const __m128i &v) noexcept
// {
//     printf("%s: %.8x %.8x %.8x %.8x\n", name, _mm_u32_(v, 3), _mm_u32_(v, 2), _mm_u32_(v, 1), _mm_u32_(v, 0) );
// }
#define debug_print_c(...)

    struct alignas(16) uint128
#else
    struct uint128
#endif
    {
        union
        {
            struct
            {
                uint64_t low;
                uint64_t high;
            };
            uint16_t u16[8];
            uint32_t u32[4];
            uint64_t u64[2];

            __uint128_t u128;

#if defined(ITK_SSE2)
            __m128i _sse;
#endif
        };

        inline operator uint64_t() const
        {
            return low;
        }

        inline uint128()
        {
            low = 0;
            high = 0;
        }

#if defined(ITK_SSE2)
        inline uint128(const __m128i &v)
        {
            _sse = v;
        }
#endif

        inline uint128(const uint128 &v)
        {
#if defined(ITK_SSE2)
            _sse = v._sse;
#else
            low = v.low;
            high = v.high;
#endif
        }

        inline void operator=(const uint128 &v)
        {
#if defined(ITK_SSE2)
            _sse = v._sse;
#else
            low = v.low;
            high = v.high;
#endif
        }

        inline uint128(const uint64_t &l, const uint64_t &h)
        {
            low = l;
            high = h;
        }

        inline uint128(const uint64_t &v)
        {
            low = v;
            high = 0;
        }

        inline void operator=(const uint64_t &v)
        {
            low = v;
            high = 0;
        }

        uint128 &operator*=(const uint128 &other)
        {
#if defined(ITK_SSE2) && false

            const int el_0 = _MM_SHUFFLE(0, 0, 0, 0);
            const int el_1 = _MM_SHUFFLE(1, 1, 1, 1);
            const int el_2 = _MM_SHUFFLE(2, 2, 2, 2);
            const int el_3 = _MM_SHUFFLE(3, 3, 3, 3);
            const __m128i _32bitMask_ = _mm_setr_epi32(0xffffffff, 0, 0xffffffff, 0);
            const __m128i mask_0 = _mm_setr_epi32(0xffffffff, 0, 0, 0);
            const __m128i mask_1 = _mm_setr_epi32(0, 0, 0xffffffff, 0);

            __m128i s_0 = _mm_and_si128(_mm_shuffle_epi32(_sse, _MM_SHUFFLE(1, 1, 0, 0)), _32bitMask_);
            __m128i s_1 = _mm_and_si128(_mm_shuffle_epi32(_sse, _MM_SHUFFLE(3, 3, 2, 2)), _32bitMask_);
            // s = [ x x x x ]

            __m128i o = _mm_and_si128(_mm_shuffle_epi32(other._sse, el_0), _32bitMask_);

            __m128i s0o0_0 = _mm_mul_epu32(s_0, o);
            __m128i s0o0_1 = _mm_mul_epu32(s_1, o);

            __m128i _lower_part_0 = _mm_and_si128(s0o0_0, _32bitMask_);
            __m128i _lower_part_1 = _mm_and_si128(s0o0_1, _32bitMask_);

            __m128i _high_part_0 = _mm_srli_epi64(s0o0_0, 32);
            __m128i _high_part_1 = _mm_srli_epi64(s0o0_1, 32);

            _high_part_1 = _mm_or_si128(_mm_slli_si128(_high_part_1, 8), _mm_srli_si128(_high_part_0, 16 - 8));
            _high_part_0 = _mm_slli_si128(_high_part_0, 8);

            __m128i r_0 = _mm_add_epi64(_lower_part_0, _high_part_0);
            __m128i r_1 = _mm_add_epi64(_lower_part_1, _high_part_1);

            // first element high sum
            // 1st carry
            // __m128i carry = _mm_and_si128(_mm_shuffle_epi32(r_0, el_1), mask_1);
            // r_0 = _mm_add_epi64(r_0, carry);

            o = _mm_and_si128(_mm_shuffle_epi32(other._sse, el_1), _32bitMask_);

            s_1 = _mm_or_si128(_mm_slli_si128(s_1, 8), _mm_srli_si128(s_0, 16 - 8));
            s_0 = _mm_slli_si128(s_0, 8);
            // s = [ x x x 0 ]

            s0o0_0 = _mm_mul_epu32(s_0, o);
            s0o0_1 = _mm_mul_epu32(s_1, o);

            _lower_part_0 = _mm_and_si128(s0o0_0, _32bitMask_);
            _lower_part_1 = _mm_and_si128(s0o0_1, _32bitMask_);

            _high_part_0 = _mm_srli_epi64(s0o0_0, 32);
            _high_part_1 = _mm_srli_epi64(s0o0_1, 32);

            _high_part_1 = _mm_or_si128(_mm_slli_si128(_high_part_1, 8), _mm_srli_si128(_high_part_0, 16 - 8));
            //_high_part_0 = _mm_slli_si128(_high_part_0, 8);

            r_0 = _mm_add_epi64(r_0, _lower_part_0);
            r_1 = _mm_add_epi64(r_1, _mm_add_epi64(_lower_part_1, _high_part_1));

            // first element high sum
            // 2nd carry
            // carry = _mm_and_si128(_mm_shuffle_epi32(r_0, el_3), mask_0);
            // r_1 = _mm_add_epi64(r_1, carry);

            o = _mm_and_si128(_mm_shuffle_epi32(other._sse, el_2), _32bitMask_);

            s_1 = _mm_or_si128(_mm_slli_si128(s_1, 8), _mm_srli_si128(s_0, 16 - 8));
            //  s = [ x x 0 0 ]

            s0o0_1 = _mm_mul_epu32(s_1, o);

            _lower_part_1 = _mm_and_si128(s0o0_1, _32bitMask_);

            _high_part_1 = _mm_srli_epi64(s0o0_1, 32);

            _high_part_1 = _mm_slli_si128(_high_part_1, 8);

            r_1 = _mm_add_epi64(r_1, _mm_add_epi64(_lower_part_1, _high_part_1));

            // first element high sum
            // 3rd carry
            // carry = _mm_and_si128(_mm_shuffle_epi32(r_1, el_1), mask_1);
            // r_1 = _mm_add_epi64(r_1, carry);

            o = _mm_and_si128(_mm_shuffle_epi32(other._sse, el_3), _32bitMask_);

            s_1 = _mm_slli_si128(s_1, 8);
            //  s = [ x 0 0 0 ]

            s0o0_1 = _mm_mul_epu32(s_1, o);

            _lower_part_1 = _mm_and_si128(s0o0_1, _32bitMask_);

            r_1 = _mm_add_epi64(r_1, _lower_part_1);

            // first element high sum

            // compute all carry at end
            __m128i carry = _mm_and_si128(_mm_shuffle_epi32(r_0, el_1), mask_1);
            r_0 = _mm_add_epi64(r_0, carry);
            // carry = _mm_and_si128(_mm_shuffle_epi32(r_0, el_3), mask_0);
            // r_1 = _mm_add_epi64(r_1, carry);
            // carry = _mm_and_si128(_mm_shuffle_epi32(r_1, el_1), mask_1);
            carry = _mm_or_si128(
                _mm_and_si128(_mm_shuffle_epi32(r_0, el_3), mask_0),
                _mm_and_si128(_mm_shuffle_epi32(r_1, el_1), mask_1));
            r_1 = _mm_add_epi64(r_1, carry);

            r_0 = _mm_shuffle_epi32(r_0, _MM_SHUFFLE(2, 0, 2, 0));
            r_1 = _mm_shuffle_epi32(r_1, _MM_SHUFFLE(2, 0, 2, 0));

            const __m128i low_mask = _mm_setr_epi32(0xffffffff, 0xffffffff, 0, 0);
            const __m128i high_mask = _mm_setr_epi32(0, 0, 0xffffffff, 0xffffffff);

            r_0 = _mm_and_si128(r_0, low_mask);
            r_1 = _mm_and_si128(r_1, high_mask);

            _sse = _mm_or_si128(r_0, r_1);

#else
            const uint64_t _32bit_mask = UINT64_C(0x00000000ffffffff);

            uint64_t s0 = low & _32bit_mask;
            uint64_t s1 = low >> 32;
            uint64_t s2 = high & _32bit_mask;
            uint64_t s3 = high >> 32;

            uint64_t o0 = other.low & _32bit_mask;
            uint64_t o1 = other.low >> 32;
            uint64_t o2 = other.high & _32bit_mask;
            uint64_t o3 = other.high >> 32;

            uint64_t s0o0 = o0 * s0;
            uint64_t s1o0 = o0 * s1;
            uint64_t s2o0 = o0 * s2;
            uint64_t s3o0 = o0 * s3;

            // 1st multiplication
            uint64_t r0 = (s0o0 & _32bit_mask);
            uint64_t r1 = (s1o0 & _32bit_mask) + (s0o0 >> 32) + (r0 >> 32);
            uint64_t r2 = (s2o0 & _32bit_mask) + (s1o0 >> 32);
            uint64_t r3 = (s3o0 & _32bit_mask) + (s2o0 >> 32);

            uint64_t s0o1 = o1 * s0;
            uint64_t s1o1 = o1 * s1;
            uint64_t s2o1 = o1 * s2;

            // 2nd multiplication
            r1 += (s0o1 & _32bit_mask);
            r2 += (s1o1 & _32bit_mask) + (s0o1 >> 32) + (r1 >> 32);
            r3 += (s2o1 & _32bit_mask) + (s1o1 >> 32);

            uint64_t s0o2 = o2 * s0;
            uint64_t s1o2 = o2 * s1;

            // 3th multiplication
            r2 += (s0o2 & _32bit_mask);
            r3 += (s1o2 & _32bit_mask) + (s0o2 >> 32) + (r2 >> 32);

            uint64_t s0o3 = o3 * s0;

            // 4th multiplication
            r3 += (s0o3 & _32bit_mask);

            // result
            low = (r1 << 32) | (r0 & _32bit_mask);
            high = (r3 << 32) | (r2 & _32bit_mask);

#endif
            return *this;
        }

        // Division
        uint128 &operator/=(const uint128 &_divisor)
        {
            const uint128 uint128_0(0);
            const uint128 uint128_1(1);

            if (_divisor == uint128_0)
            {
                throw std::domain_error("division by 0");
            }
            else if (_divisor == uint128_1)
            {
                return *this;
            }
            else if (*this == _divisor)
            {
                *this = uint128_1;
                return *this;
            }
            else if ((*this == uint128_0) || (*this < _divisor))
            {
                *this = uint128_0;
                return *this;
            }

            const uint128 &dividend = *this;
            uint128 divisor = _divisor;

            uint128 quotient = 0;
            uint128 remainder = dividend;

            int shift_count = 0;
            while (divisor <= remainder)
            {
                divisor <<= 1;
                shift_count++;
            }

            while (shift_count >= 0)
            {
                divisor >>= 1;
                shift_count--;

                if (remainder >= divisor)
                {
                    remainder -= divisor;
                    quotient |= (uint128_1 << shift_count);
                }
            }

            *this = quotient;
            return *this;
        }

        uint128 &operator+=(const uint128 &b)
        {
            high += b.high + ((low + b.low) < low);
            low += b.low;
            return *this;
        }

        uint128 &operator-=(const uint128 &b)
        {
            high = high - b.high - ((low - b.low) > low);
            low = low - b.low;
            return *this;
        }

        uint128 &operator<<=(int shift)
        {
            high = high << shift | low >> (64 - shift);
            low = low << shift;
            return *this;
        }

        uint128 &operator>>=(int shift)
        {
            low = (high << (64 - shift)) + (low >> shift);
            high = high >> shift;
            return *this;
        }

        uint128 &operator|=(const uint128 &b)
        {
#if defined(ITK_SSE2)
            _sse = _mm_or_si128(_sse, b._sse);
#else
            low |= b.low;
            high |= b.high;
#endif
            return *this;
        }

        uint128 operator~() const
        {
#if defined(ITK_SSE2)
            const __m128i mask = _mm_set1_epi32(0xffffffff);
            return _mm_xor_si128(_sse, mask);
#else
            return uint128(~low, ~high);
#endif
        }

        uint128 &operator&=(const uint128 &b)
        {
#if defined(ITK_SSE2)
            _sse = _mm_and_si128(_sse, b._sse);
#else
            low &= b.low;
            high &= b.high;
#endif
            return *this;
        }
    };

    static inline uint128 operator*(const uint128 &a, const uint128 &b)
    {
        return uint128{a} *= b;
    }

    static inline uint128 operator/(const uint128 &a, const uint128 &b)
    {
        return uint128{a} /= b;
    }

    static inline uint128 operator+(const uint128 &a, const uint128 &b)
    {
        return uint128{a} += b;
    }

    static inline uint128 operator-(const uint128 &a, const uint128 &b)
    {
        return uint128{a} -= b;
    }

    static inline uint128 operator<<(const uint128 &a, int shift)
    {
        return uint128{a} <<= shift;
    }

    static inline uint128 operator>>(const uint128 &a, int shift)
    {
        return uint128{a} >>= shift;
    }

    static inline uint128 operator|(const uint128 &a, const uint128 &b)
    {
        return uint128{a} |= b;
    }

    static inline uint128 operator&(const uint128 &a, const uint128 &b)
    {
        return uint128{a} &= b;
    }

    static inline bool operator>=(const uint128 &a, const uint128 &b)
    {
        return a.high > b.high || (a.high == b.high && a.low >= b.low);
    }

    static inline bool operator>(const uint128 &a, const uint128 &b)
    {
        return a.high > b.high || (a.high == b.high && a.low > b.low);
    }

    static inline bool operator<=(const uint128 &a, const uint128 &b)
    {
        return a.high < b.high || (a.high == b.high && a.low <= b.low);
    }

    static inline bool operator<(const uint128 &a, const uint128 &b)
    {
        return a.high < b.high || (a.high == b.high && a.low < b.low);
    }

    static inline bool operator==(const uint128 &a, const uint128 &b)
    {
        return a.high == b.high && a.low == b.low;
    }

    static inline bool operator!=(const uint128 &a, const uint128 &b)
    {
        return a.high != b.high || a.low != b.low;
    }

#endif

#pragma pop_macro("___need_to_define_uint128_type")

}