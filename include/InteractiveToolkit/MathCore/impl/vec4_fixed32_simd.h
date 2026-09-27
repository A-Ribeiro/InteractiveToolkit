#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'vec4_fixed32_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "../vec2.h"
#include "../vec3.h"

#include "vec4_base.h"

#include "../fixed_t.h"

namespace MathCore
{

    /// \brief Homogeneous 3D (vec4)
    ///
    /// Stores four components(x,y,z,w) to represent a tridimensional vector with the homogeneous component w. <br/>
    /// It can be used to represent points or vectors in 3D.
    ///
    /// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
    ///
    /// It is possible to use any arithmetic with vec4 and float combinations.
    ///
    /// Example:
    ///
    /// \code
    ///
    /// vec4 a, b, result;
    ///
    /// result = ( a * 0.25f + b * 0.75f ) * 2.0f + 1.0f;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam store_type The integer storage type of each fixed point component (int32_t or uint32_t).
    /// \tparam frac_bits The number of fractional bits of the fixed point representation.
    /// \tparam _SimdType The SIMD strategy used for the vector; this specialization
    ///         is selected when _SimdType is SIMD_TYPE::SSE or SIMD_TYPE::NEON.
    ///
    template <typename store_type, int frac_bits, typename _SimdType>
    class alignas(16) vec4<FixedPoint::fixed_t<store_type, frac_bits>, _SimdType,
                           typename std::enable_if<
                               (std::is_same<store_type, int32_t>::value || std::is_same<store_type, uint32_t>::value) &&
                               (std::is_same<_SimdType, SIMD_TYPE::SSE>::value || std::is_same<_SimdType, SIMD_TYPE::NEON>::value)>::type>
    {
        /// \brief The fixed point scalar type of each component.
        ///
        using _BaseType = FixedPoint::fixed_t<store_type, frac_bits>;
        /// \brief Alias for the fully specialized vec4 type.
        ///
        using self_type = vec4<_BaseType, _SimdType>;
        /// \brief Alias for the vec3 type with the same base type and SIMD strategy.
        ///
        using vec3_compatible_type = vec3<_BaseType, _SimdType>;
        /// \brief Alias for the vec2 type with the same base type and SIMD strategy.
        ///
        using vec2_compatible_type = vec2<_BaseType, _SimdType>;

#if defined(ITK_NEON)
        /// \brief The NEON SIMD vector type for the store type.
        ///
        using neon_type = typename iNeonOps<store_type>::type;
#endif

    public:
        /// \brief Number of components stored by the vector (always 4).
        ///
        static constexpr int array_count = 4;
        /// \brief Alias for the vector type itself.
        ///
        using type = self_type;
        /// \brief The scalar type of each component.
        ///
        using element_type = _BaseType;

        /// \brief Union providing multiple views of the four components.
        ///
        /// The components can be accessed as a C array (array),
        /// as named components (x, y, z, w), as color components (r, g, b, a),
        /// as rectangle components (left, top, width, height),
        /// as corner components (right, bottom)
        /// or as the raw SIMD register (array_sse / array_neon).
        ///
        union
        {
            /// \brief The components as a C array (index 0 = x, index 1 = y, index 2 = z, index 3 = w).
            ///
            _BaseType array[4];
            struct
            {
                /// \brief The X component of the vector.
                ///
                _BaseType x;
                /// \brief The Y component of the vector.
                ///
                _BaseType y;
                /// \brief The Z component of the vector.
                ///
                _BaseType z;
                /// \brief The W (homogeneous) component of the vector.
                ///
                _BaseType w;
            };
            struct
            {
                /// \brief The X component viewed as a red color value.
                ///
                _BaseType r;
                /// \brief The Y component viewed as a green color value.
                ///
                _BaseType g;
                /// \brief The Z component viewed as a blue color value.
                ///
                _BaseType b;
                /// \brief The W component viewed as an alpha color value.
                ///
                _BaseType a;
            };
            struct
            {
                /// \brief The X component viewed as a left coordinate value.
                ///
                _BaseType left;
                /// \brief The Y component viewed as a top coordinate value.
                ///
                _BaseType top;
                /// \brief The Z component viewed as a width value.
                ///
                _BaseType width;
                /// \brief The W component viewed as a height value.
                ///
                _BaseType height;
            };
            struct
            {
                /// \brief The X component viewed as a right coordinate value.
                ///
                _BaseType right;
                /// \brief The Y component viewed as a bottom coordinate value.
                ///
                _BaseType bottom;
            };
#if defined(ITK_SSE2)
            /// \brief The components as a raw SSE SIMD register.
            ///
            __m128i array_sse;
#elif defined(ITK_NEON)
            /// \brief The components as a raw NEON SIMD register.
            ///
            neon_type array_neon;
#endif
        };

#if defined(ITK_SSE2)
        /// \brief Constructs a vec4 from a raw SIMD register
        ///
        /// Initialize the vec4 components directly from a SIMD register
        /// holding the four fixed point values.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// __m128i reg = _mm_setr_epi32(0, 1, 2, 3);
        /// vec4 vec = vec4( reg );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v SIMD register holding the four components
        ///
        ITK_INLINE vec4(const __m128i &v)
        {
            array_sse = v;
        }
#elif defined(ITK_NEON)
        /// \brief Constructs a vec4 from a raw SIMD register
        ///
        /// Initialize the vec4 components directly from a SIMD register
        /// holding the four fixed point values.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// neon_type reg = {0, 1, 2, 3};
        /// vec4 vec = vec4( reg );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v SIMD register holding the four components
        ///
        ITK_INLINE vec4(const neon_type &v)
        {
            array_neon = v;
        }
#endif

        /// \brief Construct a ZERO vec4 class
        ///
        /// The ZERO vec4 class has the point information in the origin as a vector (x=0,y=0,z=0,w=0)
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ITK_INLINE vec4()
        {
#if defined(ITK_SSE2)
            // const __m128i _vec4_zero_sse = _mm_set1_ps(0.0f);
            array_sse = _mm_set1_epi32(0); //_vec4_zero_sse;// = _mm_set1_ps(0.0f);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vset1(0); //(neon_type){0, 0, 0, 0};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
        /*constexpr ITK_INLINE vec4() :array{ 0,0,0,0 } {}*/
        /// \brief Constructs a tridimensional Vector with homogeneous component
        ///
        /// Initialize the vec3 components with the same value (by scalar)
        ///
        /// X = v, Y = v, Z = v and W = v
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( 0.5f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to initialize the components
        ///
        ITK_INLINE vec4(const _BaseType &v)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_set1_epi32(v.value);
#elif defined(ITK_NEON)
            array_neon = iNeonOps<store_type>::vset1(v.value);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a vec4 with all components set to the same value.
        ///
        /// Initialize the components with the same value (by scalar),
        /// converting the input value to the base type when necessary.
        ///
        /// X = v, Y = v, Z = v and W = v
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( 0.5f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to initialize the components
        ///
        template <typename _InputType,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          !std::is_same<_InputType, _BaseType>::value,
                      bool>::type = true>
        ITK_INLINE vec4(const _InputType &v) : self_type((_BaseType)v) {}

        /// \brief Constructs a tridimensional Vector with homogeneous component
        ///
        /// Initialize the vec4 components from the parameters
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( 0.1f, 0.2f, 0.3f, 1.0f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the X component of the vector
        /// \param y Value to assign to the Y component of the vector
        /// \param z Value to assign to the Z component of the vector
        /// \param w Value to assign to the W component of the vector
        ///
        ITK_INLINE vec4(const _BaseType &x, const _BaseType &y, const _BaseType &z, const _BaseType &w)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_setr_epi32(x.value, y.value, z.value, w.value);
#elif defined(ITK_NEON)
            array_neon = (neon_type){x.value, y.value, z.value, w.value};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a vec4 from four convertible component values.
        ///
        /// Initialize the vec4 components from the parameters, converting
        /// each to the base type when necessary.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( 0.1f, 0.2f, 0.3f, 1.0f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _x Value to assign to the X component of the vector
        /// \param _y Value to assign to the Y component of the vector
        /// \param _z Value to assign to the Z component of the vector
        /// \param _w Value to assign to the W component of the vector
        ///
        template <typename __x, typename __y, typename __z, typename __w,
                  typename std::enable_if<
                      std::is_convertible<__x, _BaseType>::value &&
                          std::is_convertible<__y, _BaseType>::value &&
                          std::is_convertible<__z, _BaseType>::value &&
                          std::is_convertible<__w, _BaseType>::value &&

                          !(std::is_same<__x, _BaseType>::value &&
                            std::is_same<__y, _BaseType>::value &&
                            std::is_same<__z, _BaseType>::value &&
                            std::is_same<__w, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE vec4(const __x &_x, const __y &_y, const __z &_z, const __w &_w) : self_type((_BaseType)_x, (_BaseType)_y, (_BaseType)_z, (_BaseType)_w)
        {
        }

        /// \brief Constructs a tridimensional Vector with homogeneous component
        ///
        /// Initialize the vec4 components from a vec3 xyz and an isolated w value
        ///
        /// this->xyz = xyz <br />
        /// this->w = w
        ///
        /// If the w is 0 the class represent a vector. <br />
        /// If the w is 1 the class represent a point. <br />
        /// Otherwise it might have a result of a projection
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( vec3( 0.1f, 0.2f, 0.3f ), 1.0f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param xyz Vector 3D to assign to the components x, y and Z of the instance respectively
        /// \param w Value to assign to the component w of the instance
        ///
        ITK_INLINE vec4(const vec3_compatible_type &xyz, const _BaseType &w)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_setr_epi32(xyz.x.value, xyz.y.value, xyz.z.value, w.value);
            // array_sse = xyz.array_sse;
            // _mm_f32_(array_sse, 3) = w.value;
#elif defined(ITK_NEON)
            array_neon = (neon_type){xyz.x.value, xyz.y.value, xyz.z.value, w.value};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a vec4 from a vec3 and a convertible w value.
        ///
        /// Initialize the vec4 components from a vec3 xyz and an isolated w value,
        /// converting each to the base type when necessary.
        ///
        /// this->xyz = xyz <br />
        /// this->w = w
        ///
        /// If the w is 0 the class represent a vector. <br />
        /// If the w is 1 the class represent a point. <br />
        /// Otherwise it might have a result of a projection
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( vec3( 0.1f, 0.2f, 0.3f ), 1.0f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _xyz Vector 3D to assign to the components x, y and Z of the instance respectively
        /// \param _w Value to assign to the component w of the instance
        ///
        template <typename __BT, typename __V3T,
                  typename std::enable_if<

                      std::is_convertible<__BT, _BaseType>::value &&
                          std::is_convertible<__V3T, vec3_compatible_type>::value &&

                          !(std::is_same<__BT, _BaseType>::value &&
                            std::is_same<__V3T, vec3_compatible_type>::value),
                      bool>::type = true>
        ITK_INLINE vec4(const __V3T &_xyz, const __BT &_w) : self_type((vec3_compatible_type)_xyz, (_BaseType)_w)
        {
        }

        /// \brief Constructs a tridimensional Vector with homogeneous component
        ///
        /// Initialize the vec4 components from an isolated x value and a vec3 yzw
        ///
        /// this->x = x <br />
        /// this->yzw = yzw
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( 0.1f, vec3( 0.2f, 0.3f, 1.0f ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the component x of the instance
        /// \param yzw Vector 3D to assign to the components y, z and w of the instance respectively
        ///
        ITK_INLINE vec4(const _BaseType &x, const vec3_compatible_type &yzw)
        {
#if defined(ITK_SSE2)
            // array_sse = _mm_setr_epi32(x.value, yzw.x.value, yzw.y.value, yzw.z.value);
            array_sse = _mm_shuffle_epi32(yzw.array_sse, _MM_SHUFFLE(2, 1, 0, 2)); // first 2 can be ignored...
            iSseOps<store_type>::_mm_32_(array_sse, 0) = x.value;
#elif defined(ITK_NEON)
            array_neon = (neon_type){x.value, yzw.x.value, yzw.y.value, yzw.z.value};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a vec4 from a convertible x value and a vec3 yzw.
        ///
        /// Initialize the vec4 components from an isolated x value and a vec3 yzw,
        /// converting each to the base type when necessary.
        ///
        /// this->x = x <br />
        /// this->yzw = yzw
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( 0.1f, vec3( 0.2f, 0.3f, 1.0f ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _x Value to assign to the component x of the instance
        /// \param _yzw Vector 3D to assign to the components y, z and w of the instance respectively
        ///
        template <typename __BT, typename __V3T,
                  typename std::enable_if<

                      std::is_convertible<__BT, _BaseType>::value &&
                          std::is_convertible<__V3T, vec3_compatible_type>::value &&

                          !(std::is_same<__BT, _BaseType>::value &&
                            std::is_same<__V3T, vec3_compatible_type>::value),
                      bool>::type = true>
        ITK_INLINE vec4(const __BT &_x, const __V3T &_yzw) : self_type((_BaseType)_x, (vec3_compatible_type)_yzw)
        {
        }

        //--------------------------------------------------------------------------------

        ITK_INLINE vec4(const vec2_compatible_type &a, const vec2_compatible_type &b)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_setr_epi32(a.x.value, a.y.value, b.x.value, b.y.value);
            // #if defined(ITK_SSE_SKIP_SSE41)
            //             array_sse = _mm_setr_epi32(a.x.value, a.y.value, b.x.value, b.y.value);
            // #else
            //             __m128i a_shuffle = _mm_shuffle_epi32(a.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
            //             __m128i b_shuffle = _mm_shuffle_epi32(b.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
            //             array_sse = _mm_blend_epi16(a_shuffle, b_shuffle, 0xf0);
            // #endif

#elif defined(ITK_NEON)
            array_neon = (neon_type){a.x.value, a.y.value, b.x.value, b.y.value};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a vec4 from two vec2 instances.
        ///
        /// Initialize the vec4 components from two vec2 instances.
        ///
        /// this->x = a.x, this->y = a.y, this->z = b.x, this->w = b.y
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2 a( 0.1f, 0.2f );
        /// vec2 b( 0.3f, 0.4f );
        /// vec4 vec( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a First vec2 whose components become x and y
        /// \param b Second vec2 whose components become z and w
        ///
        template <typename __V2A, typename __V2B,
                  typename std::enable_if<

                      std::is_convertible<__V2A, vec2_compatible_type>::value &&
                          std::is_convertible<__V2B, vec2_compatible_type>::value &&

                          !(std::is_same<__V2A, vec2_compatible_type>::value &&
                            std::is_same<__V2B, vec2_compatible_type>::value),
                      bool>::type = true>
        ITK_INLINE vec4(const __V2A &a, const __V2B &b) : self_type((vec2_compatible_type)a, (vec2_compatible_type)b)
        {
        }

        /// \brief Constructs a tridimensional Vector with homogeneous component
        ///
        /// Initialize the vec4 components from another vec4 instance by copy
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec_source;
        ///
        /// vec4 vec = vec4( vec_source );
        ///
        /// vec4 veca = vec_source;
        ///
        /// vec4 vecb;
        /// vecb = vec_source;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to copy from
        ///
        ITK_INLINE vec4(const self_type &v)
        {
#if defined(ITK_SSE2)
            array_sse = v.array_sse;
#elif defined(ITK_NEON)
            array_neon = v.array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Assigns the components of another vec4 to this instance
        ///
        /// Copy the X, Y, Z and W components from another vec4 instance.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec_a, vec_b;
        ///
        /// vec_a = vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to copy the components from
        /// \return A reference to the current instance after the assignment
        ///
        ITK_INLINE self_type& operator=(const self_type &v)
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

        /// \brief Constructs a tridimensional Vector with homogeneous component from the subtraction b-a
        ///
        /// Initialize the vec4 components from two other vectors using the equation: <br />
        /// this = b - a
        ///
        /// If a and b were points then the result will be a vector pointing from a to b with the w component equals to zero.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec_a, vec_b;
        ///
        /// vec4 vec_a_to_b = vec4( vec_a, vec_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Origin point
        /// \param b Destination point
        ///
        ITK_INLINE vec4(const self_type &a, const self_type &b)
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
        ///
        /// vec4 vec_a, vec_b;
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
#if defined(ITK_SSE_SKIP_SSE41)
            int mask = _mm_movemask_epi8(eq);
            return mask == 0xFFFF;
#else
            return _mm_test_all_ones(eq) != 0;
#endif

#elif defined(ITK_NEON)
            return iNeonOps<store_type>::eq(array_neon, v.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        // inter SIMD types converting...
        /// \brief Assigns the components of a vec4 with a different type/SIMD strategy
        ///
        /// Convert the components of another vec4 instance (different base type
        /// and/or SIMD strategy) to this instance's base type and assign them.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4<float> vec_f;
        /// vec4<FixedPoint::fixed_t<int32_t, 8>> vec_fixed;
        ///
        /// vec_fixed = vec_f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param vec Vector to copy the components from (converted to the base type)
        /// \return A reference to the current instance after the assignment
        ///
        template <typename _InputType, typename _InputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          !(std::is_same<_InputSimdTypeAux, _SimdType>::value &&
                            std::is_same<_InputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE self_type& operator=(const vec4<_InputType, _InputSimdTypeAux> &vec)
        {
            *this = self_type((_BaseType)vec.x, (_BaseType)vec.y, (_BaseType)vec.z, (_BaseType)vec.w);
            return *this;
        }
        // inter SIMD types converting...
        /// \brief Converts the vec4 to another vec4 with a different type/SIMD strategy
        ///
        /// Implicit conversion operator that converts the components to the
        /// output base type and returns a new vec4 instance.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4<FixedPoint::fixed_t<int32_t, 8>> vec_fixed;
        /// vec4<float> vec_f = vec_fixed;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A vec4 instance with the converted components
        ///
        template <typename _OutputType, typename _OutputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_BaseType, _OutputType>::value &&
                          !(std::is_same<_OutputSimdTypeAux, _SimdType>::value &&
                            std::is_same<_OutputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE operator vec4<_OutputType, _OutputSimdTypeAux>() const
        {
            return vec4<_OutputType, _OutputSimdTypeAux>(
                (_OutputType)x, (_OutputType)y, (_OutputType)z, (_OutputType)w);
        }

        /// \brief Compare vectors considering #EPSILON (not equal)
        ///
        /// Compare two vectors using #EPSILON to see if they are the same.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec_a, vec_b;
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
        ///
        /// vec4 vec, vec_b;
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
        ///
        /// vec4 vec, vec_b;
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
        ///
        /// vec4 vec;
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
        ///
        /// vec4 vec, vec_b;
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
            // // array_neon = vmulq_f32(array_neon, v.array_neon);
            // x *= v.x;
            // y *= v.y;
            // z *= v.z;
            // w *= v.w;

            int64x2_t r1_64 = vmull_s32( vget_low_s32(array_neon), vget_low_s32(v.array_neon) );
            int64x2_t r2_64 = vmull_s32( vget_high_s32(array_neon), vget_high_s32(v.array_neon) );

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

        /// \brief Component-wise multiply operator overload (unsigned)
        ///
        /// Multiply the vector by the components of another vector (unsigned variant).
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec, vec_b;
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
            // array_neon = vmulq_f32(array_neon, v.array_neon);
            // x *= v.x;
            // y *= v.y;
            // z *= v.z;
            // w *= v.w;

            uint64x2_t r1_64 = vmull_u32( vget_low_u32(array_neon), vget_low_u32(v.array_neon) );
            uint64x2_t r2_64 = vmull_u32( vget_high_u32(array_neon), vget_high_u32(v.array_neon) );

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
        ///
        /// vec4 vec, vec_b;
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
            w /= v.w;
            // array_sse = _mm_div_ps(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            // array_neon = vdivq_f32(array_neon, v.array_neon);
            x /= v.x;
            y /= v.y;
            z /= v.z;
            w /= v.w;
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
        ///
        /// vec4 vec;
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
        ///
        /// vec4 vec;
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
        /// Multiply the vector components by a single value (scalar)
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec;
        ///
        /// vec *= 0.5f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to multiply all components of the current vector instance
        /// \return A reference to the current instance after the multiplication
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
        ///
        /// vec4 vec;
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
        /// \brief Index the components of the vec4 as a C array
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec;
        ///
        /// float x = vec[0];
        ///
        /// vec[3] = 1.0f;
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

        /// \brief Index the components of the vec4 as a C array
        ///
        /// Example:
        ///
        /// \code
        ///
        /// void process_vec( const vec4 &vec ) {
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

        /// \brief Component-wise left shift operator overload
        ///
        /// Shifts all components left by the specified number of bits.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec;
        ///
        /// vec <<= 2;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param shift Number of bits to shift each component left
        /// \return A reference to the current instance after the shift
        ///
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

        /// \brief Component-wise signed right shift operator overload
        ///
        /// Shifts all components right by the specified number of bits (signed).
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec;
        ///
        /// vec >>= 2;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param shift Number of bits to shift each component right
        /// \return A reference to the current instance after the shift
        ///
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

        /// \brief Component-wise unsigned right shift operator overload
        ///
        /// Shifts all components right by the specified number of bits (unsigned).
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec;
        ///
        /// vec >>= 2;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param shift Number of bits to shift each component right
        /// \return A reference to the current instance after the shift
        ///
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

        /// \brief Component-wise bitwise AND operator overload
        ///
        /// Performs a bitwise AND on each component with the corresponding
        /// component of another vector.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec, vec_b;
        ///
        /// vec &= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to perform bitwise AND with
        /// \return A reference to the current instance after the operation
        ///
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
        /// \brief Component-wise bitwise OR operator overload
        ///
        /// Performs a bitwise OR on each component with the corresponding
        /// component of another vector.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec, vec_b;
        ///
        /// vec |= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to perform bitwise OR with
        /// \return A reference to the current instance after the operation
        ///
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
        /// \brief Component-wise bitwise XOR operator overload
        ///
        /// Performs a bitwise XOR on each component with the corresponding
        /// component of another vector.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec, vec_b;
        ///
        /// vec ^= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to perform bitwise XOR with
        /// \return A reference to the current instance after the operation
        ///
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

        /// \brief Component-wise bitwise NOT operator overload
        ///
        /// Performs a bitwise NOT on each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec;
        ///
        /// vec4 result = ~vec;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A new vec4 instance with each component bitwise NOT'd
        ///
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
