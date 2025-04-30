#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'vec2_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "vec2_base.h"

#include "../fixed_t.h"

namespace MathCore
{

    /// \brief Vector 2D (vec2)
    ///
    /// Stores two components(x,y) to represent a bidimensional vector. <br/>
    /// It can be used as points or vectors in 2D.
    ///
    /// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
    ///
    /// It is possible to use any arithmetic with vec2 and float combinations.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aRibeiroCore/aRibeiroCore.h>
    /// using namespace aRibeiro;
    ///
    /// vec2 a, b, result;
    ///
    /// result = ( a * 0.25f + b * 0.75f ) * 2.0f + 1.0f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename store_type, int frac_bits, typename _SimdType>
    class alignas(16) vec2<FixedPoint::fixed_t<store_type, frac_bits>, _SimdType,
                           typename std::enable_if<
                               (std::is_same<store_type, int32_t>::value || std::is_same<store_type, uint32_t>::value) &&
                               (std::is_same<_SimdType, SIMD_TYPE::SSE>::value ||
                                std::is_same<_SimdType, SIMD_TYPE::NEON>::value)>::type>
    {
        using _BaseType = FixedPoint::fixed_t<store_type, frac_bits>;
        using self_type = vec2<_BaseType, _SimdType>;

#if defined(ITK_NEON)
        using neon_type = typename iNeonOps<store_type>::type_v2;
#endif

    public:
        static constexpr int array_count = 2;
        using type = self_type;
        using element_type = _BaseType;

        union
        {
            _BaseType array[2];
            struct
            {
                _BaseType x, y;
            };
            struct
            {
                _BaseType width, height;
            };
#if defined(ITK_SSE2)
            __m128i array_sse;
#elif defined(ITK_NEON)
            neon_type array_neon;
#endif
        };

#if defined(ITK_SSE2)
        ITK_INLINE vec2(const __m128i &v)
        {
            array_sse = v;
        }
#elif defined(ITK_NEON)
        ITK_INLINE vec2(const neon_type &v)
        {
            array_neon = v;
        }
#endif

        /// \brief Construct a ZERO vec2 class
        ///
        /// The ZERO vec2 class have the point information in the origin (x=0,y=0)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec = vec2();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ITK_INLINE vec2()
        {
#if defined(ITK_SSE2)
            array_sse = _mm_set1_epi32(0);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vset1_v2(0); //(neon_type){0, 0, 0, 0};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
        /*constexpr ITK_INLINE vec2() :x(0), y(0) {}*/
        /// \brief Constructs a bidimensional Vector
        ///
        /// Initialize the vec2 components with the same float value (by scalar)
        ///
        /// X = v and Y = v
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec = vec2( 0.5f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to initialize the components
        ///
        ITK_INLINE vec2(const _BaseType &v)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_set1_epi32(v.value);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vset1_v2(v.value);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        template <typename _InputType,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          !std::is_same<_InputType, _BaseType>::value,
                      bool>::type = true>
        ITK_INLINE vec2(const _InputType &v) : self_type((_BaseType)v) {}

        /*constexpr ITK_INLINE vec2(const _BaseType& _v) :x(_v), y(_v) {}*/
        /// \brief Constructs a bidimensional Vector
        ///
        /// Initialize the vec2 components from the parameters
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec = vec2( 0.1f, 0.2f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the X component of the vector
        /// \param y Value to assign to the Y component of the vector
        ///
        ITK_INLINE vec2(const _BaseType &x, const _BaseType &y)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_setr_epi32(x.value, y.value, 0, 0);
#elif defined(ITK_NEON)
            array_neon = (neon_type){x.value, y.value};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        template <typename __x, typename __y,
                  typename std::enable_if<
                      std::is_convertible<__x, _BaseType>::value &&
                          std::is_convertible<__y, _BaseType>::value &&

                          !(std::is_same<__x, _BaseType>::value &&
                            std::is_same<__y, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE vec2(const __x &_x, const __y &_y) : self_type((_BaseType)_x, (_BaseType)_y)
        {
        }

        /*constexpr ITK_INLINE vec2(const _BaseType& _x, const _BaseType& _y) :x(_x), y(_y) {}*/
        /// \brief Constructs a bidimensional Vector
        ///
        /// Initialize the vec2 components from other vec2 instance by copy
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec_source;
        ///
        /// vec2 vec = vec2( vec_source );
        ///
        /// vec2 veca = vec_source;
        ///
        /// vec2 vecb;
        /// vecb = vec_source;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to assign to the instance
        ///
        ITK_INLINE vec2(const self_type &v)
        {
#if defined(ITK_SSE2)
            array_sse = v.array_sse;
#elif defined(ITK_NEON)
            array_neon = v.array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
        ITK_INLINE self_type &operator=(const self_type &v)
        {
#if defined(ITK_SSE2)
            array_sse = v.array_sse;
#elif defined(ITK_NEON)
            array_neon = v.array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return *this;
        }
        /*constexpr ITK_INLINE vec2(const self_type& _v) :x(_v.x), y(_v.y) {}*/
        /// \brief Constructs a bidimensional Vector from the subtraction b-a
        ///
        /// Initialize the vec2 components from two other vectors using the equation: <br />
        /// this = b - a
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec_a, vec_b;
        ///
        /// vec2 vec_a_to_b = vec2( vec_a, vec_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Orign vector
        /// \param b Destiny vector
        ///
        ITK_INLINE vec2(const self_type &a, const self_type &b)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_sub_epi32(b.array_sse, a.array_sse);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vsubq_v2(b.array_neon, a.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Compare vectors considering #EPSILON (equal)
        ///
        /// Compare two vectors using #EPSILON to see if they are the same.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec_a, vec_b;
        ///
        /// if ( vec_a == vec_b ){
        ///     //do something
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to compare against
        /// \return true if the values are the same considering #EPSILON
        ///
        ITK_INLINE bool operator==(const self_type &v) const
        {
#if defined(ITK_SSE2)
            __m128i eq = _mm_cmpeq_epi32(array_sse, v.array_sse);
            eq = _mm_or_si128(eq, _vec4i_00xx_sse);
#if defined(ITK_SSE_SKIP_SSE41)
            int mask = _mm_movemask_epi8(eq);
            return mask == 0xFFFF;
#else
            return _mm_test_all_ones(eq) != 0;
#endif

#elif defined(ITK_NEON)
            return iNeonOps<store_type>::eq_v2(array_neon, v.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        // inter SIMD types converting...
        template <typename _InputType, typename _InputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          !(std::is_same<_InputSimdTypeAux, _SimdType>::value &&
                            std::is_same<_InputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE self_type &operator=(const vec2<_InputType, _InputSimdTypeAux> &vec)
        {
            *this = self_type((_BaseType)vec.x, (_BaseType)vec.y);
            return *this;
        }
        // inter SIMD types converting...
        template <typename _OutputType, typename _OutputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_BaseType, _OutputType>::value &&
                          !(std::is_same<_OutputSimdTypeAux, _SimdType>::value &&
                            std::is_same<_OutputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE operator vec2<_OutputType, _OutputSimdTypeAux>() const
        {
            return vec2<_OutputType, _OutputSimdTypeAux>(
                (_OutputType)x, (_OutputType)y);
        }

        /// \brief Compare vectors considering #EPSILON (not equal)
        ///
        /// Compare two vectors using #EPSILON to see if they are the same.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec_a, vec_b;
        ///
        /// if ( vec_a != vec_b ){
        ///     //do something
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to compare against
        /// \return true if the values are not the same considering #EPSILON
        ///
        ITK_INLINE bool operator!=(const self_type &v) const
        {
            return !((*this) == v);
        }

        /// \brief Component-wise sum (add) operator overload
        ///
        /// Increment the vector by the components of another vector
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec, vec_b;
        ///
        /// vec += vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to increment the current vector instance
        /// \return A reference to the current instance after the increment
        ///
        ITK_INLINE self_type &operator+=(const self_type &v)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_add_epi32(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vaddq_v2(array_neon, v.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return (*this);
        }

        /// \brief Component-wise subtract operator overload
        ///
        /// Decrement the vector by the components of another vector
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec, vec_b;
        ///
        /// vec -= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to decrement the current vector instance
        /// \return A reference to the current instance after the decrement
        ///
        ITK_INLINE self_type &operator-=(const self_type &v)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_sub_epi32(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vsubq_v2(array_neon, v.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return (*this);
        }

        /// \brief Component-wise minus operator overload
        ///
        /// Negates the vector components with the operator minus
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec;
        ///
        /// vec = -vec;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A copy of the current instance after the negation operation
        ///
        ITK_INLINE self_type operator-() const
        {
#if defined(ITK_SSE2)
            // const __m128i _vec2_sign_mask = _mm_setr_ps(-0.f, -0.f, 0.f, 0.0f);
            return _mm_sub_epi32(_vec4i_zero_sse, array_sse);
#elif defined(ITK_NEON)
            return iNeonOps<store_type>::vneg_v2(array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Component-wise multiply operator overload
        ///
        /// Multiply the vector by the components of another vector
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec, vec_b;
        ///
        /// vec *= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to multiply the current vector instance
        /// \return A reference to the current instance after the multiplication
        ///
        template <typename _internal_type = store_type,
                  typename std::enable_if<
                      std::is_same<_internal_type, int32_t>::value,
                      bool>::type = true>
        ITK_INLINE self_type &operator*=(const self_type &v)
        {
#if defined(ITK_SSE2)
            __m128i aabb_self = _mm_shuffle_epi32(array_sse, _MM_SHUFFLE(1, 1, 0, 0));
            __m128i aabb_v = _mm_shuffle_epi32(v.array_sse, _MM_SHUFFLE(1, 1, 0, 0));

            __m128i ab = _mm_mul_epi32(aabb_self, aabb_v);
            ab = _mm_srai_epi64(ab, frac_bits);

            array_sse = _mm_shuffle_epi32(ab, _MM_SHUFFLE(2, 0, 2, 0));
#elif defined(ITK_NEON)
            int64x2_t r1_64 = vmull_s32(array_neon, v.array_neon);
            r1_64 = vshrq_n_s64(r1_64, frac_bits);
            array_neon = vmovn_s64(r1_64);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return (*this);
        }

        template <typename _internal_type = store_type,
                  typename std::enable_if<
                      std::is_same<_internal_type, uint32_t>::value,
                      bool>::type = true>
        ITK_INLINE self_type &operator*=(const self_type &v)
        {
#if defined(ITK_SSE2)
            __m128i aabb_self = _mm_shuffle_epi32(array_sse, _MM_SHUFFLE(1, 1, 0, 0));
            __m128i aabb_v = _mm_shuffle_epi32(v.array_sse, _MM_SHUFFLE(1, 1, 0, 0));

            __m128i ab = _mm_mul_epu32(aabb_self, aabb_v);
            ab = _mm_srli_epi64(ab, frac_bits);

            array_sse = _mm_shuffle_epi32(ab, _MM_SHUFFLE(2, 0, 2, 0));
#elif defined(ITK_NEON)
            uint64x2_t r1_64 = vmull_u32(array_neon, v.array_neon);
            r1_64 = vshrq_n_u64(r1_64, frac_bits);
            array_neon = vmovn_u64(r1_64);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return (*this);
        }

        /// \brief Component-wise division operator overload
        ///
        /// Divides the vector by the components of another vector
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec, vec_b;
        ///
        /// vec /= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to divide the current vector instance
        /// \return A reference to the current instance after the division
        ///
        ITK_INLINE self_type &operator/=(const self_type &v)
        {
#if defined(ITK_SSE2)
            x /= v.x;
            y /= v.y;
            // array_sse = _mm_div_ps(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            // array_neon = vdivq_f32(array_neon, v.array_neon);
            x /= v.x;
            y /= v.y;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return (*this);
        }

        /// \brief Single value increment (add, sum) operator overload
        ///
        /// Increment the vector components by a single value (scalar)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec;
        ///
        /// vec += 0.5f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to increment all components of the current vector instance
        /// \return A reference to the current instance after the increment
        ///
        ITK_INLINE self_type &operator+=(const _BaseType &v)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_add_epi32(array_sse, _mm_set1_epi32(v.value));
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vaddq_v2(array_neon, iNeonOps<store_type>::vset1_v2(v.value));
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return (*this);
        }

        /// \brief Single value decrement (subtract) operator overload
        ///
        /// Decrement the vector components by a single value (scalar)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec;
        ///
        /// vec -= 0.5f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to decrement all components of the current vector instance
        /// \return A reference to the current instance after the decrement
        ///
        ITK_INLINE self_type &operator-=(const _BaseType &v)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_sub_epi32(array_sse, _mm_set1_epi32(v.value));
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vsubq_v2(array_neon, iNeonOps<store_type>::vset1_v2(v.value));
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return (*this);
        }

        /// \brief Single value multiply operator overload
        ///
        /// Decrement the vector components by a single value (scalar)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec;
        ///
        /// vec *= 0.5f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to decrement all components of the current vector instance
        /// \return A reference to the current instance after the decrement
        ///
        ITK_INLINE self_type &operator*=(const _BaseType &v)
        {
#if defined(ITK_SSE2)
            (*this) *= self_type(v);
            // array_sse = _mm_mul_ps(array_sse, _mm_set1_ps(v));
#elif defined(ITK_NEON)
            // array_neon = vmulq_f32(array_neon, iNeonOps<store_type>::vset1(v));
            (*this) *= self_type(v);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return (*this);
        }

        /// \brief Single value division operator overload
        ///
        /// Divides the vector components by a single value (scalar)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec;
        ///
        /// vec /= 0.5f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to divide all components of the current vector instance
        /// \return A reference to the current instance after the division
        ///
        ITK_INLINE self_type &operator/=(const _BaseType &v)
        {
#if defined(ITK_SSE2)
            // array_sse = _mm_div_ps(array_sse, _mm_set1_ps(v));
            (*this) /= self_type(v);
#elif defined(ITK_NEON)
            // array_neon = vmulq_f32(array_neon, iNeonOps<store_type>::vset1(1.0f / v));
            (*this) /= self_type(v);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return (*this);
        }

        /// \brief Index the components of the vec2 as a C array
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 vec;
        ///
        /// float x = vec[0];
        ///
        /// vec[1] = 1.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The index of the components starting by 0
        /// \return A reference to the element at the index v
        ///
        ITK_INLINE _BaseType &operator[](const int v)
        {
            return array[v];
        }

        /// \brief Index the components of the vec2 as a C array
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// void process_vec( const vec2 &vec ) {
        ///     float x = vec[0];
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The index of the components starting by 0
        /// \return A reference to the element at the index v
        ///
        ITK_INLINE const _BaseType &operator[](const int v) const
        {
            return array[v];
        }


        ITK_INLINE self_type &operator<<=(int shift)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_slli_epi32(array_sse, shift);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vshlq_n_v2(array_neon, shift);
            // x <<= shift;
            // y <<= shift;
            // z <<= shift;
            // w <<= shift;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return *this;
        }

        template <typename _internal_type = store_type,
                  typename std::enable_if<
                      std::is_same<_internal_type, int32_t>::value,
                      bool>::type = true>
        ITK_INLINE self_type &operator>>=(int shift)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_srai_epi32(array_sse, shift);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vshrq_n_v2(array_neon, shift);
            // x >>= shift;
            // y >>= shift;
            // z >>= shift;
            // w >>= shift;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return *this;
        }

        template <typename _internal_type = store_type,
                  typename std::enable_if<
                      std::is_same<_internal_type, uint32_t>::value,
                      bool>::type = true>
        ITK_INLINE self_type &operator>>=(int shift)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_srli_epi32(array_sse, shift);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vshrq_n_v2(array_neon, shift);
            // x >>= shift;
            // y >>= shift;
            // z >>= shift;
            // w >>= shift;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return *this;
        }

        ITK_INLINE self_type &operator&=(const self_type &v)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_and_si128(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vandq_v2(array_neon, v.array_neon);
            // x &= v.x;
            // y &= v.y;
            // z &= v.z;
            // w &= v.w;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return *this;
        }
        ITK_INLINE self_type &operator|=(const self_type &v)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_or_si128(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vorrq_v2(array_neon, v.array_neon);
            // x |= v.x;
            // y |= v.y;
            // z |= v.z;
            // w |= v.w;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return *this;
        }
        ITK_INLINE self_type &operator^=(const self_type &v)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_xor_si128(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::veorq_v2(array_neon, v.array_neon);
            // x ^= v.x;
            // y ^= v.y;
            // z ^= v.z;
            // w ^= v.w;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
            return *this;
        }

        ITK_INLINE self_type operator~() const
        {
#if defined(ITK_SSE2)
            __m128i result = _mm_xor_si128(_vec4i_all_sse, array_sse);
            return self_type(result);
#elif defined(ITK_NEON)
            return iNeonOps<store_type>::vmvnq_v2(array_neon);
            // return self_type(~x, ~y, ~z, ~w);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        
    };

    // INLINE_OPERATION_IMPLEMENTATION(vec2)

    // #pragma pack(pop)

}
