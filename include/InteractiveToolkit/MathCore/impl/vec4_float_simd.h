#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'vec4_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "../vec2.h"
#include "../vec3.h"

#include "vec4_base.h"

namespace MathCore
{

    /// \brief Homogeneous 3D (vec4)
    ///
    /// Stores four components (x, y, z, w) to represent a tridimensional vector with the homogeneous component w. <br/>
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
    /// \tparam _BaseType The scalar type of each component (e.g., float, double).
    /// \tparam _SimdType The SIMD strategy used for the vector; this specialization
    ///         is selected when _SimdType is SIMD_TYPE::SSE or SIMD_TYPE::NEON (SIMD optimization).
    ///
    /// \ingroup MathCore
    ///
    template <typename _BaseType, typename _SimdType>
    class alignas(16) vec4<_BaseType, _SimdType,
                           typename std::enable_if<
                               std::is_same<_BaseType, float>::value &&
                               (std::is_same<_SimdType, SIMD_TYPE::SSE>::value ||
                                std::is_same<_SimdType, SIMD_TYPE::NEON>::value)>::type>
    {
        /// \brief Alias for the fully specialized vec4 type.
        ///
        using self_type = vec4<_BaseType, _SimdType>;
        /// \brief Alias for the compatible vec3 type.
        ///
        using vec3_compatible_type = vec3<_BaseType, _SimdType>;
        /// \brief Alias for the compatible vec2 type.
        ///
        using vec2_compatible_type = vec2<_BaseType, _SimdType>;

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
        /// or as a pair of 2D points (right, bottom).
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
                /// \brief The X component viewed as a red color channel.
                ///
                _BaseType r;
                /// \brief The Y component viewed as a green color channel.
                ///
                _BaseType g;
                /// \brief The Z component viewed as a blue color channel.
                ///
                _BaseType b;
                /// \brief The W component viewed as an alpha color channel.
                ///
                _BaseType a;
            };
            struct
            {
                /// \brief The X component viewed as a left coordinate.
                ///
                _BaseType left;
                /// \brief The Y component viewed as a top coordinate.
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
                /// \brief The X component viewed as a right coordinate.
                ///
                _BaseType right;
                /// \brief The Y component viewed as a bottom coordinate.
                ///
                _BaseType bottom;
            };
#if defined(ITK_SSE2)
            /// \brief The components as an SSE register (__m128).
            ///
            __m128 array_sse;
#elif defined(ITK_NEON)
            /// \brief The components as a NEON register (float32x4_t).
            ///
            float32x4_t array_neon;
#endif
        };

#if defined(ITK_SSE2)
        /// \brief Constructs a vec4 from a SIMD register
        ///
        /// Initialize the vec4 components from a SIMD register value.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// __m128 simd_data;
        /// vec4 vec(simd_data);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v SIMD register to initialize the components from
        ///
        ITK_INLINE vec4(const __m128 &v)
        {
            array_sse = v;
        }
#elif defined(ITK_NEON)
        /// \brief Constructs a vec4 from a SIMD register
        ///
        /// Initialize the vec4 components from a SIMD register value.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// float32x4_t simd_data;
        /// vec4 vec(simd_data);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v SIMD register to initialize the components from
        ///
        ITK_INLINE vec4(const float32x4_t &v)
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
            // const __m128 _vec4_zero_sse = _mm_set1_ps(0.0f);
            array_sse = _mm_set1_ps(0.0f); //_vec4_zero_sse;// = _mm_set1_ps(0.0f);
#elif defined(ITK_NEON)
            array_neon = vset1(0.0f); //(float32x4_t){0, 0, 0, 0};
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
            array_sse = _mm_set1_ps(v);
#elif defined(ITK_NEON)
            array_neon = vset1(v);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a tridimensional Vector with homogeneous component
        ///
        /// Initialize the vec4 components with the same value (by scalar),
        /// converting the input value to the base type when necessary.
        ///
        /// X = v, Y = v, Z = v and W = v
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( 0.5 );
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
            array_sse = _mm_setr_ps(x, y, z, w);
