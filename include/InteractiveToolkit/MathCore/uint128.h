#pragma once

#include <stdint.h>
#include <utility>
#include <stdexcept>

#if defined(_MSC_VER)
//  VisualStudio  Intrinsics
#include <intrin.h> // Required for _umul128

#endif

#define INT128_GNU_IMPLEMENTATION 1
#define INT128_MSVC_IMPLEMENTATION 2

namespace MathCore
{
#pragma push_macro("___need_to_define_uint128_type")
#if defined(__x86_64__) || defined(_M_X64) || defined(__aarch64__)
#if defined(__APPLE__) || defined(__linux__)
    typedef __uint128_t uint128;
#define INT128_MSVC_CHOOSE INT128_GNU_IMPLEMENTATION
// #define ___need_to_define_uint128_type
#elif defined(_MSC_VER)
#define ___need_to_define_uint128_type
#define INT128_MSVC_CHOOSE INT128_MSVC_IMPLEMENTATION
#elif defined(__GNUC__) || defined(__clang__)
    typedef __uint128_t uint128; // windows with gcc or clang
#define INT128_MSVC_CHOOSE INT128_GNU_IMPLEMENTATION
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

#if defined(ITK_AVX2)

    static inline __m256i _mm256_slli_si256_c_8bytes(const __m256i &v) noexcept
    {
        constexpr int shift_bytes = 8;

        //__m128i _l = _mm256_extracti128_si256(v, 0);
        //__m128i _h = _mm256_extracti128_si256(v, 1);

        //__m128i _l_sft = _mm_slli_si128(_l, shift_bytes);
        //__m128i _h_sft = _mm_slli_si128(_h, shift_bytes);

        //__m128i low_overflow = _mm_srli_si128(_l, 16 - shift_bytes);

        //__m128i _compose_h_sft = _mm_or_si128(_h_sft, low_overflow);

        // return _mm256_set_m128i(_compose_h_sft, _l_sft);

        __m128i low_overflow = _mm_srli_si128(_mm256_extracti128_si256(v, 0), 16 - shift_bytes);
        __m256i tmp = _mm256_set_m128i(low_overflow, _mm_setzero_si128());
        __m256i _128lane_shifted = _mm256_slli_si256(v, shift_bytes);

        return _mm256_or_si256(_128lane_shifted, tmp);
    }

#endif

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

