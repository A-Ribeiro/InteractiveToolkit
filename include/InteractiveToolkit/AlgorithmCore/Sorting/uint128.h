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
    typedef unsigned __int128 uint128;
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
            // MIT license
            // adapted from: https://github.com/calccrypto/uint128_t/blob/0c916f210828adc2e2edd35497456249563239a7/uint128_t.cpp#L317

            const uint64_t _32bit_mask = UINT64_C(0x00000000ffffffff); 

            // split values into 4 32-bit parts
            uint64_t top[4] = {
                low & _32bit_mask,
                low >> 32, 
                high & _32bit_mask, 
                high >> 32
            };
            uint64_t bottom[4] = {other.high >> 32, other.high & _32bit_mask, other.low >> 32, other.low & _32bit_mask};
            //uint64_t products[4][4];

            // multiply each component of the values

            // for (int x = 0; x < 4; x++)
            //     for (int y = 0; y < 4; y++)
            //         products[x][y] = top[x] * bottom[y];

            uint64_t products_0_3 = top[0] * bottom[3];
            uint64_t products_0_2 = top[0] * bottom[2];
            uint64_t products_0_1 = top[0] * bottom[1];
            uint64_t products_0_0 = top[0] * bottom[0];

            // first row
            uint64_t fourth32 = (products_0_3 & _32bit_mask);
            uint64_t third32 = (products_0_2 & _32bit_mask) + (products_0_3 >> 32);
            uint64_t second32 = (products_0_1 & _32bit_mask) + (products_0_2 >> 32);
            uint64_t first32 = (products_0_0 & _32bit_mask) + (products_0_1 >> 32);


            uint64_t products_1_3 = top[1] * bottom[3];
            uint64_t products_1_2 = top[1] * bottom[2];
            uint64_t products_1_1 = top[1] * bottom[1];

            // second row
            third32 += (products_1_3 & _32bit_mask);
            second32 += (products_1_2 & _32bit_mask) + (products_1_3 >> 32);
            first32 += (products_1_1 & _32bit_mask) + (products_1_2 >> 32);

            uint64_t products_2_3 = top[2] * bottom[3];
            uint64_t products_2_2 = top[2] * bottom[2];

            // third row
            second32 += (products_2_3 & _32bit_mask);
            first32 += (products_2_2 & _32bit_mask) + (products_2_3 >> 32);

            uint64_t products_3_3 = top[3] * bottom[3];

            // fourth row
            first32 += (products_3_3 & _32bit_mask);

            // move carry to next digit
            third32 += fourth32 >> 32;
            second32 += third32 >> 32;
            first32 += second32 >> 32;

            // remove carry from current digit
            fourth32 &= _32bit_mask;
            third32 &= _32bit_mask;
            second32 &= _32bit_mask;
            first32 &= _32bit_mask;

            // combine components
            low = (third32 << 32) | fourth32;
            high = (first32 << 32) | second32;
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