#pragma once

// #include <InteractiveToolkit/InteractiveToolkit.h>
// #include "../common.h"
#include "uint128.h"
#include <stdint.h>
#include <type_traits>

namespace MathCore
{

	namespace FixedPoint
	{

		template <typename _T>
		struct StoreSpecType
		{
		};

		template <>
		struct StoreSpecType<uint32_t>
		{
			using _is_signed = std::false_type;

			typedef uint32_t valueT;
			typedef uint32_t u_valueT;

			typedef uint64_t sizeT;
			typedef int64_t rangeT;
			typedef int64_t mul_div_T;

			static constexpr valueT SIGN_MASK = UINT32_C(0x80000000);

			static constexpr int SHIFT_GET_SIGN_MINUS_ONE = 30;
			static constexpr valueT FIRST_BIT_ZERO = UINT32_C(0xfffffffe);

			static constexpr int VALUE_TOTAL_BITS = 32;

			static constexpr rangeT INTEGER_PART_MIN_RANGE(int bits) noexcept { return 0; }
			static constexpr rangeT INTEGER_PART_MAX_RANGE(int bits) noexcept { return (rangeT(1) << bits) - rangeT(1); }
			static constexpr rangeT FRACTIONAL_PART_MIN_RANGE(int bits) noexcept { return 0; }
			static constexpr rangeT FRACTIONAL_PART_MAX_RANGE(int bits) noexcept { return (rangeT(1) << bits) - rangeT(1); }
		};

		template <>
		struct StoreSpecType<int32_t>
		{
			using _is_signed = std::true_type;

			typedef int32_t valueT;
			typedef uint32_t u_valueT;

			typedef uint64_t sizeT;
			typedef int64_t rangeT;
			typedef int64_t mul_div_T;

			static constexpr valueT SIGN_MASK = INT32_C(0x80000000);

			static constexpr int SHIFT_GET_SIGN_MINUS_ONE = 30;
			static constexpr valueT FIRST_BIT_ZERO = INT32_C(0xfffffffe);

			static constexpr int VALUE_TOTAL_BITS = 32;

			static constexpr rangeT INTEGER_PART_MIN_RANGE(int bits) noexcept { return -(rangeT(1) << (bits - 1)); }
			static constexpr rangeT INTEGER_PART_MAX_RANGE(int bits) noexcept { return (rangeT(1) << (bits - 1)) - rangeT(1); }
			static constexpr rangeT FRACTIONAL_PART_MIN_RANGE(int bits) noexcept { return -((rangeT(1) << bits) - rangeT(1)); }
			static constexpr rangeT FRACTIONAL_PART_MAX_RANGE(int bits) noexcept { return (rangeT(1) << bits) - rangeT(1); }
		};

		template <>
		struct StoreSpecType<uint64_t>
		{
			using _is_signed = std::false_type;

			typedef uint64_t valueT;
			typedef uint64_t u_valueT;

			// the best int type for size and range is the int128
			// but int64 is best supported by compilers in general.
			// This implementation is guaranteeded to be working
			// with the following ranges for the fractional part:
			//     - min of 2 bits
			//     - max of 62 bits
			typedef uint64_t sizeT;
			typedef int64_t rangeT;
			typedef uint128 mul_div_T;

			static constexpr valueT SIGN_MASK = UINT64_C(0x8000000000000000);

			static constexpr int SHIFT_GET_SIGN_MINUS_ONE = 62;
			static constexpr valueT FIRST_BIT_ZERO = UINT64_C(0xfffffffffffffffe);

			static constexpr int VALUE_TOTAL_BITS = 64;

			static constexpr rangeT INTEGER_PART_MIN_RANGE(int bits) noexcept { return 0; }
			static constexpr rangeT INTEGER_PART_MAX_RANGE(int bits) noexcept { return (rangeT(1) << bits) - rangeT(1); }
			static constexpr rangeT FRACTIONAL_PART_MIN_RANGE(int bits) noexcept { return 0; }
			static constexpr rangeT FRACTIONAL_PART_MAX_RANGE(int bits) noexcept { return (rangeT(1) << bits) - rangeT(1); }
		};

