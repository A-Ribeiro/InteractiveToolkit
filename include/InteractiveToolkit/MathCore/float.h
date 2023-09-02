#pragma once

#include "math_common.h"

namespace MathCore
{

    //
    // CONSTANT<float|double>
    //
    template <typename _type>
    struct CONSTANT<_type,
                    typename std::enable_if<
                        std::is_floating_point<_type>::value>::type>
    {
        static constexpr _type PI = (_type)3.1415926535897932384626433832795;
        static constexpr _type _PI_180 = (_type)0.0174532925199432957692222222222222;
        static constexpr _type _180_PI = (_type)57.2957795130823208767981548141052;

        static constexpr _type RAD2DEG = _180_PI;
        static constexpr _type DEG2RAD = _PI_180;

        //static constexpr _type _SQRT_5 = (_type)2.236067977499789805;
        //1st Solution Golden Ratio = (1 + sqrt(5))*0.5
        static constexpr _type PHI = (_type)1.61803398874989484820;
        
        //2nd Solution Golden Ratio = (1 - sqrt(5))*0.5
        static constexpr _type PHI2nd = (_type)-0.61803398874989484820;

        //reciprocal Golden Ratio = 1.0 / PHI
        static constexpr _type RPHI = (_type)0.61803398874989484820;

        // static constexpr _type PI() noexcept { return (_type)3.1415926535897932384626433832795; }
        // static constexpr _type _PI_180() noexcept { return (_type)0.0174532925199432957692222222222222; }
        // static constexpr _type _180_PI() noexcept { return (_type)57.2957795130823208767981548141052; }
    };

    //
    // FloatTypeInfo
    //
    template <typename _type>
    struct FloatTypeInfo
    {
    };
    template <>
    struct FloatTypeInfo<float>
    {
        using type = FloatTypeInfo<float>;
        using compatible_uint = uint32_t;

        static constexpr float minus_zero = -.0f;
        static constexpr float one = 1.0f;
        static constexpr float min = FLT_MIN;
        static constexpr float max = FLT_MAX;
        static constexpr uint32_t one_uint = 0x3f800000;
        static constexpr uint32_t sign_bit = 0x80000000;
        static constexpr uint32_t sign_bit_negated = ~sign_bit;
    };
    template <>
    struct FloatTypeInfo<double>
    {
        using type = FloatTypeInfo<double>;
        using compatible_uint = uint64_t;

        static constexpr double minus_zero = -.0;
        static constexpr double one = 1.0;
        static constexpr double min = 2.22507385850720138309023271733240406e-308;//DBL_MIN;
        static constexpr double max = 1.79769313486231570814527423731704357e+308;//DBL_MAX;
        static constexpr uint64_t one_uint = 0x3ff0000000000000;
        static constexpr uint64_t sign_bit = 0x8000000000000000;
        static constexpr uint64_t sign_bit_negated = ~sign_bit;
    };

    //
    // EPSILON<float|double>
    //

    template <>
    struct EPSILON<float>
    {
        static constexpr float low_precision = 1e-4f;
        static constexpr float high_precision = 1e-6f;
        // static constexpr ITK_INLINE float low_precision() noexcept { return 1e-4f; }
        // static constexpr ITK_INLINE float high_precision() noexcept { return 1e-6f; } // 1e-7f
    };

    template <>
    struct EPSILON<double>
    {
        static constexpr double low_precision = 1e-13;
        static constexpr double high_precision = 1e-15;
        // static constexpr ITK_INLINE double low_precision() noexcept { return 1e-13; }
        // static constexpr ITK_INLINE double high_precision() noexcept { return 1e-15; } // 1e-16
    };