#elif defined(ITK_NEON)
            array_neon = (float32x4_t){x, y, z, w};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a tridimensional Vector with homogeneous component
        ///
        /// Initialize the vec4 components from the parameters, converting
        /// each value to the base type when necessary.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( 0.1, 0.2, 0.3, 1.0 );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the X component of the vector
        /// \param y Value to assign to the Y component of the vector
        /// \param z Value to assign to the Z component of the vector
        /// \param w Value to assign to the W component of the vector
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
        /// \param xyz Vector 3D to assign to the components x, y and z of the instance respectively
        /// \param w Value to assign to the component w of the instance
        ///
        ITK_INLINE vec4(const vec3_compatible_type &xyz, const _BaseType &w)
        {
#if defined(ITK_SSE2)
            // array_sse = _mm_setr_ps(xyz.x, xyz.y, xyz.z, w);
            array_sse = xyz.array_sse;
            _mm_f32_(array_sse, 3) = w;
#elif defined(ITK_NEON)
            //array_neon = (float32x4_t){xyz.x, xyz.y, xyz.z, w};
            array_neon = vsetq_lane_f32(w, xyz.array_neon, 3);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a tridimensional Vector with homogeneous component
        ///
        /// Initialize the vec4 components from a vec3 xyz and an isolated w value,
        /// converting each value to the base type when necessary.
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
        /// vec4 vec = vec4( vec3( 0.1f, 0.2f, 0.3f ), 1.0 );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param xyz Vector 3D to assign to the components x, y and z of the instance respectively
        /// \param w Value to assign to the component w of the instance
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
            // array_sse = _mm_setr_ps(x, yzw.x, yzw.y, yzw.z);
            array_sse = _mm_shuffle_ps(yzw.array_sse, yzw.array_sse, _MM_SHUFFLE(2, 1, 0, 2)); // first 2 can be ignored...
            _mm_f32_(array_sse, 0) = x;
#elif defined(ITK_NEON)
            array_neon = (float32x4_t){x, yzw.x, yzw.y, yzw.z};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a tridimensional Vector with homogeneous component
        ///
        /// Initialize the vec4 components from an isolated x value and a vec3 yzw,
        /// converting each value to the base type when necessary.
        ///
        /// this->x = x <br />
        /// this->yzw = yzw
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( 0.1, vec3( 0.2f, 0.3f, 1.0f ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the component x of the instance
        /// \param yzw Vector 3D to assign to the components y, z and w of the instance respectively
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

        /// \brief Constructs a vec4 from two vec2 instances
        ///
        /// Initialize the vec4 components from two vec2 instances.
        /// The first vec2 provides x and y, the second vec2 provides z and w.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2 a, b;
        /// vec4 vec(a, b);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a vec2 to assign the x and y components from
        /// \param b vec2 to assign the z and w components from
        ///
        ITK_INLINE vec4(const vec2_compatible_type &a, const vec2_compatible_type &b)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_shuffle_ps(a.array_sse, b.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
#elif defined(ITK_NEON)
            //array_neon = (float32x4_t){a.x, a.y, b.x, b.y};
            array_neon =  vcombine_f32(a.array_neon, b.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a vec4 from two vec2 instances
        ///
        /// Initialize the vec4 components from two vec2 instances, converting
        /// each value to the base type when necessary.
        /// The first vec2 provides x and y, the second vec2 provides z and w.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2 a, b;
        /// vec4 vec(a, b);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a vec2 to assign the x and y components from
        /// \param b vec2 to assign the z and w components from
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
            array_sse = _mm_sub_ps(b.array_sse, a.array_sse);
#elif defined(ITK_NEON)
            array_neon = vsubq_f32(b.array_neon, a.array_neon);
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
            __m128i result_int = compare_almost_eq_ps(array_sse, v.array_sse);
            int test_all_ones = _mm_test_all_ones(result_int);
            return (bool)test_all_ones;

            // __m128 diff_abs = _mm_sub_ps(array_sse, v.array_sse);
            // // abs
            // // const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
            // diff_abs = _mm_andnot_ps(_vec4_sign_mask_sse, diff_abs);

            // diff_abs = _mm_hadd_ps(diff_abs, diff_abs);
            // diff_abs = _mm_hadd_ps(diff_abs, diff_abs);

            // return _mm_f32_(diff_abs, 0) <= EPSILON<_BaseType>::high_precision;
#elif defined(ITK_NEON)

            uint32x4_t result_int = compare_almost_eq_ps(array_neon, v.array_neon);
            
            uint64x2_t cmp64 = vreinterpretq_u64_u32(result_int);
            return (vgetq_lane_u64(cmp64, 0) & vgetq_lane_u64(cmp64, 1)) == UINT64_C(0xFFFFFFFFFFFFFFFF);

            // float32x4_t diff_abs = vsubq_f32(array_neon, v.array_neon);
            // // abs
            // diff_abs = vabsq_f32(diff_abs);

            // float32x2_t acc_2_elements = vadd_f32(vget_high_f32(diff_abs), vget_low_f32(diff_abs));
            // acc_2_elements = vpadd_f32(acc_2_elements, acc_2_elements);

            // return vget_lane_f32(acc_2_elements, 0) <= EPSILON<_BaseType>::high_precision;
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
        /// vec4<double> vec_d;
        ///
        /// vec_d = vec_f;
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
        ITK_INLINE self_type &operator=(const vec4<_InputType, _InputSimdTypeAux> &vec)
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
        /// vec4<float> vec_f;
        /// vec4<double> vec_d = vec_f;
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
            array_sse = _mm_add_ps(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            array_neon = vaddq_f32(array_neon, v.array_neon);
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
            array_sse = _mm_sub_ps(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            array_neon = vsubq_f32(array_neon, v.array_neon);
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
            // const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
            return _mm_xor_ps(_vec4_sign_mask_sse, array_sse);
#elif defined(ITK_NEON)
            return vnegq_f32(array_neon);
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
        ITK_INLINE self_type &operator*=(const self_type &v)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_mul_ps(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            array_neon = vmulq_f32(array_neon, v.array_neon);
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
            array_sse = _mm_div_ps(array_sse, v.array_sse);
#elif defined(ITK_NEON)
            array_neon = vdivq_f32(array_neon, v.array_neon);
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
            array_sse = _mm_add_ps(array_sse, _mm_set1_ps(v));
#elif defined(ITK_NEON)
            array_neon = vaddq_f32(array_neon, vset1(v));
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
            array_sse = _mm_sub_ps(array_sse, _mm_set1_ps(v));
#elif defined(ITK_NEON)
            array_neon = vsubq_f32(array_neon, vset1(v));
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
            array_sse = _mm_mul_ps(array_sse, _mm_set1_ps(v));
#elif defined(ITK_NEON)
            array_neon = vmulq_f32(array_neon, vset1(v));
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
            array_sse = _mm_div_ps(array_sse, _mm_set1_ps(v));
#elif defined(ITK_NEON)
            array_neon = vmulq_f32(array_neon, vset1(1.0f / v));
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
    };

}