		template <>
		struct StoreSpecType<int64_t>
		{
			using _is_signed = std::true_type;

			typedef int64_t valueT;
			typedef uint64_t u_valueT;

			// the best int type for size and range is the int128
			// but int64 is best supported by compilers in general.
			// This implementation is guaranteeded to be working
			// with the following ranges for the fractional part:
			//     - min of 2 bits
			//     - max of 62 bits
			typedef uint64_t sizeT;
			typedef int64_t rangeT;
			typedef uint128 mul_div_T;

			static constexpr valueT SIGN_MASK = UINT64_C(0x8000000000000000);

			static constexpr int SHIFT_GET_SIGN_MINUS_ONE = 62;
			static constexpr valueT FIRST_BIT_ZERO = UINT64_C(0xfffffffffffffffe);

			static constexpr int VALUE_TOTAL_BITS = 64;

			static constexpr rangeT INTEGER_PART_MIN_RANGE(int bits) noexcept { return -(rangeT(1) << (bits - 1)); }
			static constexpr rangeT INTEGER_PART_MAX_RANGE(int bits) noexcept { return (rangeT(1) << (bits - 1)) - rangeT(1); }
			static constexpr rangeT FRACTIONAL_PART_MIN_RANGE(int bits) noexcept { return -((rangeT(1) << bits) - rangeT(1)); }
			static constexpr rangeT FRACTIONAL_PART_MAX_RANGE(int bits) noexcept { return (rangeT(1) << bits) - rangeT(1); }
		};

		template <typename store_type, int frac_bits>
		class fixed_t
		{
		public:
			using StoreSpec = StoreSpecType<store_type>;

			typedef typename StoreSpec::valueT valueT;
			typedef typename StoreSpec::u_valueT u_valueT;
			typedef typename StoreSpec::sizeT sizeT;
			typedef typename StoreSpec::rangeT rangeT;
			typedef typename StoreSpec::mul_div_T mul_div_T;

		private:
			static const int FRACTIONAL_BITS = frac_bits;
			static const sizeT FRACTIONAL_MASK = (sizeT(1) << FRACTIONAL_BITS) - sizeT(1);
			static const sizeT FACTOR = sizeT(1) << FRACTIONAL_BITS;

		public:
			valueT value;

			static const int INT_BITS = StoreSpec::VALUE_TOTAL_BITS - FRACTIONAL_BITS;
			static const rangeT INT_RANGE_MIN = StoreSpec::INTEGER_PART_MIN_RANGE(INT_BITS);
			static const rangeT INT_RANGE_MAX = StoreSpec::INTEGER_PART_MAX_RANGE(INT_BITS);