    //
    // OP<float|double>
    //
    template <typename _type, typename _Algorithm>
    struct OP<_type,
              typename std::enable_if<
                  std::is_floating_point<_type>::value>::type,
              _Algorithm>
    {
        using type = _type;
        using self_type = OP<_type>;

        static constexpr ITK_INLINE _type rad_2_deg(const _type &v) noexcept
        {
            return v * CONSTANT<_type>::_180_PI;
        }
        static constexpr ITK_INLINE _type deg_2_rad(const _type &v) noexcept
        {
            return v * CONSTANT<_type>::_PI_180;
        }

        // optimized instructions
        static ITK_INLINE _type abs(const _type &a) noexcept
        {
            using type_info = FloatTypeInfo<_type>;
            using compatible_uint = typename type_info::compatible_uint;

            compatible_uint result = (type_info::sign_bit_negated & (*(compatible_uint *)&a));
            return *((_type *)&result);

            //return (a < (_type)0) ? -a : a;

            //return OP<_type>::sign(a) * a;
        }

        /// \brief Computes the squared distance between two 1D vectors
        ///
        /// The squared distance is the euclidian distance, without the square root:
        ///
        /// |b-a|^2
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float result;
        /// // result = 25.0f
        /// result = sqrDistance(25.0f, 20.0f);
        /// // result = 25.0f
        /// result = sqrDistance(20.0f, 25.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The First vector
        /// \param b The Second vector
        /// \return The squared distance between a and b
        ///
        static ITK_INLINE _type sqrDistance(const _type &a, const _type &b) noexcept
        {
            _type ab = b - a;
            return ab * ab;
        }

        /// \brief Computes the distance between two 1D vectors
        ///
        /// The distance is the euclidian distance from a point a to point b:
        ///
        /// |b-a|
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float result;
        /// // result = 5.0f
        /// result = distance(25.0f, 20.0f);
        /// // result = 5.0f
        /// result = distance(20.0f, 25.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The First vector
        /// \param b The Second vector
        /// \return The distance between a and b
        ///
        static ITK_INLINE _type distance(const _type &a, const _type &b) noexcept
        {
            _type ab = b - a;
            return self_type::abs(ab);
        }

        /// \brief Return the maximum between the two parameters
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float result;
        /// // result = 25.0f
        /// result = maximum(25.0f, 20.0f);
        /// // result = 25.0f
        /// result = maximum(20.0f, 25.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A value to test
        /// \param b A value to test
        /// \return The maximum value of the parameters
        ///
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float maximum(const float &a, const float &b) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_f32_(_mm_max_ss(_mm_set_ss(a), _mm_set_ss(b)), 0);
#elif defined(ITK_NEON)
            float32x2_t max_neon = vmax_f32(vdup_n_f32(a), vdup_n_f32(b));
            return max_neon[0];
#else
            return (a > b) ? a : b;
#endif
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double maximum(const double &a, const double &b) noexcept
        {
            return (a > b) ? a : b;
        }

        /// \brief Return the minimum between the two parameters
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float result;
        /// // result = 20.0f
        /// result = minimum(25.0f, 20.0f);
        /// // result = 20.0f
        /// result = minimum(20.0f, 25.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A value to test
        /// \param b A value to test
        /// \return The minimum value of the parameters
        ///
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float minimum(const float &a, const float &b) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_f32_(_mm_min_ss(_mm_set_ss(a), _mm_set_ss(b)), 0);
#elif defined(ITK_NEON)
            float32x2_t min_neon = vmin_f32(vdup_n_f32(a), vdup_n_f32(b));
            return min_neon[0];
#else
            return (a < b) ? a : b;
#endif
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double minimum(const double &a, const double &b) noexcept
        {
            return (a < b) ? a : b;
        }

        /// \brief Component wise clamp values
        ///
        /// For each component of the vector, evaluate:
        /// ```
        ///     if min < value then return min
        ///     if max > value then return max
        ///     else return value
        /// ```
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float result;
        /// // result = 5.0f
        /// result = clamp(30.0f, 0.0f, 5.0f);
        /// // result = 0.0f
        /// result = clamp(-100.0f, 0.0f, 5.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param value The value to evaluate
        /// \param min The min threshold
        /// \param max The max threshold
        /// \return The evaluated value
        ///
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float clamp(const float &value, const float &min, const float &max) noexcept
        {
#if defined(ITK_SSE2)
            __m128 maxStep = _mm_max_ss(_mm_set_ss(value), _mm_set_ss(min));
            __m128 minStep = _mm_min_ss(maxStep, _mm_set_ss(max));
            return _mm_f32_(minStep, 0);
#elif defined(ITK_NEON)
            float32x2_t max_neon = vmax_f32(vdup_n_f32(value), vdup_n_f32(min));
            float32x2_t min_neon = vmin_f32(max_neon, vdup_n_f32(max));
            return min_neon[0];
#else
            _type maxStep = OP<_type>::maximum(value, min);
            _type minStep = OP<_type>::minimum(maxStep, max);
            return minStep;
            // return (value < min) ? min : ((value > max) ? max : value);
#endif
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double clamp(const double &value, const double &min, const double &max) noexcept
        {
            _type maxStep = OP<_type>::maximum(value, min);
            _type minStep = OP<_type>::minimum(maxStep, max);
            return minStep;
            // return (value < min) ? min : ((value > max) ? max : value);
        }

        /// \brief Compute the sign of a float
        ///
        /// ```
        /// if a >=0 then sign = 1
        /// else sign = -1
        /// ```
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float result;
        /// // result = 1.0f
        /// result = sign(25.0f);
        /// // result = -1.0f
        /// result = sign(-25.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The value to test
        /// \return The sign of a
        ///
        static ITK_INLINE _type sign(const _type &a) noexcept
        {

            // using type_info = FloatTypeInfo<_type>;
            // using compatible_uint = typename type_info::compatible_uint;

            // compatible_uint &value_int = *(compatible_uint *)(&a);
            // compatible_uint sign_int = (value_int & type_info::sign_bit) | type_info::one_uint;
            // return *(_type *)(&sign_int);

            return (a >= (_type)0) ? (_type)1 : (_type)-1;
        }

        static ITK_INLINE _type copy_sign(const _type &value, const _type &signToCopy) noexcept
        {
            using type_info = FloatTypeInfo<_type>;
            using compatible_uint = typename type_info::compatible_uint;

            compatible_uint &value_int = *(compatible_uint *)(&value);
            compatible_uint &sign_int = *(compatible_uint *)(&signToCopy);

            _type result_f;
            compatible_uint &result = *(compatible_uint *)(&result_f);
            result = (sign_int & type_info::sign_bit) | (value_int & ~type_info::sign_bit);

            return result_f;

            //return OP<_type>::abs(value) * signToCopy;
        }

        /// \brief Computes the linear interpolation
        ///
        /// When the fator is between 0 and 1 it returns the convex relation (linear interpolation) between a and b.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float result;
        /// // result = 25.0f
        /// result = lerp(0.0f, 100.0f, 0.25f);
        /// // result = 75.0f
        /// result = lerp(0.0f, 100.0f, 0.75f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Origin Vector
        /// \param b Target Vector
        /// \param factor The amount (%) to leave the Origin to the Target.
        /// \return The interpolation result
        ///
        static ITK_INLINE _type lerp(const _type &a, const _type &b, const _type &factor) noexcept
        {
            return a * ((_type)1 - factor) + (b * factor);
        }

        /// \brief Move from current to target, considering the max variation
        ///
        /// This function could be used as a constant motion interpolation<br />
        /// between two values considering the delta time and max speed variation.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// PlatformTime timer;
        /// float moveSpeed;
        /// float current;
        /// float target;
        ///
        /// {
        ///     timer.update();
        ///     ...
        ///     // current will be modified to be the target,
        ///     // but the delta time and move speed will make
        ///     // this transition smoother.
        ///     current = move( current, target, time.deltaTime * moveSpeed );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \sa move(float current, float target, float maxDistanceVariation)
        /// \param current The current state
        /// \param target The target state
        /// \param maxDistanceVariation The max amount the current can be modified to reach target
        /// \return the lerp from current to target according max variation
        ///
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float move(const float &current, const float &target, const float &maxDistanceVariation) noexcept
        {
            // const float EPSILON = 1e-6f; high precision epsilon
            float deltaDistance = self_type::distance(current, target);
#if defined(ITK_SSE2) || defined(ITK_NEON) || true // force use this implementation for better performance
            deltaDistance = self_type::maximum(deltaDistance, maxDistanceVariation);
            // avoid division by zero
            // deltaDistance = self_type::maximum(deltaDistance, EPSILON<float>::high_precision);
            deltaDistance = self_type::maximum(deltaDistance, FloatTypeInfo<float>::min);
            return self_type::lerp(current, target, maxDistanceVariation / deltaDistance);
#else
            if (deltaDistance < maxDistanceVariation + EPSILON<float>::high_precision)
                return target;
            return self_type::lerp(current, target, maxDistanceVariation / deltaDistance);
#endif
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double move(const double &current, const double &target, const double &maxDistanceVariation) noexcept
        {
            // const float EPSILON = 1e-6f; high precision epsilon
            double deltaDistance = self_type::distance(current, target);
#if defined(ITK_SSE2) || defined(ITK_NEON) || true // force use this implementation for better performance
            deltaDistance = self_type::maximum(deltaDistance, maxDistanceVariation);
            // avoid division by zero
            // deltaDistance = self_type::maximum(deltaDistance, EPSILON<float>::high_precision);
            deltaDistance = self_type::maximum(deltaDistance, FloatTypeInfo<double>::min);
            return self_type::lerp(current, target, maxDistanceVariation / deltaDistance);
#else
            if (deltaDistance < maxDistanceVariation + EPSILON<double>::high_precision)
                return target;
            return self_type::lerp(current, target, maxDistanceVariation / deltaDistance);
#endif
        }

        template <typename Algorithm = _Algorithm, class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value &&
                          std::is_same<Algorithm, RSQRT::NORMAL>::value,
                      bool>::type = true>
        static ITK_INLINE float rsqrt(const float &v) noexcept
        {
            // printf("float normal\n");

            using type_info = FloatTypeInfo<_type>;
            float v_ = self_type::maximum(v, type_info::min);
            return 1.0f / sqrtf(v_);
        }

        template <typename Algorithm = _Algorithm, class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value &&
                          std::is_same<Algorithm, RSQRT::SIMD>::value,
                      bool>::type = true>
        static ITK_INLINE float rsqrt(const float &v_) noexcept
        {
            // printf("float simd\n");

            using type_info = FloatTypeInfo<_type>;
            float v = self_type::maximum(v_, type_info::min);
#if defined(ITK_SSE2)
            return _mm_f32_(_mm_rsqrt_ss(_mm_set_ss(v)), 0);
#elif defined(ITK_NEON)
            const float &x = v_;
            float y = vrsqrtes_f32(x);
            // from arm documentation
            // The Newton-Raphson iteration:
            //     y[n+1] = y[n] * (3 - x * (y[n] * y[n])) / 2
            // converges to (1/sqrt(x)) if y0 is the result of VRSQRTE applied to x.
            //
            // Note: The precision did not improve after 2 iterations.
            // way 1
            // y = y * vrsqrtss_f32(y * y, x); // 1st iteration
            // y = y * vrsqrtss_f32(y * y, x); // 2nd iteration
            // way 2
            y = y * vrsqrtss_f32(x * y, y); // 1st iteration
            y = y * vrsqrtss_f32(x * y, y); // 2nd iteration
            return y;
#else
            return 1.0f / sqrtf(v);
#endif
        }
        template <typename Algorithm = _Algorithm, class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value &&
                          std::is_same<Algorithm, RSQRT::CARMACK>::value,
                      bool>::type = true>
        static ITK_INLINE float rsqrt(const float &v) noexcept
        {
            // printf("float carmack\n");

            // http://www.lomont.org/papers/2003/InvSqrt.pdf
            // https://en.wikipedia.org/wiki/Fast_inverse_square_root

            // // original algorithm
            // float x2, y;
            // uint32_t &i = *(uint32_t *)&y;
            // const float threehalfs = 1.5f;
            // x2 = v * 0.5f;
            // y = v;
            // //i = *(long *)&y;
            // i = 0x5f3759df - ( i >> 1 );
            // //y = *(float *)&i;
            // y = y * ( threehalfs - ( x2 * y * y ) ); // 1st iteration, low precision
            // y = y * ( threehalfs - ( x2 * y * y ) ); // 2nd iteration, medium precision
            // //y = y * ( threehalfs - ( x2 * y * y ) ); // 3rd iteration, better precision

            // lomont algorithm - better starting estimative
            float x2, y;
            uint32_t &i = *(uint32_t *)&y;
            const float threehalfs = 1.5f;
            x2 = v * 0.5f;
            y = v;
            // i = *(long *)&y;
            i = 0x5F375A86 - (i >> 1);
            // y = *(float *)&i;
            y = y * (threehalfs - (x2 * y * y)); // 1st iteration, low precision
            y = y * (threehalfs - (x2 * y * y)); // 2nd iteration, medium precision
            // y = y * ( threehalfs - ( x2 * y * y ) ); // 3rd iteration, better precision

            // // Jan Kadlec algorithm - 2.7x more accurate
            // const float &x = v;
            // float y;
            // uint32_t &i = *(uint32_t *)&y;
            // // const float threehalfs = 1.5f;
            // // x2 = v * 0.5f;
            // y = v;
            // // i = *(long *)&y;
            // i = 0x5F1FFFF9 - (i >> 1);
            // // y = *(float *)&i;
            // y = y * (0.703952253f * (2.38924456f - (x * y) * y)); // 1st iteration, low precision

            return y;
        }

        template <typename Algorithm = _Algorithm, class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double rsqrt(const double &v) noexcept
        {
            // printf("double\n");

            using type_info = FloatTypeInfo<_type>;
            double v_ = self_type::maximum(v, type_info::min);
            return 1.0 / sqrt(v_);
        }

        static ITK_INLINE _type step(const _type &threshould, const _type &v) noexcept
        {
            _type _sub = v - threshould;
            _type _sign = self_type::sign(_sub);
            _sign = self_type::maximum(_sign, (_type)0);
            return _sign;
        }

        static ITK_INLINE _type smoothstep(const _type &edge0, const _type &edge1, const _type &x) noexcept
        {
            using type_info = FloatTypeInfo<_type>;
            _type div_factor = self_type::maximum((edge1 - edge0), type_info::min);
            _type t = self_type::clamp((x - edge0) / div_factor, (_type)0, (_type)1);
            return t * t * ((_type)3 - (_type)2 * t);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value,
                      bool>::type = true>
        static ITK_INLINE float sqrt(const float &v) noexcept
        {
            return ::sqrtf(v);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value,
                      bool>::type = true>
        static ITK_INLINE double sqrt(const double &v) noexcept
        {
            return ::sqrt(v);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float floor(const float &v) noexcept
        {
            return ::floorf(v);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double floor(const double &v) noexcept
        {
            return ::floor(v);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float ceil(const float &v) noexcept
        {
            return ::ceilf(v);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double ceil(const double &v) noexcept
        {
            return ::ceil(v);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float round(const float &v) noexcept
        {
            return ::roundf(v);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double round(const double &v) noexcept
        {
            return ::round(v);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float cos(const float &v) noexcept
        {
            return ::cosf(v);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double cos(const double &v) noexcept
        {
            return ::cos(v);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float acos(const float &v) noexcept
        {
            return ::acosf(v);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double acos(const double &v) noexcept
        {
            return ::acos(v);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float sin(const float &v) noexcept
        {
            return ::sinf(v);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double sin(const double &v) noexcept
        {
            return ::sin(v);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float asin(const float &v) noexcept
        {
            return ::asinf(v);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double asin(const double &v) noexcept
        {
            return ::asin(v);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float tan(const float &v) noexcept
        {
            return ::tanf(v);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double tan(const double &v) noexcept
        {
            return ::tan(v);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float atan(const float &v) noexcept
        {
            return ::atanf(v);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double atan(const double &v) noexcept
        {
            return ::atan(v);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float atan2(const float &y, const float &x) noexcept
        {
            return ::atan2f(y, x);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double atan2(const double &y, const double &x) noexcept
        {
            return ::atan2(y, x);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float fmod(const float &a, const float &b) noexcept
        {
            return ::fmodf(a, b);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double fmod(const double &a, const double &b) noexcept
        {
            return ::fmod(a, b);
        }

        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, float>::value, bool>::type = true>
        static ITK_INLINE float pow(const float &a, const float &b) noexcept
        {
            return ::powf(a, b);
        }
        template <class _Type = _type,
                  typename std::enable_if<
                      std::is_same<_Type, double>::value, bool>::type = true>
        static ITK_INLINE double pow(const double &a, const double &b) noexcept
        {
            return ::pow(a, b);
        }
    };
}
