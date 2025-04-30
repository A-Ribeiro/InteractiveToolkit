#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'vec3_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "../vec2.h"

#include "vec3_base.h"

#include "../fixed_t.h"

namespace MathCore
{

    /// \brief Vector 3D (vec3)
    ///
    /// Stores three components(x,y,z) to represent a tridimensional vector. <br/>
    /// It can be used as points or vectors in 3D.
    /// \warning The class is not designed to represent 2D homogeneous space.
    ///
    /// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
    ///
    /// It is possible to use any arithmetic with vec3 and float combinations.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aRibeiroCore/aRibeiroCore.h>
    /// using namespace aRibeiro;
    ///
    /// vec3 a, b, result;
    ///
    /// result = ( a * 0.25f + b * 0.75f ) * 2.0f + 1.0f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename store_type, int frac_bits, typename _SimdType>
    class alignas(16) vec3<FixedPoint::fixed_t<store_type, frac_bits>, _SimdType,
                           typename std::enable_if<
                               (std::is_same<store_type, int32_t>::value || std::is_same<store_type, uint32_t>::value) &&
                               (std::is_same<_SimdType, SIMD_TYPE::SSE>::value ||
                                std::is_same<_SimdType, SIMD_TYPE::NEON>::value)>::type>
    {
        using _BaseType = FixedPoint::fixed_t<store_type, frac_bits>;
        using self_type = vec3<_BaseType, _SimdType>;
        using vec2_compatible_type = vec2<_BaseType, SIMD_TYPE::NONE>;

#if defined(ITK_NEON)
        using neon_type = typename iNeonOps<store_type>::type;
#endif

    public:
        static constexpr int array_count = 3;
        using type = self_type;
        using element_type = _BaseType;

        union
        {
            _BaseType array[4];
            struct
            {
                _BaseType x, y, z;
            };
            struct
            {
                _BaseType r, g, b;
            };
#if defined(ITK_SSE2)
            __m128i array_sse;
#elif defined(ITK_NEON)
            neon_type array_neon;
#endif
        };

#if defined(ITK_SSE2)
        ITK_INLINE vec3(const __m128i &v)
        {
            array_sse = v;
        }
#elif defined(ITK_NEON)
        ITK_INLINE vec3(const neon_type &v)
        {
            array_neon = v;
        }
#endif

        /// \brief Construct a ZERO vec3 class
        ///
        /// The ZERO vec3 class have the point information in the origin (x=0,y=0,z=0)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec = vec3();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ITK_INLINE vec3()
        {
#if defined(ITK_SSE2)
            array_sse = _mm_set1_epi32(0);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vset1(0); //(neon_type){0, 0, 0, 0};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
        /*constexpr ITK_INLINE vec3() :array{ 0, 0, 0, 0 }{}*/
        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components with the same float value (by scalar)
        ///
        /// X = v, Y = v and Z = v
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec = vec3( 0.5f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to initialize the components
        ///
        ITK_INLINE vec3(const _BaseType &v)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_setr_epi32(v, v, v, 0);
#elif defined(ITK_NEON)
            array_neon = (neon_type){v, v, v, 0};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        template <typename _InputType,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          !std::is_same<_InputType, _BaseType>::value,
                      bool>::type = true>
        ITK_INLINE vec3(const _InputType &v) : self_type((_BaseType)v) {}

        /*constexpr ITK_INLINE vec3(const _BaseType& _v) :array{ _v, _v, _v, 0 } {}*/
        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from the parameters
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec = vec3( 0.1f, 0.2f, 0.3f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the X component of the vector
        /// \param y Value to assign to the Y component of the vector
        /// \param z Value to assign to the Z component of the vector
        ///
        ITK_INLINE vec3(const _BaseType &x, const _BaseType &y, const _BaseType &z)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_setr_epi32(x.value, y.value, z.value, 0);
#elif defined(ITK_NEON)
            array_neon = (neon_type){x.value, y.value, z.value, 0};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        template <typename __x, typename __y, typename __z,
                  typename std::enable_if<
                      std::is_convertible<__x, _BaseType>::value &&
                          std::is_convertible<__y, _BaseType>::value &&
                          std::is_convertible<__z, _BaseType>::value &&

                          !(std::is_same<__x, _BaseType>::value &&
                            std::is_same<__y, _BaseType>::value &&
                            std::is_same<__z, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE vec3(const __x &_x, const __y &_y, const __z &_z) : self_type((_BaseType)_x, (_BaseType)_y, (_BaseType)_z)
        {
        }

        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from a vec2 xy and an isolated z value
        ///
        /// this->xy = xy <br />
        /// this->z = z
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec = vec3( vec2( 0.1f, 0.2f ), 0.3f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param xy Vector 2D to assign to the components x and y of the instance respectively
        /// \param z Value to assign to the component z of the instance
        ///
        ITK_INLINE vec3(const vec2_compatible_type &xy, const _BaseType &z)
        {
#if defined(ITK_SSE2)
            // array_sse = _mm_setr_ps(xy.x, xy.y, z, 0);
            array_sse = xy.array_sse;
            _mm_f32_(array_sse, 2) = z;
#elif defined(ITK_NEON)
            array_neon = (neon_type){xy.x, xy.y, z, 0};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        template <typename __BT, typename __V2T,
                  typename std::enable_if<

                      std::is_convertible<__BT, _BaseType>::value &&
                          std::is_convertible<__V2T, vec2_compatible_type>::value &&

                          !(std::is_same<__BT, _BaseType>::value &&
                            std::is_same<__V2T, vec2_compatible_type>::value),
                      bool>::type = true>
        ITK_INLINE vec3(const __V2T &_xy, const __BT &_z) : self_type((vec2_compatible_type)_xy, (_BaseType)_z)
        {
        }

        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from an isolated x value and a vec2 yz
        ///
        /// this->x = x <br />
        /// this->yz = yz
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec = vec3( 0.1f, vec2( 0.2f, 0.3f ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the component x of the instance
        /// \param yz Vector 2D to assign to the components y and z of the instance respectively
        ///

        ITK_INLINE vec3(const _BaseType &x, const vec2_compatible_type &yz)
        {
#if defined(ITK_SSE2)
            // array_sse = _mm_setr_epi32(x.value, yz.x.value, yz.y.value, 0);
            array_sse = _mm_shuffle_epi32(yz.array_sse, _MM_SHUFFLE(2, 1, 0, 2)); // first 2 can be ignored...
            iSseOps<store_type>::_mm_32_(array_sse, 0) = x.value;
#elif defined(ITK_NEON)
            array_neon = (neon_type){x, yz.x, yz.y, 0};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        template <typename __BT, typename __V2T,
                  typename std::enable_if<

                      std::is_convertible<__BT, _BaseType>::value &&
                          std::is_convertible<__V2T, vec2_compatible_type>::value &&

                          !(std::is_same<__BT, _BaseType>::value &&
                            std::is_same<__V2T, vec2_compatible_type>::value),
                      bool>::type = true>
        ITK_INLINE vec3(const __BT &_x, const __V2T &_yz) : self_type((_BaseType)_x, (vec2_compatible_type)_yz)
        {
        }

        /*constexpr ITK_INLINE vec3(const _BaseType& _x, const vec2_compatible_type& _yz) :array{ x, _yz.x, _yz.y, 0 } {}*/
        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from other vec3 instance by copy
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec_source;
        ///
        /// vec3 vec = vec3( vec_source );
        ///
        /// vec3 veca = vec_source;
        ///
        /// vec3 vecb;
        /// vecb = vec_source;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to assign to the instance
        ///
        ITK_INLINE vec3(const self_type &v)
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
        /// \brief Constructs a tridimensional Vector from the subtraction b-a
        ///
        /// Initialize the vec3 components from two other vectors using the equation: <br />
        /// this = b - a
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec_a, vec_b;
        ///
        /// vec3 vec_a_to_b = vec3( vec_a, vec_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Orign vector
        /// \param b Destiny vector
        ///
        ITK_INLINE vec3(const self_type &a, const self_type &b)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_sub_epi32(b.array_sse, a.array_sse);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vsubq(b.array_neon, a.array_neon);
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
        /// vec3 vec_a, vec_b;
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
            eq = _mm_or_si128(eq, _vec4i_000x_sse);
#if defined(ITK_SSE_SKIP_SSE41)
            int mask = _mm_movemask_epi8(eq);
            return mask == 0xFFFF;
#else
            return _mm_test_all_ones(eq) != 0;
#endif

#elif defined(ITK_NEON)
            return iNeonOps<store_type>::eq_v3(array_neon, v.array_neon);
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
        ITK_INLINE self_type &operator=(const vec3<_InputType, _InputSimdTypeAux> &vec)
        {
            *this = self_type((_BaseType)vec.x, (_BaseType)vec.y, (_BaseType)vec.z);
            return *this;
        }
        // inter SIMD types converting...
        template <typename _OutputType, typename _OutputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_BaseType, _OutputType>::value &&
                          !(std::is_same<_OutputSimdTypeAux, _SimdType>::value &&
                            std::is_same<_OutputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE operator vec3<_OutputType, _OutputSimdTypeAux>() const
        {
            return vec3<_OutputType, _OutputSimdTypeAux>(
                (_OutputType)x, (_OutputType)y, (_OutputType)z);
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
        /// vec3 vec_a, vec_b;
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
        /// vec3 vec, vec_b;
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
            array_neon = iNeonOps<store_type>::vaddq(array_neon, v.array_neon);
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
        /// vec3 vec, vec_b;
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
            array_neon = iNeonOps<store_type>::vsubq(array_neon, v.array_neon);
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
        /// vec3 vec;
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
            // const __m128i _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
            return _mm_sub_epi32(_vec4i_zero_sse, array_sse);
#elif defined(ITK_NEON)
            return iNeonOps<store_type>::vneg(array_neon);
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
        /// vec3 vec, vec_b;
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
            __m128i ac = _mm_mul_epi32(array_sse, v.array_sse);
            __m128i bd = _mm_mul_epi32(_mm_srli_si128(array_sse, 4), _mm_srli_si128(v.array_sse, 4));
            ac = _mm_srli_epi64(ac, frac_bits);
            bd = _mm_srli_epi64(bd, frac_bits);
            bd = _mm_slli_si128(bd, 4);
#if defined(ITK_SSE_SKIP_SSE41)
            ac = _mm_and_si128(_vec4i_x0x0_sse, ac);
            bd = _mm_and_si128(_vec4i_0x0x_sse, bd);
            array_sse = _mm_or_si128(ac, bd);
#else
            array_sse = _mm_blend_epi16(ac, bd, 0xcc);
#endif

#elif defined(ITK_NEON)
            int64x2_t r1_64 = vmull_s32(vget_low_s32(array_neon), vget_low_s32(v.array_neon));
            int64x2_t r2_64 = vmull_s32(vget_high_s32(array_neon), vget_high_s32(v.array_neon));

            r1_64 = vshrq_n_s64(r1_64, frac_bits);
            r2_64 = vshrq_n_s64(r2_64, frac_bits);

            // r1_64[0] >>= frac_bits;
            // r1_64[1] >>= frac_bits;
            // r2_64[0] >>= frac_bits;
            // r2_64[1] >>= frac_bits;

            int32x2_t low = vmovn_s64(r1_64);
            int32x2_t high = vmovn_s64(r2_64);

            array_neon = vcombine_s32(low, high);
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
            __m128i ac = _mm_mul_epu32(array_sse, v.array_sse);
            __m128i bd = _mm_mul_epu32(_mm_srli_si128(array_sse, 4), _mm_srli_si128(v.array_sse, 4));
            ac = _mm_srli_epi64(ac, frac_bits);
            bd = _mm_srli_epi64(bd, frac_bits);
            bd = _mm_slli_si128(bd, 4);
#if defined(ITK_SSE_SKIP_SSE41)
            ac = _mm_and_si128(_vec4i_x0x0_sse, ac);
            bd = _mm_and_si128(_vec4i_0x0x_sse, bd);
            array_sse = _mm_or_si128(ac, bd);
#else
            array_sse = _mm_blend_epi16(ac, bd, 0xcc);
#endif

#elif defined(ITK_NEON)
            uint64x2_t r1_64 = vmull_u32(vget_low_u32(array_neon), vget_low_u32(v.array_neon));
            uint64x2_t r2_64 = vmull_u32(vget_high_u32(array_neon), vget_high_u32(v.array_neon));

            r1_64 = vshrq_n_u64(r1_64, frac_bits);
            r2_64 = vshrq_n_u64(r2_64, frac_bits);

            // r1_64[0] >>= frac_bits;
            // r1_64[1] >>= frac_bits;
            // r2_64[0] >>= frac_bits;
            // r2_64[1] >>= frac_bits;

            uint32x2_t low = vmovn_u64(r1_64);
            uint32x2_t high = vmovn_u64(r2_64);

            array_neon = vcombine_u32(low, high);

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
        /// vec3 vec, vec_b;
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
            z /= v.z;
            // array_sse = _mm_div_ps(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            // array_neon = vdivq_f32(array_neon, v.array_neon);
            x /= v.x;
            y /= v.y;
            z /= v.z;
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
        /// vec3 vec;
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
            array_neon = iNeonOps<store_type>::vaddq(array_neon, iNeonOps<store_type>::vset1(v.value));
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
        /// vec3 vec;
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
            array_neon = iNeonOps<store_type>::vsubq(array_neon, iNeonOps<store_type>::vset1(v.value));
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
        /// vec3 vec;
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
        /// vec3 vec;
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
        /// \brief Index the components of the vec3 as a C array
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 vec;
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

        /// \brief Index the components of the vec3 as a C array
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// void process_vec( const vec3 &vec ) {
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
            array_neon = iNeonOps<store_type>::vshlq_n(array_neon, shift);
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
            array_neon = iNeonOps<store_type>::vshrq_n(array_neon, shift);
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
            array_neon = iNeonOps<store_type>::vshrq_n(array_neon, shift);
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
            array_neon = iNeonOps<store_type>::vandq(array_neon, v.array_neon);
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
            array_neon = iNeonOps<store_type>::vorrq(array_neon, v.array_neon);
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
            array_neon = iNeonOps<store_type>::veorq(array_neon, v.array_neon);
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
            return iNeonOps<store_type>::vmvnq(array_neon);
            // return self_type(~x, ~y, ~z, ~w);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
    };
}
