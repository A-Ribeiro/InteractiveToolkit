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

            uint64_t uint64_byte0_self = low & UINT64_C(0xffffffff);
            uint64_t uint64_byte0_other = other.low & UINT64_C(0xffffffff);

            uint64_t uint64_byte1_self = low >> 32;
            uint64_t uint64_byte1_other = other.low >> 32;

            uint64_t uint64_byte2_self = high & UINT64_C(0xffffffff);
            uint64_t uint64_byte2_other = other.high & UINT64_C(0xffffffff);

            uint64_t uint64_byte3_self = high >> 32;
            uint64_t uint64_byte3_other = other.high >> 32;

            uint64_t uint64_byte0_res = uint64_byte0_self * uint64_byte0_other;
            uint64_t uint64_byte0_res_carry = uint64_byte0_res >> 32;

            // low_l = uint64_byte0_res & 0xffffffff;

            uint64_t uint64_byte1_res =
                uint64_byte1_self * uint64_byte0_other +
                uint64_byte0_self * uint64_byte1_other +
                uint64_byte0_res_carry;
            uint64_t uint64_byte1_res_carry = uint64_byte1_res >> 32;

            // low_h = uint64_byte1_res & 0xffffffff;
            low = (uint64_byte1_res << 32) | (uint64_byte0_res & UINT64_C(0xffffffff));

            uint64_t uint64_byte2_res =
                uint64_byte2_self * uint64_byte0_other +
                uint64_byte1_self * uint64_byte1_other +
                uint64_byte0_self * uint64_byte2_other +
                uint64_byte1_res_carry;
            uint64_t uint64_byte2_res_carry = uint64_byte2_res >> 32;

            // high_l = uint64_byte2_res & 0xffffffff;

            uint64_t uint64_byte3_res =
                uint64_byte3_self * uint64_byte0_other +
                uint64_byte2_self * uint64_byte1_other +
                uint64_byte1_self * uint64_byte2_other +
                uint64_byte0_self * uint64_byte3_other +
                uint64_byte2_res_carry;
            // uint64_t uint64_byte3_res_carry = uint64_byte3_res >> 32;

            // high_h = uint64_byte3_res & 0xffffffff;

            high = (uint64_byte3_res << 32) | (uint64_byte2_res & UINT64_C(0xffffffff));

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