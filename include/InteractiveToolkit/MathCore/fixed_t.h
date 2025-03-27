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
			typedef int32_t i_valueT;

			typedef uint64_t sizeT;
			typedef int64_t rangeT;
			typedef int64_t mul_div_T;
			typedef uint64_t u_mul_div_T;

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
			typedef int32_t i_valueT;

			typedef uint64_t sizeT;
			typedef int64_t rangeT;
			typedef int64_t mul_div_T;
			typedef uint64_t u_mul_div_T;

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
			typedef int64_t i_valueT;

			// the best int type for size and range is the int128
			// but int64 is best supported by compilers in general.
			// This implementation is guaranteeded to be working
			// with the following ranges for the fractional part:
			//     - min of 2 bits
			//     - max of 62 bits
			typedef uint64_t sizeT;
			typedef int64_t rangeT;
			typedef uint128 mul_div_T;
			typedef uint128 u_mul_div_T;

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
			typedef int64_t i_valueT;

			// the best int type for size and range is the int128
			// but int64 is best supported by compilers in general.
			// This implementation is guaranteeded to be working
			// with the following ranges for the fractional part:
			//     - min of 2 bits
			//     - max of 62 bits
			typedef uint64_t sizeT;
			typedef int64_t rangeT;
			typedef uint128 mul_div_T;
			typedef uint128 u_mul_div_T;

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
			typedef typename StoreSpec::i_valueT i_valueT;
			typedef typename StoreSpec::sizeT sizeT;
			typedef typename StoreSpec::rangeT rangeT;
			typedef typename StoreSpec::mul_div_T mul_div_T;
			typedef typename StoreSpec::u_mul_div_T u_mul_div_T;

		private:
			static constexpr int FRACTIONAL_BITS = frac_bits;
			static constexpr sizeT FRACTIONAL_MASK = (sizeT(1) << FRACTIONAL_BITS) - sizeT(1);
			static constexpr sizeT FACTOR = sizeT(1) << FRACTIONAL_BITS;
			static constexpr float _1_FACTOR_f = 1.0f / (float)FACTOR;
			static constexpr double _1_FACTOR_d = 1.0 / (double)FACTOR;

		public:
			valueT value;

			static constexpr int INT_BITS = StoreSpec::VALUE_TOTAL_BITS - FRACTIONAL_BITS;
			static constexpr rangeT INT_RANGE_MIN = StoreSpec::INTEGER_PART_MIN_RANGE(INT_BITS);
			static constexpr rangeT INT_RANGE_MAX = StoreSpec::INTEGER_PART_MAX_RANGE(INT_BITS);

			static constexpr int FRAC_BITS = FRACTIONAL_BITS;
			static constexpr rangeT FRAC_RANGE_MIN = StoreSpec::FRACTIONAL_PART_MIN_RANGE(FRAC_BITS);
			static constexpr rangeT FRAC_RANGE_MAX = StoreSpec::FRACTIONAL_PART_MAX_RANGE(FRAC_BITS);

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			inline constexpr fixed_t(const valueT &integerPart, const valueT &fractionalPart) noexcept : value(valueT(u_valueT(integerPart) << FRACTIONAL_BITS) + fractionalPart)
			{
				// value = (integerPart << FRACTIONAL_BITS) + fractionalPart;
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			inline constexpr fixed_t(const valueT &integerPart, const valueT &fractionalPart) noexcept : value((integerPart << FRACTIONAL_BITS) | (fractionalPart & FRACTIONAL_MASK))
			{
				// value = (integerPart << FRACTIONAL_BITS) | (fractionalPart & FRACTIONAL_MASK);
			}

			inline constexpr fixed_t() noexcept : value(0)
			{
				// value = 0;
			}

			template <typename _valueT,
					  typename std::enable_if<
						  std::is_integral<_valueT>::value,
						  bool>::type = true>
			inline constexpr fixed_t(const _valueT &integerPart) noexcept : value((u_valueT(valueT(integerPart)) << FRACTIONAL_BITS))
			{
				// value = ((valueT)integerPart << FRACTIONAL_BITS);
			}

			inline constexpr fixed_t(const fixed_t &rawValue) noexcept : value(rawValue.value)
			{
				// value = rawValue.value;
			}

			inline void operator=(const fixed_t &rawValue) noexcept
			{
				value = rawValue.value;
			}

			inline constexpr fixed_t(const float &v) noexcept : value(fixed_t::fromFloat(v).value)
			{
				// *this = fixed_t::fromFloat(v);
			}

			inline constexpr fixed_t(const double &v) noexcept : value(fixed_t::fromDouble(v).value)
			{
				// *this = fixed_t::fromDouble(v);
			}

		private:
			inline constexpr fixed_t(const valueT &rawValue, bool is_raw_unused_1, bool is_raw_unused_2) noexcept : value(rawValue)
			{
				// value = 0;
			}

		public:
			static constexpr fixed_t fromRaw(const valueT &rawValue) noexcept
			{
				// fixed_t result;
				// result.value = rawValue;
				// return result;
				return fixed_t(rawValue, true, true);
			}

			//
			// Arithmetic block
			//

			// inline constexpr fixed_t operator+(const fixed_t& other)
			//{
			//	return fixed_t::fromRaw(value + other.value);
			// }

			inline fixed_t &operator+=(const fixed_t &other) noexcept
			{
				value += other.value;
				return *this;
			}

			// inline constexpr fixed_t operator-(const fixed_t& other)
			//{
			//	return fixed_t::fromRaw(value - other.value);
			// }

			inline fixed_t &operator-=(const fixed_t &other) noexcept
			{
				value -= other.value;
				return *this;
			}

			inline constexpr fixed_t operator-() const noexcept
			{
				return fixed_t::fromRaw(valueT(-i_valueT(value)));
			}

			// special case for int64_t...
			// do the multiplication using uint128,
			// but considering the signal...
			// template <typename T = StoreSpec, typename std::enable_if<std::is_same<typename T::valueT, int64_t>::value, bool>::type = true>
			// inline constexpr fixed_t operator_mul(const fixed_t &other) const noexcept
			// {
			// 	// bool value_neg = (value & StoreSpec::SIGN_MASK) != 0;
			// 	// bool other_neg = (other.value & StoreSpec::SIGN_MASK) != 0;

			// 	// mul_div_T result = mul_div_T((value_neg) ? -value : value);
			// 	// result *= mul_div_T((other_neg) ? -other.value : other.value);
			// 	// result >>= FRACTIONAL_BITS;

			// 	// bool neg_result = value_neg ^ other_neg;

			// 	// value = (neg_result) ? -(valueT)result : (valueT)result;
			// 	// return *this;

			// 	return ((value < 0) ^ (other.value < 0)) ? -fixed_t::fromRaw((valueT)((mul_div_T((value < 0) ? -value : value) * mul_div_T((other.value < 0) ? -other.value : other.value)) >> FRACTIONAL_BITS))
			// 											 : fixed_t::fromRaw((valueT)((mul_div_T((value < 0) ? -value : value) * mul_div_T((other.value < 0) ? -other.value : other.value)) >> FRACTIONAL_BITS));
			// }
			template <typename T = StoreSpec, typename std::enable_if<std::is_same<typename T::valueT, int64_t>::value, bool>::type = true>
			inline fixed_t &operator*=(const fixed_t &other) noexcept
			{
				// bool value_neg = (value & StoreSpec::SIGN_MASK) != 0;
				// bool other_neg = (other.value & StoreSpec::SIGN_MASK) != 0;

				// mul_div_T result = mul_div_T((value_neg) ? -value : value);
				// result *= mul_div_T((other_neg) ? -other.value : other.value);
				// result >>= FRACTIONAL_BITS;

				// bool neg_result = value_neg ^ other_neg;

				// value = (neg_result) ? -(valueT)result : (valueT)result;

				uint64_t r_high, r_low;
				MathCore::multiply_int64_to_int128(value, other.value, &r_high, &r_low);

				// if (FRACTIONAL_BITS >= 64)
				// 	value = (r_high >> (FRACTIONAL_BITS - 64));
				// else
				value = static_cast<int64_t>((r_low >> FRACTIONAL_BITS) | ((r_high << (64 - FRACTIONAL_BITS)))); // & -(FRACTIONAL_BITS != 0);

				return *this;
			}

			template <typename T = StoreSpec, typename std::enable_if<std::is_same<typename T::valueT, uint64_t>::value, bool>::type = true>
			inline fixed_t &operator*=(const fixed_t &other) noexcept
			{
				uint64_t r_high, r_low;
				MathCore::multiply_uint64_to_uint128(value, other.value, &r_high, &r_low);

				// if (FRACTIONAL_BITS >= 64)
				// 	value = (r_high >> (FRACTIONAL_BITS - 64));
				// else
				value = static_cast<uint64_t>((r_low >> FRACTIONAL_BITS) | ((r_high << (64 - FRACTIONAL_BITS)))); // & -(FRACTIONAL_BITS != 0);

				return *this;
			}

			// template <typename T = StoreSpec, typename std::enable_if<!std::is_same<typename T::valueT, int64_t>::value, bool>::type = true>
			// inline constexpr fixed_t operator_mul(const fixed_t &other) const noexcept
			// {
			// 	// mul_div_T result = mul_div_T(value);
			// 	// result *= mul_div_T(other.value);
			// 	// result >>= FRACTIONAL_BITS;
			// 	// value = (valueT)result;
			// 	return fixed_t::fromRaw((valueT)((mul_div_T(value) * mul_div_T(other.value)) >> FRACTIONAL_BITS));
			// }

			template <typename T = StoreSpec, typename std::enable_if<!std::is_same<typename T::valueT, int64_t>::value &&
																		  !std::is_same<typename T::valueT, uint64_t>::value,
																	  bool>::type = true>
			inline fixed_t &operator*=(const fixed_t &other) noexcept
			{
				mul_div_T result = mul_div_T(value) * mul_div_T(other.value);
				result >>= FRACTIONAL_BITS;
				value = (valueT)result;
				return *this;
			}

			// special case for int64_t...
			// do the multiplication using uint128,
			// but considering the signal...

			// template <typename T = StoreSpec, typename std::enable_if<std::is_same<typename T::valueT, int64_t>::value, bool>::type = true>
			// inline constexpr fixed_t operator_div(const fixed_t &other) const noexcept
			// {
			// 	// bool value_neg = (value & StoreSpec::SIGN_MASK) != 0;
			// 	// bool other_neg = (other.value & StoreSpec::SIGN_MASK) != 0;

			// 	// mul_div_T result = mul_div_T((value_neg) ? -value : value);
			// 	// result <<= FRACTIONAL_BITS;
			// 	// result /= mul_div_T((other_neg) ? -other.value : other.value);

			// 	// bool neg_result = value_neg ^ other_neg;

			// 	// value = (neg_result) ? -(valueT)result : (valueT)result;
			// 	// return *this;

			// 	return ((value < 0) ^ (other.value < 0)) ? -fixed_t::fromRaw((valueT)(mul_div_T(u_mul_div_T(mul_div_T((value < 0) ? -value : value)) << FRACTIONAL_BITS) / mul_div_T((other.value < 0) ? -other.value : other.value)))
			// 											 : fixed_t::fromRaw((valueT)(mul_div_T(u_mul_div_T(mul_div_T((value < 0) ? -value : value)) << FRACTIONAL_BITS) / mul_div_T((other.value < 0) ? -other.value : other.value)));
			// }

			template <typename T = StoreSpec, typename std::enable_if<std::is_same<typename T::valueT, int64_t>::value, bool>::type = true>
			inline fixed_t &operator/=(const fixed_t &other) noexcept
			{
				// bool value_neg = (value & StoreSpec::SIGN_MASK) != 0;
				// bool other_neg = (other.value & StoreSpec::SIGN_MASK) != 0;

				// mul_div_T result = mul_div_T((value_neg) ? -value : value);
				// result <<= FRACTIONAL_BITS;
				// result /= mul_div_T((other_neg) ? -other.value : other.value);

				// bool neg_result = value_neg ^ other_neg;

				// value = (neg_result) ? -(valueT)result : (valueT)result;

				uint64_t r_high, r_low;

				r_high = value >> (64 - FRACTIONAL_BITS);
				r_low = value << FRACTIONAL_BITS;

				value = MathCore::divide_int128_by_int64(r_high, r_low, other.value);

				return *this;
			}

			// template <typename T = StoreSpec, typename std::enable_if<!std::is_same<typename T::valueT, int64_t>::value, bool>::type = true>
			// inline constexpr fixed_t operator_div(const fixed_t &other) const noexcept
			// {
			// 	// mul_div_T result = mul_div_T(value);
			// 	// result <<= FRACTIONAL_BITS;
			// 	// result /= mul_div_T(other.value);
			// 	// value = (valueT)result;
			// 	// return *this;

			// 	return fixed_t::fromRaw((valueT)(mul_div_T(u_mul_div_T(mul_div_T(value)) << FRACTIONAL_BITS) / mul_div_T(other.value)));
			// }

			template <typename T = StoreSpec, typename std::enable_if<std::is_same<typename T::valueT, uint64_t>::value,
												  bool>::type = true>
			inline fixed_t &operator/=(const fixed_t &other) noexcept
			{

				// mul_div_T result = mul_div_T(value);
				// result <<= FRACTIONAL_BITS;
				// result /= mul_div_T(other.value);
				// value = (valueT)result;

				uint64_t r_high, r_low;

				r_high = value >> (64 - FRACTIONAL_BITS);
				r_low = value << FRACTIONAL_BITS;

				value = MathCore::divide_uint128_by_uint64(r_high, r_low, other.value);

				return *this;
			}

			template <typename T = StoreSpec, typename std::enable_if<
												  !std::is_same<typename T::valueT, int64_t>::value &&
													  !std::is_same<typename T::valueT, uint64_t>::value,
												  bool>::type = true>
			inline fixed_t &operator/=(const fixed_t &other) noexcept
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

			// inline constexpr fixed_t operator&(const fixed_t& other)
			//{
			//	return fixed_t::fromRaw(value & other.value);
			// }

			inline fixed_t &operator&=(const fixed_t &other) noexcept
			{
				value &= other.value;
				return *this;
			}

			// inline constexpr fixed_t operator|(const fixed_t& other)
			//{
			//	return fixed_t::fromRaw(value | other.value);
			// }

			inline fixed_t &operator|=(const fixed_t &other) noexcept
			{
				value |= other.value;
				return *this;
			}

			// inline constexpr fixed_t operator^(const fixed_t& other)
			//{
			//	return fixed_t::fromRaw(value ^ other.value);
			// }

			inline fixed_t &operator^=(const fixed_t &other) noexcept
			{
				value ^= other.value;
				return *this;
			}
			inline constexpr fixed_t operator~() const noexcept
			{
				return fixed_t::fromRaw(~value);
			}

			//
			// Shift block
			//

			inline constexpr fixed_t operator_shift_left(int shift) const noexcept
			{
				return fixed_t::fromRaw(valueT(u_valueT(value) << shift));
			}

			inline fixed_t &operator<<=(int shift)
			{
				value <<= shift;
				return *this;
			}

			inline constexpr fixed_t operator_shift_right(int shift) const noexcept
			{
				return fixed_t::fromRaw(value >> shift);
			}

			inline fixed_t &operator>>=(int shift) noexcept
			{
				value >>= shift;
				return *this;
			}

			//
			// Comparison block
			//
			inline constexpr bool operator>(const fixed_t &other) const noexcept { return value > other.value; }
			inline constexpr bool operator>=(const fixed_t &other) const noexcept { return value >= other.value; }
			inline constexpr bool operator<(const fixed_t &other) const noexcept { return value < other.value; }
			inline constexpr bool operator<=(const fixed_t &other) const noexcept { return value <= other.value; }
			inline constexpr bool operator==(const fixed_t &other) const noexcept { return value == other.value; }
			inline constexpr bool operator!=(const fixed_t &other) const noexcept { return value != other.value; }

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			constexpr double toDouble() const noexcept
			{
				// bool sign_negative = (value & StoreSpec::SIGN_MASK) != 0;
				// u_valueT uvalue = (sign_negative) ? -value : value;
				// u_valueT integerPart = uvalue >> FRACTIONAL_BITS;
				// u_valueT fractionalPart = uvalue & FRACTIONAL_MASK;
				// // double number = (double)integerPart + (double)fractionalPart / (double)FACTOR;
				// double number = (double)integerPart + (double)fractionalPart * _1_FACTOR_d;
				// return (sign_negative) ? -number : number;

				return (value < 0) ? -((double)((-value) >> FRACTIONAL_BITS) + (double)((-value) & FRACTIONAL_MASK) * _1_FACTOR_d)
								   : ((double)(value >> FRACTIONAL_BITS) + (double)(value & FRACTIONAL_MASK) * _1_FACTOR_d);
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			constexpr double toDouble() const noexcept
			{
				// valueT integerPart = value >> FRACTIONAL_BITS;
				// valueT fractionalPart = value & FRACTIONAL_MASK;
				// // double number = (double)integerPart + (double)fractionalPart / (double)FACTOR;
				// double number = (double)integerPart + (double)fractionalPart * _1_FACTOR_d;
				// return number;
				return (double)(value >> FRACTIONAL_BITS) + (double)(value & FRACTIONAL_MASK) * _1_FACTOR_d;
			}

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			constexpr float toFloat() const noexcept
			{
				// bool sign_negative = (value & StoreSpec::SIGN_MASK) != 0;
				// u_valueT uvalue = (sign_negative) ? -value : value;
				// u_valueT integerPart = uvalue >> FRACTIONAL_BITS;
				// u_valueT fractionalPart = uvalue & FRACTIONAL_MASK;
				// // float number = (float)integerPart + (float)fractionalPart / (float)FACTOR;
				// float number = (float)integerPart + (float)fractionalPart * _1_FACTOR_f;
				// return (sign_negative) ? -number : number;

				return (value < 0) ? -((float)((-value) >> FRACTIONAL_BITS) + (float)((-value) & FRACTIONAL_MASK) * _1_FACTOR_f)
								   : ((float)(value >> FRACTIONAL_BITS) + (float)(value & FRACTIONAL_MASK) * _1_FACTOR_f);
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			constexpr float toFloat() const noexcept
			{
				// valueT integerPart = value >> FRACTIONAL_BITS;
				// valueT fractionalPart = value & FRACTIONAL_MASK;
				// // float number = (float)integerPart + (float)fractionalPart / (float)FACTOR;
				// float number = (float)integerPart + (float)fractionalPart * _1_FACTOR_f;
				// return number;

				return (float)(value >> FRACTIONAL_BITS) + (float)(value & FRACTIONAL_MASK) * _1_FACTOR_f;
			}

			// std::string toString() const
			// {
			// 	return ITKCommon::PrintfToStdString("%f", toDouble());
			// }

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			constexpr valueT signed_integer_part() const noexcept
			{
				return (value & StoreSpec::SIGN_MASK) ? (-valueT(-value >> FRACTIONAL_BITS))
													  : (value >> FRACTIONAL_BITS);
			}
			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			constexpr u_valueT integer_part() const noexcept
			{
				return (value & StoreSpec::SIGN_MASK) ? ((-value >> FRACTIONAL_BITS))
													  : (value >> FRACTIONAL_BITS);
			}
			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			constexpr valueT signed_fract_part() const noexcept
			{
				return (value & StoreSpec::SIGN_MASK) ? (-valueT(-value & FRACTIONAL_MASK))
													  : (value & FRACTIONAL_MASK);
			}
			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			constexpr u_valueT fract_part() const noexcept
			{
				return (value & StoreSpec::SIGN_MASK) ? ((-value & FRACTIONAL_MASK))
													  : (value & FRACTIONAL_MASK);
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			constexpr valueT signed_integer_part() const noexcept
			{
				return value >> FRACTIONAL_BITS;
			}
			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			constexpr valueT integer_part() const noexcept
			{
				return value >> FRACTIONAL_BITS;
			}
			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			constexpr valueT signed_fract_part() const noexcept
			{
				return value & FRACTIONAL_MASK;
			}
			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			constexpr valueT fract_part() const noexcept
			{
				return value & FRACTIONAL_MASK;
			}

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			constexpr valueT sign() const noexcept
			{
				return ((value >> StoreSpec::SHIFT_GET_SIGN_MINUS_ONE) & StoreSpec::FIRST_BIT_ZERO) + 1;
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			constexpr valueT sign() const noexcept
			{
				return 1;
			}

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			constexpr bool isNegative() const noexcept
			{
				return (value & StoreSpec::SIGN_MASK) != 0;
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			constexpr bool isNegative() const noexcept
			{
				return 0;
			}

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			static constexpr fixed_t fromFloat(float v) noexcept
			{
				// bool sign_negative = v < 0;
				// v = (sign_negative) ? -v : v;
				// u_valueT integerPart = (u_valueT)v;
				// u_valueT fractionalPart = (u_valueT)((v - (float)integerPart) * (float)FACTOR);
				// return (sign_negative) ? -fixed_t(integerPart, fractionalPart) : fixed_t(integerPart, fractionalPart);

				return (v < 0) ? -fixed_t((valueT)-v, (valueT)((-v - (float)(valueT)-v) * (float)FACTOR))
							   : fixed_t((valueT)v, (valueT)((v - (float)(valueT)v) * (float)FACTOR));
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			static constexpr fixed_t fromFloat(float v) noexcept
			{
				// valueT integerPart = (valueT)v;
				// valueT fractionalPart = (valueT)((v - (float)integerPart) * (float)FACTOR);
				// return fixed_t(integerPart, fractionalPart);

				return (v < 0) ? -fixed_t((valueT)-v, (valueT)((-v - (float)(valueT)-v) * (float)FACTOR))
							   : fixed_t((valueT)v, (valueT)((v - (float)(valueT)v) * (float)FACTOR));

				// return fixed_t((valueT)v, (valueT)((v - (float)(valueT)v) * (float)FACTOR));
			}

			template <typename T = StoreSpec, typename std::enable_if<T::_is_signed::value, bool>::type = true>
			static constexpr fixed_t fromDouble(double v) noexcept
			{
				// uint64_t &v_int = &v;

				// bool sign_negative = v < 0;
				// v = (sign_negative) ? -v : v;
				// u_valueT integerPart = (u_valueT)v;
				// u_valueT fractionalPart = (u_valueT)((v - (double)integerPart) * (double)FACTOR);
				// return (sign_negative) ? -fixed_t(integerPart, fractionalPart) : fixed_t(integerPart, fractionalPart);

				return (v < 0) ? -fixed_t((valueT)-v, (valueT)((-v - (double)(valueT)-v) * (double)FACTOR))
							   : fixed_t((valueT)v, (valueT)((v - (double)(valueT)v) * (double)FACTOR));
			}

			template <typename T = StoreSpec, typename std::enable_if<!T::_is_signed::value, bool>::type = true>
			static constexpr fixed_t fromDouble(double v) noexcept
			{
				// valueT integerPart = (valueT)v;
				// valueT fractionalPart = (valueT)((v - (double)integerPart) * (double)FACTOR);
				// return fixed_t(integerPart, fractionalPart);

				return (v < 0) ? -fixed_t((valueT)-v, (valueT)((-v - (double)(valueT)-v) * (double)FACTOR))
							   : fixed_t((valueT)v, (valueT)((v - (double)(valueT)v) * (double)FACTOR));

				// return fixed_t((valueT)v, (valueT)((v - (double)(valueT)v) * (double)FACTOR));
			}

			template <typename _valueT,
					  typename std::enable_if<
						  std::is_integral<_valueT>::value,
						  bool>::type = true>
			inline constexpr operator _valueT() const noexcept
			{
				return (_valueT)signed_integer_part();
			}

			inline constexpr operator float() const noexcept
			{
				return this->toFloat();
			}

			inline constexpr operator double() const noexcept
			{
				return this->toDouble();
			}

			template <typename _store_type, int _frac_bits,
					  typename _store_type_b = store_type, int _frac_bits_b = frac_bits,
					  typename std::enable_if<
						  std::is_signed<_store_type>::value &&
							  std::is_convertible<_store_type, _store_type_b>::value &&
							  (_frac_bits > _frac_bits_b),
						  bool>::type = true>
			inline void operator=(const fixed_t<_store_type, _frac_bits> &vec) noexcept
			{
				this->value = (_store_type_b)(vec.value >> (_frac_bits - _frac_bits_b));
				// this->value >>= _frac_bits - _frac_bits_b;
			}
			template <typename _store_type, int _frac_bits,
					  typename _store_type_b = store_type, int _frac_bits_b = frac_bits,
					  typename std::enable_if<
						  !std::is_signed<_store_type>::value &&
							  std::is_convertible<_store_type, _store_type_b>::value &&
							  (_frac_bits > _frac_bits_b),
						  bool>::type = true>
			inline void operator=(const fixed_t<_store_type, _frac_bits> &vec) noexcept
			{
				this->value = (_store_type_b)vec.value;
				this->value >>= (_frac_bits - _frac_bits_b);
			}
			template <typename _store_type, int _frac_bits,
					  typename _store_type_b = store_type, int _frac_bits_b = frac_bits,
					  typename std::enable_if<
						  // std::is_signed<_store_type>::value&&
						  std::is_convertible<_store_type, _store_type_b>::value &&
							  (_frac_bits < _frac_bits_b),
						  bool>::type = true>
			inline void operator=(const fixed_t<_store_type, _frac_bits> &vec) noexcept
			{
				this->value = (_store_type_b)(vec.value << (_frac_bits_b - _frac_bits));
				// this->value <<= _frac_bits_b - _frac_bits;
			}
			// template <typename _store_type, int _frac_bits,
			//	typename _store_type_b = store_type, int _frac_bits_b = frac_bits,
			//	typename std::enable_if<
			//	!std::is_signed<_store_type>::value&&
			//	std::is_convertible<_store_type, _store_type_b>::value &&
			//	(_frac_bits < _frac_bits_b),
			//	bool>::type = true>
			//	inline void operator=(const fixed_t<_store_type, _frac_bits>& vec) noexcept
			//{
			//	this->value = (_store_type_b)vec.value;
			//	this->value <<= (_frac_bits_b - _frac_bits);
			// }
			template <typename _store_type, int _frac_bits,
					  typename _store_type_b = store_type, int _frac_bits_b = frac_bits,
					  typename std::enable_if<
						  std::is_convertible<_store_type, _store_type_b>::value &&
							  !std::is_same<_store_type, _store_type_b>::value &&
							  (_frac_bits == _frac_bits_b),
						  bool>::type = true>
			inline void operator=(const fixed_t<_store_type, _frac_bits> &vec) noexcept
			{
				this->value = (_store_type_b)vec.value;
			}

			// template <typename _store_type, int _frac_bits,
			//	typename _store_type_b = store_type, int _frac_bits_b = frac_bits,
			//	typename std::enable_if<
			//	std::is_convertible<fixed_t<_store_type, _frac_bits>, fixed_t<_store_type_b, _frac_bits_b>>::value,
			//	bool>::type = true>
			// inline fixed_t(const fixed_t<_store_type, _frac_bits>& vec) noexcept
			//{
			//	*this = vec;
			// }

			template <typename _store_type, int _frac_bits,
					  typename _store_type_b = store_type, int _frac_bits_b = frac_bits,
					  typename std::enable_if<
						  std::is_signed<_store_type>::value &&
							  std::is_convertible<_store_type, _store_type_b>::value &&
							  (_frac_bits > _frac_bits_b),
						  bool>::type = true>
			inline constexpr fixed_t(const fixed_t<_store_type, _frac_bits> &vec) noexcept : value((_store_type_b)(vec.value >> (_frac_bits - _frac_bits_b)))
			{
				// this->value = (_store_type_b)(vec.value >> (_frac_bits - _frac_bits_b));
				//  this->value >>= _frac_bits - _frac_bits_b;
			}
			template <typename _store_type, int _frac_bits,
					  typename _store_type_b = store_type, int _frac_bits_b = frac_bits,
					  typename std::enable_if<
						  !std::is_signed<_store_type>::value &&
							  std::is_convertible<_store_type, _store_type_b>::value &&
							  (_frac_bits > _frac_bits_b),
						  bool>::type = true>
			inline constexpr fixed_t(const fixed_t<_store_type, _frac_bits> &vec) noexcept : value(_store_type_b(vec.value) >> (_frac_bits - _frac_bits_b))
			{
				// this->value = (_store_type_b)vec.value;
				// this->value >>= (_frac_bits - _frac_bits_b);
			}
			template <typename _store_type, int _frac_bits,
					  typename _store_type_b = store_type, int _frac_bits_b = frac_bits,
					  typename _u_store_type_b = typename StoreSpecType<store_type>::u_valueT,
					  typename std::enable_if<
						  std::is_convertible<_store_type, _store_type_b>::value &&
							  (_frac_bits < _frac_bits_b),
						  bool>::type = true>
			inline constexpr fixed_t(const fixed_t<_store_type, _frac_bits> &vec) noexcept : value(_store_type_b(_u_store_type_b(vec.value) << (_frac_bits_b - _frac_bits)))
			{
				// this->value = (_store_type_b)(vec.value << (_frac_bits_b - _frac_bits));
				// this->value <<= _frac_bits_b - _frac_bits;
			}

			template <typename _store_type, int _frac_bits,
					  typename _store_type_b = store_type, int _frac_bits_b = frac_bits,
					  typename std::enable_if<
						  std::is_convertible<_store_type, _store_type_b>::value &&
							  !std::is_same<_store_type, _store_type_b>::value &&
							  (_frac_bits == _frac_bits_b),
						  bool>::type = true>
			inline constexpr fixed_t(const fixed_t<_store_type, _frac_bits> &vec) noexcept : value(_store_type_b(vec.value))
			{
				// this->value = (_store_type_b)vec.value;
			}

			template <typename _store_type, int _frac_bits,
					  typename _store_type_b = store_type, int _frac_bits_b = frac_bits,
					  typename std::enable_if<
						  !std::is_same<_store_type_b, _store_type>::value ||
							  (_frac_bits != _frac_bits_b),
						  bool>::type = true>
			inline operator fixed_t<_store_type, _frac_bits>() const noexcept
			{
				fixed_t<_store_type, _frac_bits> result;
				result = *this;
				return result;
			}
		};

		//
		// Arithmetic Block
		//

		template <typename store_type, int frac_bits>
		static inline fixed_t<store_type, frac_bits> operator/(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(vecA) / vecB);
			// return vecA.operator_div(vecB);
			return fixed_t<store_type, frac_bits>(vecA) /= vecB;
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, fixed_t<store_type, frac_bits>>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator/(const fixed_t<store_type, frac_bits> &vec, const _InputType &value) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(vec) / (fixed_t<store_type, frac_bits>)value);
			// return vec.operator_div(fixed_t<store_type, frac_bits>(value));
			return fixed_t<store_type, frac_bits>(vec) /= fixed_t<store_type, frac_bits>(value);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, fixed_t<store_type, frac_bits>>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator/(const _InputType &value, const fixed_t<store_type, frac_bits> &vec) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(value) / vec);
			// return fixed_t<store_type, frac_bits>(value).operator_div(vec);
			return fixed_t<store_type, frac_bits>(value) /= vec;
		}
		template <typename store_type, int frac_bits>
		static inline fixed_t<store_type, frac_bits> operator*(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(vecA) * vecB);
			//return vecA.operator_mul(vecB);
			return fixed_t<store_type, frac_bits>(vecA) *= vecB;
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, fixed_t<store_type, frac_bits>>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator*(const fixed_t<store_type, frac_bits> &vec, const _InputType &value) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(vec) * (fixed_t<store_type, frac_bits>)value);
			// return vec.operator_mul(fixed_t<store_type, frac_bits>(value));
			return fixed_t<store_type, frac_bits>(vec) *= fixed_t<store_type, frac_bits>(value);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, fixed_t<store_type, frac_bits>>::value,
					  bool>::type = true>
		static inline fixed_t<store_type, frac_bits> operator*(const _InputType &value, const fixed_t<store_type, frac_bits> &vec) noexcept
		{
			// return ((fixed_t<store_type, frac_bits>)value * vec);
			//return fixed_t<store_type, frac_bits>(value).operator_mul(vec);
			return fixed_t<store_type, frac_bits>(value) *= vec;
		}
		template <typename store_type, int frac_bits>
		static inline constexpr fixed_t<store_type, frac_bits> operator+(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			// return vecA + vecB;
			return fixed_t<store_type, frac_bits>::fromRaw(vecA.value + vecB.value);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, fixed_t<store_type, frac_bits>>::value,
					  bool>::type = true>
		static inline constexpr fixed_t<store_type, frac_bits> operator+(const fixed_t<store_type, frac_bits> &vec, const _InputType &value) noexcept
		{
			// return vec + (fixed_t<store_type, frac_bits>)value;
			return fixed_t<store_type, frac_bits>::fromRaw(vec.value + fixed_t<store_type, frac_bits>(value).value);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, fixed_t<store_type, frac_bits>>::value,
					  bool>::type = true>
		static inline constexpr fixed_t<store_type, frac_bits> operator+(const _InputType &value, const fixed_t<store_type, frac_bits> &vec) noexcept
		{
			// return ((fixed_t<store_type, frac_bits>)value + vec);
			return fixed_t<store_type, frac_bits>::fromRaw(fixed_t<store_type, frac_bits>(value).value + vec.value);
		}
		template <typename store_type, int frac_bits>
		static inline constexpr fixed_t<store_type, frac_bits> operator-(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(vecA) - vecB);
			return fixed_t<store_type, frac_bits>::fromRaw(vecA.value - vecB.value);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, fixed_t<store_type, frac_bits>>::value,
					  bool>::type = true>
		static inline constexpr fixed_t<store_type, frac_bits> operator-(const fixed_t<store_type, frac_bits> &vec, const _InputType &value) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(vec) - (fixed_t<store_type, frac_bits>)value);
			return fixed_t<store_type, frac_bits>::fromRaw(vec.value - fixed_t<store_type, frac_bits>(value).value);
		}
		template <typename store_type, typename _InputType, int frac_bits,
				  typename std::enable_if<
					  std::is_convertible<_InputType, fixed_t<store_type, frac_bits>>::value,
					  bool>::type = true>
		static inline constexpr fixed_t<store_type, frac_bits> operator-(const _InputType &value, const fixed_t<store_type, frac_bits> &vec) noexcept
		{
			// return ((fixed_t<store_type, frac_bits>)value - vec);
			return fixed_t<store_type, frac_bits>::fromRaw(fixed_t<store_type, frac_bits>(value).value - vec.value);
		}

		//
		// Bitwise Block
		//
		template <typename store_type, int frac_bits>
		static inline constexpr fixed_t<store_type, frac_bits> operator&(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(vecA) & vecB);
			return fixed_t<store_type, frac_bits>::fromRaw(vecA.value & vecB.value);
		}
		template <typename store_type, int frac_bits>
		static inline constexpr fixed_t<store_type, frac_bits> operator|(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(vecA) | vecB);
			return fixed_t<store_type, frac_bits>::fromRaw(vecA.value | vecB.value);
		}
		template <typename store_type, int frac_bits>
		static inline constexpr fixed_t<store_type, frac_bits> operator^(const fixed_t<store_type, frac_bits> &vecA, const fixed_t<store_type, frac_bits> &vecB) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(vecA) ^ vecB);
			return fixed_t<store_type, frac_bits>::fromRaw(vecA.value ^ vecB.value);
		}

		//
		// Shift block
		//
		template <typename store_type, int frac_bits>
		static inline constexpr fixed_t<store_type, frac_bits> operator<<(const fixed_t<store_type, frac_bits> &vecA, int shift) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(vecA) << shift);
			return vecA.operator_shift_left(shift);
		}
		template <typename store_type, int frac_bits>
		static inline constexpr fixed_t<store_type, frac_bits> operator>>(const fixed_t<store_type, frac_bits> &vecA, int shift) noexcept
		{
			// return (fixed_t<store_type, frac_bits>(vecA) >> shift);
			return vecA.operator_shift_right(shift);
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