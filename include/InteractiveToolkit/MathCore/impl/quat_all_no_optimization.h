#pragma once

#include "../vec3.h"
#include "../vec4.h"

#include "quat_base.h"

namespace MathCore
{

    /// \brief Quaternion (quat)
    ///
    /// Stores four components(x,y,z,w) to represent a quaternion. <br/>
    /// The quaternion can be seen as a unit axis with an angle in radians in the imaginary space.
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename _BaseType, typename _SimdType>
    class quat<_BaseType, _SimdType,
               typename std::enable_if<
                   std::is_same<_SimdType, SIMD_TYPE::NONE>::value>::type>
    {
        using self_type = quat<_BaseType, _SimdType>;

    public:
        static constexpr int array_count = 4;
        using type = self_type;
        using element_type = _BaseType;

        union
        {
            _BaseType array[4];
            struct
            {
                _BaseType x, y, z, w;
            };
        };

        /// \brief Construct an identity quaternion quat class
        ///
        /// The identity quat class has the folow configuration (x=0,y=0,z=0,w=1)
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
        ITK_INLINE quat() : array{0, 0, 0, (_BaseType)1} {}
        /// \brief Constructs a quaterion
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
        ITK_INLINE quat(const _BaseType &_x, const _BaseType &_y, const _BaseType &_z, const _BaseType &_w) : array{_x, _y, _z, _w} {}

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

        /// \brief Constructs a quaternion
        ///
        /// Initialize the quat components from other quat instance by copying the content of the other quat.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// quat rotation_source;
        ///
        /// quat rotation = quat( rotation_source );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to assign to the instance
        ///
        ITK_INLINE quat(const self_type &v)
        {
            *this = v;
        }
        ITK_INLINE self_type& operator=(const self_type &v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
            return *this;
        }
        // constexpr ITK_INLINE quat(const self_type& q) :array{q.x, q.y, q.z, q.w} {}
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
        template <class _Type = _BaseType,
                  typename std::enable_if<
                      std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE bool operator==(const self_type &v) const
        {
            // _BaseType accumulator = _BaseType();
            // for (int i = 0; i < 4; i++)
            //     accumulator += OP<_BaseType>::abs(array[i] - v.array[i]);
            // // accumulator += (std::abs)(array[i] - v.array[i]);
            // return accumulator <= EPSILON<_BaseType>::high_precision;
            bool equal = true;
            for (int i = 0; i < 4; i++)
                equal = equal && OP<float>::compare_almost_equal(array[i], v.array[i]);
            return equal;
        }

        template <class _Type = _BaseType,
                  typename std::enable_if<
                  !std::is_floating_point<_Type>::value, bool>::type = true>
                  // std::is_integral<_Type>::value, bool>::type = true>
        ITK_INLINE bool operator==(const self_type &v) const
        {
            bool equal = true;
            for (int i = 0; i < 4; i++)
                equal = equal && (array[i] == v.array[i]);
            return equal;
        }

        // inter SIMD types converting...
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
        // inter SIMD types converting...
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
            return self_type(-x, -y, -z, -w);
        }

        /// \brief Comparison of quaternions (not equal)
        ///
        /// Compare two quaternions considering #EPSILON.
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
        /// \return true if the quaternions are not equal considering #EPSILON
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
        /// \param v The index of the components starting by 0
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
        /// \param v The index of the components starting by 0
        /// \return A reference to the element at the index v
        ///
        ITK_INLINE const _BaseType &operator[](const int v) const
        {
            return array[v];
        }

        ITK_INLINE self_type &operator*=(const _BaseType &v)
        {
            x *= v;
            y *= v;
            z *= v;
            w *= v;
            return (*this);
        }
    };

}