        inline uint128 &operator=(const uint128 &v)
        {
#if defined(ITK_SSE2)
            _sse = v._sse;
#else
            low = v.low;
            high = v.high;
#endif
            return *this;
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

        inline uint128 &operator=(const uint64_t &v)
        {
            low = v;
            high = 0;

            return *this;
        }

        uint128 &operator*=(const uint128 &other)
        {
#if defined(ITK_AVX2) && false

            const int el_0 = _MM_SHUFFLE(0, 0, 0, 0);
            const int el_1 = _MM_SHUFFLE(1, 1, 1, 1);
            const int el_2 = _MM_SHUFFLE(2, 2, 2, 2);
            const int el_3 = _MM_SHUFFLE(3, 3, 3, 3);
            const __m256i _32bitMask_ = _mm256_set1_epi64x(UINT64_C(0x00000000ffffffff));

            const __m256i idx = _mm256_setr_epi32(0, 0, 1, 1, 2, 2, 3, 3);

            __m256i s = _mm256_and_si256(_mm256_permutevar8x32_epi32(_mm256_set_m128i(_sse, _sse), idx), _32bitMask_);
            // s = [ x x x x ]

            __m256i other_expanded = _mm256_and_si256(_mm256_permutevar8x32_epi32(_mm256_set_m128i(other._sse, other._sse), idx), _32bitMask_);

            __m256i o = _mm256_permute4x64_epi64(other_expanded, el_0);

            __m256i s0o0 = _mm256_mul_epu32(s, o);

            __m256i _lower_part = _mm256_and_si256(s0o0, _32bitMask_);
            __m256i _high_part = _mm256_srli_epi64(s0o0, 32);
            _high_part = _mm256_slli_si256_c_8bytes(_high_part);

            __m256i r = _mm256_add_epi64(_lower_part, _high_part);

            o = _mm256_permute4x64_epi64(other_expanded, el_1);
            s = _mm256_slli_si256_c_8bytes(s);
            // s = [ x x x 0 ]

            s0o0 = _mm256_mul_epu32(s, o);

            _lower_part = _mm256_and_si256(s0o0, _32bitMask_);
            _high_part = _mm256_srli_epi64(s0o0, 32);
            _high_part = _mm256_slli_si256_c_8bytes(_high_part);

            r = _mm256_add_epi64(r, _mm256_add_epi64(_lower_part, _high_part));

            o = _mm256_permute4x64_epi64(other_expanded, el_2);
            s = _mm256_slli_si256_c_8bytes(s);
            //  s = [ x x 0 0 ]

            s0o0 = _mm256_mul_epu32(s, o);

            _lower_part = _mm256_and_si256(s0o0, _32bitMask_);
            _high_part = _mm256_srli_epi64(s0o0, 32);
            // 128bit lane shift... the lanes are only in the 128bit high part
            _high_part = _mm256_slli_si256(_high_part, 8);

            r = _mm256_add_epi64(r, _mm256_add_epi64(_lower_part, _high_part));

            o = _mm256_permute4x64_epi64(other_expanded, el_3);
            // 128bit lane shift... the lanes are only in the 128bit high part
            s = _mm256_slli_si256(s, 8);
            //  s = [ x 0 0 0 ]

            s0o0 = _mm256_mul_epu32(s, o);

            _lower_part = _mm256_and_si256(s0o0, _32bitMask_);

            r = _mm256_add_epi64(r, _lower_part);

            // compute all carry at end
            const __m256i mask_0 = _mm256_setr_epi64x(UINT64_C(0x00000000ffffffff), 0, 0, 0);
            const __m256i mask_1 = _mm256_setr_epi64x(0, UINT64_C(0x00000000ffffffff), 0, 0);
            const __m256i mask_2 = _mm256_setr_epi64x(0, 0, UINT64_C(0x00000000ffffffff), 0);
            const __m256i mask_3 = _mm256_setr_epi64x(0, 0, 0, UINT64_C(0x00000000ffffffff));

            const __m256i _8_el_1 = _mm256_set1_epi32(1);
            const __m256i _8_el_3 = _mm256_set1_epi32(3);
            const __m256i _8_el_5 = _mm256_set1_epi32(5);

            __m256i carry = _mm256_and_si256(_mm256_permutevar8x32_epi32(r, _8_el_1), mask_1);
            r = _mm256_add_epi64(r, carry);
            carry = _mm256_and_si256(_mm256_permutevar8x32_epi32(r, _8_el_3), mask_2);
            r = _mm256_add_epi64(r, carry);
            carry = _mm256_and_si256(_mm256_permutevar8x32_epi32(r, _8_el_5), mask_3);
            r = _mm256_add_epi64(r, carry);

            const __m256i low_32bit_extract = _mm256_setr_epi32(0, 2, 4, 6, 0, 2, 4, 6);

            r = _mm256_permutevar8x32_epi32(r, low_32bit_extract);

            _sse = _mm256_extracti128_si256(r, 0);

#elif defined(ITK_SSE2) && false

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
            carry = _mm_and_si128(_mm_shuffle_epi32(r_0, el_3), mask_0);
            r_1 = _mm_add_epi64(r_1, carry);
            carry = _mm_and_si128(_mm_shuffle_epi32(r_1, el_1), mask_1);
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
            else if (high == 0 && _divisor.high == 0)
            {
                low /= _divisor.low;
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

            while (shift_count > 0)
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
            if (shift >= 64)
            {
                high = (low << (shift - 64)) & -(shift != 128);
                low = 0;
            }
            else
            {
                high = (high << shift) | ((low >> (64 - shift)) & -(shift != 0));
                low <<= shift;
            }
            return *this;
        }

        uint128 &operator>>=(int shift)
        {
            if (shift >= 64)
            {
                low = (high >> (shift - 64)) & -(shift != 128);
                high = 0;
            }
            else
            {
                low = (low >> shift) | ((high << (64 - shift))) & -(shift != 0);
                high >>= shift;
            }
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

#if defined(INT128_MSVC_CHOOSE) && INT128_MSVC_CHOOSE == INT128_GNU_IMPLEMENTATION

    static inline void multiply_uint64_to_uint128(uint64_t a, uint64_t b, uint64_t *r_high, uint64_t *r_low) noexcept
    {
        __uint128_t result = static_cast<__uint128_t>(a) * static_cast<__uint128_t>(b);
        *r_low = static_cast<uint64_t>(result);
        *r_high = static_cast<uint64_t>(result >> 64);
    }
    static inline void multiply_int64_to_int128(int64_t a, int64_t b, uint64_t *r_high, uint64_t *r_low) noexcept
    {
        __uint128_t result = static_cast<__uint128_t>(static_cast<__int128_t>(a) * static_cast<__int128_t>(b));
        *r_low = static_cast<uint64_t>(result);
        *r_high = static_cast<uint64_t>(result >> 64);
    }
    static inline uint64_t divide_uint128_by_uint64(uint64_t high, uint64_t low, uint64_t divisor) noexcept
    {
        __uint128_t dividend = (static_cast<__uint128_t>(high) << 64) | low;
        return static_cast<uint64_t>(dividend / divisor);
    }
    static inline int64_t divide_int128_by_int64(uint64_t high, uint64_t low, int64_t divisor) noexcept
    {
        __int128_t dividend = static_cast<__int128_t>((static_cast<__uint128_t>(high) << 64) | low);
        return static_cast<int64_t>(dividend / divisor);
    }

#elif defined(INT128_MSVC_CHOOSE) && INT128_MSVC_CHOOSE == INT128_MSVC_IMPLEMENTATION

    static inline void multiply_uint64_to_uint128(uint64_t a, uint64_t b, uint64_t *r_high, uint64_t *r_low) noexcept
    {
        *r_low = _umul128(a, b, r_high);
    }
    static inline void multiply_int64_to_int128(int64_t a, int64_t b, uint64_t *r_high, uint64_t *r_low) noexcept
    {
        *r_low = static_cast<uint64_t>(_mul128(a, b, (int64_t *)r_high));
    }
    static inline uint64_t divide_uint128_by_uint64(uint64_t high, uint64_t low, uint64_t divisor) noexcept
    {
        uint64_t remainder;
        return _udiv128(high, low, divisor, &remainder);
    }
    static inline int64_t divide_int128_by_int64(uint64_t high, uint64_t low, int64_t divisor) noexcept
    {
        int64_t remainder;
        return _div128(static_cast<int64_t>(high), static_cast<int64_t>(low), divisor, &remainder);
    }

#else

    static inline void multiply_uint64_to_uint128(uint64_t a, uint64_t b, uint64_t *r_high, uint64_t *r_low) noexcept
    {
        // uint128 result = uint128(a) * uint128(b);
        // *r_low = result.low;
        // *r_high = result.high;

        // Karatsuba multiplication algorithm for 64-bit integers
        // This algorithm is more efficient than the naive approach for large numbers.

#if defined(ITK_SSE2)

        uint32_t a_lo = (uint32_t)a;
        uint32_t a_hi = a >> 32;
        uint32_t b_lo = (uint32_t)b;
        uint32_t b_hi = b >> 32;

#if defined(ITK_AVX2)
        __m256i a_simd = _mm256_setr_epi32(a_lo, 0, a_hi, 0, a_lo, 0, a_hi, 0);
        __m256i b_simd = _mm256_setr_epi32(b_lo, 0, b_hi, 0, b_hi, 0, b_lo, 0);

        __m256i p0_p3_p1_p2 = _mm256_mul_epu32(a_simd, b_simd);

        uint64_t &p0 = _mm256_u64_(p0_p3_p1_p2, 0);
        uint64_t &p3 = _mm256_u64_(p0_p3_p1_p2, 1);
        uint64_t &p1 = _mm256_u64_(p0_p3_p1_p2, 2);
        uint64_t &p2 = _mm256_u64_(p0_p3_p1_p2, 3);
#else
        __m128i a_simd = _mm_setr_epi32(a_lo, 0, a_hi, 0);
        __m128i b_simd = _mm_setr_epi32(b_lo, 0, b_hi, 0);
        __m128i b_hi_lo_simd = _mm_shuffle_epi32(b_simd, _MM_SHUFFLE(3, 0, 1, 2)); // (b_hi, 0, b_lo, 0)

        __m128i p0_p3 = _mm_mul_epu32(a_simd, b_simd);       // p0 = a_lo * b_lo, p3 = a_hi * b_hi
        __m128i p1_p2 = _mm_mul_epu32(a_simd, b_hi_lo_simd); // p1 = a_lo * b_hi, p2 = a_hi * b_lo

        uint64_t &p0 = _mm_u64_(p0_p3, 0); // Extract p0 (lower 64 bits)
        uint64_t &p3 = _mm_u64_(p0_p3, 1); // Extract p3 (upper 64 bits)
        uint64_t &p1 = _mm_u64_(p1_p2, 0); // Extract p1 (lower 64 bits)
        uint64_t &p2 = _mm_u64_(p1_p2, 1); // Extract p2 (upper 64 bits)
#endif
        uint64_t mid1 = p1 + (p0 >> 32);
        uint64_t mid2 = p2;

        uint64_t mid1_lo = (uint32_t)mid1;
        uint64_t mid1_hi = mid1 >> 32;

        uint64_t sum = mid2 + mid1_lo;
        uint64_t sum_hi = (sum < mid2) ? 1 : 0; // carry

        *r_low = (p0 & UINT64_C(0xFFFFFFFF)) | (sum << 32);
        *r_high = p3 + mid1_hi + (sum >> 32) + sum_hi;

#elif defined(ITK_NEON)

        uint32_t a_lo = (uint32_t)a;
        uint32_t a_hi = a >> 32;
        uint32_t b_lo = (uint32_t)b;
        uint32_t b_hi = b >> 32;

        uint32x2_t a_simd = (uint32x2_t){a_lo, a_hi};
        uint32x2_t b_simd = (uint32x2_t){b_lo, b_hi};
        uint32x2_t b_hi_lo_simd = vrev64_u32(b_simd); // (b_hi, 0, b_lo, 0)

        uint64x2_t p0_p3 = vmull_u32(a_simd, b_simd);       // p0 = a_lo * b_lo, p3 = a_hi * b_hi
        uint64x2_t p1_p2 = vmull_u32(a_simd, b_hi_lo_simd); // p1 = a_lo * b_hi, p2 = a_hi * b_lo

        uint64_t p0 = vgetq_lane_u64(p0_p3, 0); // Extract p0 (lower 64 bits)
        uint64_t p3 = vgetq_lane_u64(p0_p3, 1); // Extract p3 (upper 64 bits)
        uint64_t p1 = vgetq_lane_u64(p1_p2, 0); // Extract p1 (lower 64 bits)
        uint64_t p2 = vgetq_lane_u64(p1_p2, 1); // Extract p2 (upper 64 bits)

        uint64_t mid1 = p1 + (p0 >> 32);
        uint64_t mid2 = p2;

        uint64_t mid1_lo = (uint32_t)mid1;
        uint64_t mid1_hi = mid1 >> 32;

        uint64_t sum = mid2 + mid1_lo;
        uint64_t sum_hi = (sum < mid2) ? 1 : 0; // carry

        *r_low = (p0 & UINT64_C(0xFFFFFFFF)) | (sum << 32);
        *r_high = p3 + mid1_hi + (sum >> 32) + sum_hi;

#else
        uint64_t a_lo = (uint32_t)a;
        uint64_t a_hi = a >> 32;
        uint64_t b_lo = (uint32_t)b;
        uint64_t b_hi = b >> 32;

        uint64_t p0 = a_lo * b_lo;
        uint64_t p1 = a_lo * b_hi;
        uint64_t p2 = a_hi * b_lo;
        uint64_t p3 = a_hi * b_hi;

        uint64_t mid1 = p1 + (p0 >> 32);
        uint64_t mid2 = p2;

        uint64_t mid1_lo = (uint32_t)mid1;
        uint64_t mid1_hi = mid1 >> 32;

        uint64_t sum = mid2 + mid1_lo;
        uint64_t sum_hi = (sum < mid2) ? 1 : 0; // carry

        *r_low = (p0 & UINT64_C(0xFFFFFFFF)) | (sum << 32);
        *r_high = p3 + mid1_hi + (sum >> 32) + sum_hi;
#endif
    }
    static inline void multiply_int64_to_int128(int64_t a, int64_t b, uint64_t *r_high, uint64_t *r_low) noexcept
    {
        // uint128 result =
        //     uint128(static_cast<uint64_t>(a), static_cast<uint64_t>(a >> 63)) *
        //     uint128(static_cast<uint64_t>(b), static_cast<uint64_t>(b >> 63));

        // // bool s_a = a < 0;
        // // bool s_b = b < 0;

        // // bool s_r = s_a ^ s_b;

        // // uint128 result = (s_r)
        // // 					 ? uint128(0) - (uint128((s_a) ? -a : a) * uint128((s_b) ? -b : b))
        // // 					 : uint128((s_a) ? -a : a) * uint128((s_b) ? -b : b);

        // *r_low = result.low;
        // *r_high = result.high;

        // Handle sign
        bool neg = (a < 0) ^ (b < 0);

        // Absolute values, careful with INT64_MIN
        uint64_t ua = (a < 0) ? static_cast<uint64_t>(-(a + 1)) + 1 : static_cast<uint64_t>(a);
        uint64_t ub = (b < 0) ? static_cast<uint64_t>(-(b + 1)) + 1 : static_cast<uint64_t>(b);

        // Unsigned multiplication (reuse Karatsuba)
        uint64_t hi, lo;
        multiply_uint64_to_uint128(ua, ub, &hi, &lo);

        // If result should be negative, negate the 128-bit value
        if (neg)
        {
            // Two's complement negation of 128-bit value
            lo = ~lo;
            hi = ~hi;
            lo += 1;
            if (lo == 0)
                hi += 1;
        }

        *r_low = lo;
        *r_high = hi;
    }
    static inline uint64_t divide_uint128_by_uint64(uint64_t high, uint64_t low, uint64_t divisor) noexcept
    {
        uint128 result = uint128(low, high) / uint128(divisor);
        return result.low;
    }
    static inline int64_t divide_int128_by_int64(uint64_t high, uint64_t low, int64_t divisor) noexcept
    {
        uint128 dividend = uint128(low, high);

        bool s_a = (dividend.high & UINT64_C(0x8000000000000000)) != 0;
        bool s_b = divisor < 0;

        bool s_r = s_a ^ s_b;

        uint128 result =
            (s_r)
                ? uint128(0) - (((s_a) ? uint128(0) - dividend : dividend) / uint128((s_b) ? -divisor : divisor))
                : (((s_a) ? uint128(0) - dividend : dividend) / uint128((s_b) ? -divisor : divisor));

        return result.low;
    }

#endif

#pragma pop_macro("___need_to_define_uint128_type")

}