			static const int FRAC_BITS = FRACTIONAL_BITS;
			static const rangeT FRAC_RANGE_MIN = StoreSpec::FRACTIONAL_PART_MIN_RANGE(FRAC_BITS);
			static const rangeT FRAC_RANGE_MAX = StoreSpec::FRACTIONAL_PART_MAX_RANGE(FRAC_BITS);

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			inline fixed_t(valueT integerPart, valueT fractionalPart)
			{
				value = (integerPart << FRACTIONAL_BITS) + fractionalPart;
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			inline fixed_t(valueT integerPart, valueT fractionalPart)
			{
				value = (integerPart << FRACTIONAL_BITS) | (fractionalPart & FRACTIONAL_MASK);
			}

			inline fixed_t()
			{
				value = 0;
			}

			inline fixed_t(valueT integerPart)
			{
				value = (integerPart << FRACTIONAL_BITS);
			}

			inline fixed_t(const fixed_t &rawValue)
			{
				value = rawValue.value;
			}

			inline void operator=(const fixed_t &rawValue)
			{
				value = rawValue.value;
			}

			static constexpr fixed_t fromRaw(valueT rawValue) noexcept
			{
				fixed_t result;
				result.value = rawValue;
				return result;
			}

			//
			// Arithmetic block
			//

			inline fixed_t &operator+=(const fixed_t &other)
			{
				value += other.value;
				return *this;
			}

			inline fixed_t &operator-=(const fixed_t &other)
			{
				value -= other.value;
				return *this;
			}

			inline fixed_t operator-() const
			{
				return fixed_t::fromRaw(-value);
			}

			// special case for int64_t...
			// do the multiplication using uint128,
			// but considering the signal...
			template <typename T = StoreSpec, typename std::enable_if<std::is_same<typename T::valueT, int64_t>::value, bool>::type = true>
			inline fixed_t &operator*=(const fixed_t &other)
			{
				bool value_neg = (value & StoreSpec::SIGN_MASK) != 0;
				bool other_neg = (other.value & StoreSpec::SIGN_MASK) != 0;

				mul_div_T result = mul_div_T((value_neg) ? -value : value);
				result *= mul_div_T((other_neg) ? -other.value : other.value);
				result >>= FRACTIONAL_BITS;

				bool neg_result = value_neg ^ other_neg;

				value = (neg_result) ? -(valueT)result : (valueT)result;
				return *this;
			}

			template <typename T = StoreSpec, typename std::enable_if<!std::is_same<typename T::valueT, int64_t>::value, bool>::type = true>
			inline fixed_t &operator*=(const fixed_t &other)
			{
				mul_div_T result = mul_div_T(value);
				result *= mul_div_T(other.value);
				result >>= FRACTIONAL_BITS;
				value = (valueT)result;
				return *this;
			}

			// special case for int64_t...
			// do the multiplication using uint128,
			// but considering the signal...
			template <typename T = StoreSpec, typename std::enable_if<std::is_same<typename T::valueT, int64_t>::value, bool>::type = true>
			inline fixed_t &operator/=(const fixed_t &other)
			{
				bool value_neg = (value & StoreSpec::SIGN_MASK) != 0;
				bool other_neg = (other.value & StoreSpec::SIGN_MASK) != 0;

				mul_div_T result = mul_div_T((value_neg) ? -value : value);
				result <<= FRACTIONAL_BITS;
				result /= mul_div_T((other_neg) ? -other.value : other.value);

				bool neg_result = value_neg ^ other_neg;

				value = (neg_result) ? -(valueT)result : (valueT)result;
				return *this;
			}

			template <typename T = StoreSpec, typename std::enable_if<!std::is_same<typename T::valueT, int64_t>::value, bool>::type = true>
			inline fixed_t &operator/=(const fixed_t &other)
			{
				mul_div_T result = mul_div_T(value);
				result <<= FRACTIONAL_BITS;
				result /= mul_div_T(other.value);
				value = (valueT)result;
				return *this;
			}

			//
			// Bitwise block
			//

			inline fixed_t &operator&=(const fixed_t &other)
			{
				value &= other.value;
				return *this;
			}
			inline fixed_t &operator|=(const fixed_t &other)
			{
				value |= other.value;
				return *this;
			}
			inline fixed_t &operator^=(const fixed_t &other)
			{
				value ^= other.value;
				return *this;
			}
			inline fixed_t operator~() const
			{
				return fixed_t::fromRaw(~value);
			}

			//
			// Shift block
			//
			inline fixed_t &operator<<=(int shift)
			{
				value <<= shift;
				return *this;
			}
			inline fixed_t &operator>>=(int shift)
			{
				value >>= shift;
				return *this;
			}

			//
			// Comparison block
			//
			inline bool operator>(const fixed_t &other) const { return value > other.value; }
			inline bool operator>=(const fixed_t &other) const { return value >= other.value; }
			inline bool operator<(const fixed_t &other) const { return value < other.value; }
			inline bool operator<=(const fixed_t &other) const { return value <= other.value; }
			inline bool operator==(const fixed_t &other) const { return value == other.value; }
			inline bool operator!=(const fixed_t &other) const { return value != other.value; }

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			double toDouble() const
			{
				bool sign_negative = (value & StoreSpec::SIGN_MASK) != 0;
				u_valueT uvalue = (sign_negative) ? -value : value;
				u_valueT integerPart = uvalue >> FRACTIONAL_BITS;
				u_valueT fractionalPart = uvalue & FRACTIONAL_MASK;
				double number = (double)integerPart + (double)fractionalPart / (double)FACTOR;
				return (sign_negative) ? -number : number;
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			double toDouble() const
			{
				valueT integerPart = value >> FRACTIONAL_BITS;
				valueT fractionalPart = value & FRACTIONAL_MASK;
				double number = (double)integerPart + (double)fractionalPart / (double)FACTOR;
				return number;
			}

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			double toFloat() const
			{
				bool sign_negative = (value & StoreSpec::SIGN_MASK) != 0;
				u_valueT uvalue = (sign_negative) ? -value : value;
				u_valueT integerPart = uvalue >> FRACTIONAL_BITS;
				u_valueT fractionalPart = uvalue & FRACTIONAL_MASK;
				float number = (float)integerPart + (float)fractionalPart / (float)FACTOR;
				return (sign_negative) ? -number : number;
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			double toFloat() const
			{
				valueT integerPart = value >> FRACTIONAL_BITS;
				valueT fractionalPart = value & FRACTIONAL_MASK;
				float number = (float)integerPart + (float)fractionalPart / (float)FACTOR;
				return number;
			}

			// std::string toString() const
			// {
			// 	return ITKCommon::PrintfToStdString("%f", toDouble());
			// }

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			valueT signed_integer_part() const
			{
				return (value & StoreSpec::SIGN_MASK) ? (-valueT(-value >> FRACTIONAL_BITS)) : (value >> FRACTIONAL_BITS);
			}
			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			u_valueT integer_part() const
			{
				return (value & StoreSpec::SIGN_MASK) ? ((-value >> FRACTIONAL_BITS)) : (value >> FRACTIONAL_BITS);
			}
			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			valueT signed_fract_part() const
			{
				return (value & StoreSpec::SIGN_MASK) ? (-valueT(-value & FRACTIONAL_MASK)) : (value & FRACTIONAL_MASK);
			}
			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			u_valueT fract_part() const
			{
				return (value & StoreSpec::SIGN_MASK) ? ((-value & FRACTIONAL_MASK)) : (value & FRACTIONAL_MASK);
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			valueT signed_integer_part() const
			{
				return value >> FRACTIONAL_BITS;
			}
			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			valueT integer_part() const
			{
				return value >> FRACTIONAL_BITS;
			}
			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			valueT signed_fract_part() const
			{
				return value & FRACTIONAL_MASK;
			}
			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			valueT fract_part() const
			{
				return value & FRACTIONAL_MASK;
			}

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			valueT sign() const
			{
				return ((value >> StoreSpec::SHIFT_GET_SIGN_MINUS_ONE) & StoreSpec::FIRST_BIT_ZERO) + 1;
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			valueT sign() const
			{
				return 1;
			}

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			bool isNegative() const
			{
				return (value & StoreSpec::SIGN_MASK) != 0;
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			bool isNegative() const
			{
				return 0;
			}

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			static constexpr fixed_t fromFloat(float v) noexcept
			{
				bool sign_negative = v < 0;
				v = (sign_negative) ? -v : v;
				u_valueT integerPart = (u_valueT)v;
				u_valueT fractionalPart = (u_valueT)((v - (float)integerPart) * (float)FACTOR);
				return (sign_negative) ? -fixed_t(integerPart, fractionalPart) : fixed_t(integerPart, fractionalPart);
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			static constexpr fixed_t fromFloat(float v) noexcept
			{
				valueT integerPart = (valueT)v;
				valueT fractionalPart = (valueT)((v - (float)integerPart) * (float)FACTOR);
				return fixed_t(integerPart, fractionalPart);
			}

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			static constexpr fixed_t fromDouble(double v) noexcept
			{
				bool sign_negative = v < 0;
				v = (sign_negative) ? -v : v;
				u_valueT integerPart = (u_valueT)v;
				u_valueT fractionalPart = (u_valueT)((v - (double)integerPart) * (double)FACTOR);
				return (sign_negative) ? -fixed_t(integerPart, fractionalPart) : fixed_t(integerPart, fractionalPart);
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			static constexpr fixed_t fromDouble(double v) noexcept
			{
				valueT integerPart = (valueT)v;
				valueT fractionalPart = (valueT)((v - (double)integerPart) * (double)FACTOR);
				return fixed_t(integerPart, fractionalPart);
			}

			inline operator valueT() const
			{
				return signed_integer_part();
			}
		};

		//
		// Arithmetic Block
		//

		template <typename store_type, int frac_bits>
		static inline fixed_t<store_type, frac_bits> operator/(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vecA) /= vecB);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, store_type>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator/(const fixed_t<store_type, frac_bits> &vec, const _InputType &value) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vec) /= (store_type)value);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, store_type>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator/(const _InputType &value, const fixed_t<store_type, frac_bits> &vec) noexcept
		{
			return (fixed_t<store_type, frac_bits>((store_type)value) /= vec);
		}
		template <typename store_type, int frac_bits>
		static inline fixed_t<store_type, frac_bits> operator*(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vecA) *= vecB);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, store_type>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator*(const fixed_t<store_type, frac_bits> &vec, const _InputType &value) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vec) *= (store_type)value);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, store_type>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator*(const _InputType &value, const fixed_t<store_type, frac_bits> &vec) noexcept
		{
			return (fixed_t<store_type, frac_bits>((store_type)value) *= vec);
		}
		template <typename store_type, int frac_bits>
		static inline fixed_t<store_type, frac_bits> operator+(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vecA) += vecB);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, store_type>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator+(const fixed_t<store_type, frac_bits> &vec, const _InputType &value) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vec) += (store_type)value);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, store_type>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator+(const _InputType &value, const fixed_t<store_type, frac_bits> &vec) noexcept
		{
			return (fixed_t<store_type, frac_bits>((store_type)value) += vec);
		}
		template <typename store_type, int frac_bits>
		static inline fixed_t<store_type, frac_bits> operator-(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vecA) -= vecB);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, store_type>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator-(const fixed_t<store_type, frac_bits> &vec, const _InputType &value) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vec) -= (store_type)value);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, store_type>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator-(const _InputType &value, const fixed_t<store_type, frac_bits> &vec) noexcept
		{
			return (fixed_t<store_type, frac_bits>((store_type)value) -= vec);
		}

		//
		// Bitwise Block
		//
		template <typename store_type, int frac_bits>
		static inline fixed_t<store_type, frac_bits> operator&(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vecA) &= vecB);
		}
		template <typename store_type, int frac_bits>
		static inline fixed_t<store_type, frac_bits> operator|(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vecA) |= vecB);
		}
		template <typename store_type, int frac_bits>
		static inline fixed_t<store_type, frac_bits> operator^(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vecA) ^= vecB);
		}

		//
		// Shift block
		//
		template <typename store_type, int frac_bits>
		static inline fixed_t<store_type, frac_bits> operator<<(const fixed_t<store_type, frac_bits> &vecA, int shift) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vecA) <<= shift);
		}
		template <typename store_type, int frac_bits>
		static inline fixed_t<store_type, frac_bits> operator>>(const fixed_t<store_type, frac_bits> &vecA, int shift) noexcept
		{
			return (fixed_t<store_type, frac_bits>(vecA) >>= shift);
		}

	}

	template <int frac_bits>
	using ufixed32_t = FixedPoint::fixed_t<uint32_t, frac_bits>;

	template <int frac_bits>
	using fixed32_t = FixedPoint::fixed_t<int32_t, frac_bits>;

	template <int frac_bits>
	using ufixed64_t = FixedPoint::fixed_t<uint64_t, frac_bits>;

	template <int frac_bits>
	using fixed64_t = FixedPoint::fixed_t<int64_t, frac_bits>;

}