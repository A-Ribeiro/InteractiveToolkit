#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'quat_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "quat_base.h"

namespace MathCore
{

    /// \brief Quaternion (quat)
    ///
    /// Stores four components (x, y, z, w) to represent a quaternion. <br/>
    /// The quaternion can be seen as a unit axis with an angle in radians in the imaginary space.
    ///
    /// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _BaseType The scalar type of each component (e.g., float, double).
    /// \tparam _SimdType The SIMD strategy used for the quaternion; this specialization
    ///         is selected when _SimdType is SIMD_TYPE::SSE or SIMD_TYPE::NEON (SIMD optimization).
    ///
    template <typename _BaseType, typename _SimdType>
    class alignas(16) quat<_BaseType, _SimdType,
                           typename std::enable_if<
                               std::is_same<_BaseType, float>::value &&
                               (std::is_same<_SimdType, SIMD_TYPE::SSE>::value ||
                                std::is_same<_SimdType, SIMD_TYPE::NEON>::value)>::type>
    {
        /// \brief Alias for the fully specialized quat type.
        ///
        using self_type = quat<_BaseType, _SimdType>;

    public:
        /// \brief Number of components stored by the quaternion (always 4).
        ///
        static constexpr int array_count = 4;
        /// \brief Alias for the quaternion type itself.
        ///
        using type = self_type;
        /// \brief The scalar type of each component.
        ///
        using element_type = _BaseType;

        /// \brief Union providing multiple views of the four components.
        ///
        /// The components can be accessed as a C array (array),
        /// as named components (x, y, z, w).
        ///
        union
        {
            /// \brief The components as a C array (index 0 = x, index 1 = y, index 2 = z, index 3 = w).
            ///
            _BaseType array[4];
            struct
            {
                /// \brief The X component of the quaternion.
                ///
                _BaseType x;
                /// \brief The Y component of the quaternion.
                ///
                _BaseType y;
                /// \brief The Z component of the quaternion.
                ///
                _BaseType z;
                /// \brief The W component of the quaternion.
                ///
                _BaseType w;
            };
#if defined(ITK_SSE2)
            /// \brief The components as a SIMD SSE register.
            ///
            __m128 array_sse;
#elif defined(ITK_NEON)
            /// \brief The components as a SIMD NEON register.
            ///
            float32x4_t array_neon;
#endif
        };

#if defined(ITK_SSE2)
        /// \brief Constructs a quat from a SIMD register
        ///
        /// Initialize the quat components from a SIMD register value.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// __m128 simd_data;
        /// quat rotation(simd_data);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v SIMD register to initialize the components from
        ///
        ITK_INLINE quat(const __m128 &v)
        {
            array_sse = v;
        }
#elif defined(ITK_NEON)
        /// \brief Constructs a quat from a SIMD register
        ///
        /// Initialize the quat components from a SIMD register value.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// float32x4_t simd_data;
        /// quat rotation(simd_data);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v SIMD register to initialize the components from
        ///
        ITK_INLINE quat(const float32x4_t &v)
        {
            array_neon = v;
        }
#endif

        /// \brief Construct an identity quaternion
        ///
        /// The identity quaternion has the following configuration (x=0,y=0,z=0,w=1)
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat rotation = quat();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ITK_INLINE quat()
        {
#if defined(ITK_SSE2)
            array_sse = _vec4_0001_sse; // _mm_setr_ps(0.0f, 0.0f, 0.0f, 1.0f);
#elif defined(ITK_NEON)
            array_neon = _neon_0001; //(float32x4_t){0.0f, 0.0f, 0.0f, 1.0f};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
        /*constexpr ITK_INLINE quat() :array{0, 0, 0, (_BaseType)1} {}*/
        /// \brief Constructs a quaternion
        ///
        /// Initialize the quat components from the parameters
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat rotation = quat( 0.0f, 0.0f, 0.0f, 1.0f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the X component
        /// \param y Value to assign to the Y component
        /// \param z Value to assign to the Z component
        /// \param w Value to assign to the W component
        ///
        ITK_INLINE quat(const _BaseType &x, const _BaseType &y, const _BaseType &z, const _BaseType &w)
        {
#if defined(ITK_SSE2)
            array_sse = _mm_setr_ps(x, y, z, w);
#elif defined(ITK_NEON)
            array_neon = (float32x4_t){x, y, z, w};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Constructs a quaternion
        ///
        /// Initialize the quat components from the parameters, converting
        /// each value to the base type when necessary.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat rotation = quat( 0.0, 0.0, 0.0, 1.0 );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the X component of the quaternion
        /// \param y Value to assign to the Y component of the quaternion
        /// \param z Value to assign to the Z component of the quaternion
        /// \param w Value to assign to the W component of the quaternion
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
        ITK_INLINE quat(const __x &_x, const __y &_y, const __z &_z, const __w &_w) : self_type((_BaseType)_x, (_BaseType)_y, (_BaseType)_z, (_BaseType)_w)
        {
        }

        /*constexpr ITK_INLINE quat(const _BaseType& _x, const _BaseType& _y, const _BaseType& _z, const _BaseType& _w) :array{_x, _y, _z, _w} {}*/
        /// \brief Constructs a quaternion
        ///
        /// Initialize the quat components from another quat instance by copying the content of the other quat.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat rotation_source;
        ///
        /// quat rotation = quat( rotation_source );
        ///
        /// quat rotationa = rotation_source;
        ///
        /// quat rotationb;
        /// rotationb = rotation_source;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Quaternion to copy from
        ///
        ITK_INLINE quat(const self_type &v)
        {
#if defined(ITK_SSE2)
            array_sse = v.array_sse;
#elif defined(ITK_NEON)
            array_neon = v.array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Assigns the components of another quat to this instance
        ///
        /// Copy the X, Y, Z, and W components from another quat instance.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat rotation_a, rotation_b;
        ///
        /// rotation_a = rotation_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Quaternion to copy the components from
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

        // constexpr ITK_INLINE quat(const self_type& q) :x(q.x), y(q.y), z(q.z), w(q.w) {}

        /// \brief Comparison of quaternions (equal)
        ///
        /// Compare two quaternions considering #EPSILON.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat rotation_a, rotation_b;
        ///
        /// if ( rotation_a == rotation_b ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Quaternion to compare against
        /// \return true if the quaternions are equal considering #EPSILON
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

            // return vget_lane_f32(acc_2_elements,0) <= EPSILON<_BaseType>::high_precision;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Assigns the components of a quat with a different type/SIMD strategy
        ///
        /// Convert the components of another quat instance (different base type
        /// and/or SIMD strategy) to this instance's base type and assign them.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat<float> quat_f;
        /// quat<double> quat_d;
        ///
        /// quat_d = quat_f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param vec Quaternion to copy the components from (converted to the base type)
        /// \return A reference to the current instance after the assignment
        ///
        template <typename _InputType, typename _InputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          !(std::is_same<_InputSimdTypeAux, _SimdType>::value &&
                            std::is_same<_InputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE self_type& operator=(const quat<_InputType, _InputSimdTypeAux> &vec)
        {
            *this = self_type((_BaseType)vec.x, (_BaseType)vec.y, (_BaseType)vec.z, (_BaseType)vec.w);
            return *this;
        }
        /// \brief Converts the quat to another quat with a different type/SIMD strategy
        ///
        /// Implicit conversion operator that converts the components to the
        /// output base type and returns a new quat instance.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat<float> quat_f;
        /// quat<double> quat_d = quat_f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A quat instance with the converted components
        ///
        template <typename _OutputType, typename _OutputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_BaseType, _OutputType>::value &&
                          !(std::is_same<_OutputSimdTypeAux, _SimdType>::value &&
                            std::is_same<_OutputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE operator quat<_OutputType, _OutputSimdTypeAux>() const
        {
            return quat<_OutputType, _OutputSimdTypeAux>(
                (_OutputType)x, (_OutputType)y, (_OutputType)z, (_OutputType)w);
        }

        /// \brief Invert all signs of the quaternion
        ///
        /// Negates all components of the quaternion, effectively inverting its direction.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat rotation;
        ///
        /// rotation = -rotation;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return the quaternion with sign of the elements inverted
        ///
        ITK_INLINE self_type operator-() const
        {
#if defined(ITK_SSE2)
            // const __m128 _vec4_sign_mask = _mm_set1_ps(-0.f); // -0.f = 1 << 31
            return _mm_xor_ps(_vec4_sign_mask_sse, array_sse);
#elif defined(ITK_NEON)
            // const float32x4_t minus_one = (float32x4_t){ -1.0f, -1.0f, -1.0f, -1.0f };
            // return vmulq_f32(_vec4_minus_one, array_neon);
            return vnegq_f32(array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Comparison of quaternions (not equal)
        ///
        /// Compare two quaternions for inequality. For floating-point types,
        /// considers #EPSILON via \c operator==. For non-floating-point types,
        /// uses exact comparison.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat rotation_a, rotation_b;
        ///
        /// if ( rotation_a != rotation_b ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Quaternion to compare against
        /// \return true if the quaternions are not equal
        ///
        ITK_INLINE bool operator!=(const self_type &v) const
        {
            return !((*this) == v);
        }

        /// \brief Index the components of the quat as a C array
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat rotation;
        ///
        /// float x = rotation[0];
        ///
        /// rotation[3] = 1.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The index of the component, starting from 0
        /// \return A reference to the element at the index v
        ///
        ITK_INLINE _BaseType &operator[](const int v)
        {
            return array[v];
        }

        /// \brief Index the components of the quat as a C array
        ///
        /// Example:
        ///
        /// \code
        ///
        /// void processQuaternion ( const quat &rotation ) {
        ///     float x = rotation[0];
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The index of the component, starting from 0
        /// \return A const reference to the element at the index v
        ///
        ITK_INLINE const _BaseType &operator[](const int v) const
        {
            return array[v];
        }

        /// \brief In-place scalar multiplication operator
        ///
        /// Multiply the quaternion components by a single value (scalar) in-place.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat rotation;
        ///
        /// rotation *= 0.5f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to multiply all components of the current quaternion instance
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
    };
}
