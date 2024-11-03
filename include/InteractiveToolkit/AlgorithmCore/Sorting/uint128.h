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

    struct uint128
    {
        uint64_t low;
        uint64_t high;

        inline operator uint64_t() const
        {
            return low;
        }

        inline uint128()
        {
            low = 0;
            high = 0;
        }

        inline uint128(const uint128 &v)
        {
            low = v.low;
            high = v.high;
        }

        inline void operator=(const uint128 &v)
        {
            low = v.low;
            high = v.high;
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

            return *this;
        }

        // Division
        uint128 &operator/=(const uint128 &_divisor)
        {
            const uint128 uint128_0(0);
            const uint128 uint128_1(1);
            // if (divisor == uint128_0)
            // {
            //     throw std::domain_error("Error: division or modulus by 0");
            // }
            // else
            if (_divisor == uint128_1)
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
            low |= b.low;
            high |= b.high;
            return *this;
        }

        uint128 operator~() const
        {
            return uint128(~low, ~high);
        }

        uint128 &operator&=(const uint128 &b)
        {
            low &= b.low;
            high &= b.high;
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