#pragma once

#include "../vec2.h"
#include "../vec3.h"

#include "vec4_base.h"

namespace MathCore
{

    /// \brief Homogeneous 4D (vec4)
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
    /// \tparam _BaseType The scalar type of each component (e.g., float, double).
    /// \tparam _SimdType The SIMD strategy used for the vector; this specialization
    ///         is selected when _SimdType is SIMD_TYPE::NONE (no SIMD optimization).
    ///
    template <typename _BaseType, typename _SimdType>
    class vec4<_BaseType, _SimdType,
               typename std::enable_if<
                   std::is_same<_SimdType, SIMD_TYPE::NONE>::value>::type>
    {
        /// \brief Alias for the fully specialized vec4 type.
        ///
        using self_type = vec4<_BaseType, _SimdType>;
        /// \brief Alias for the vec3 type compatible with this vec4 specialization.
        ///
        using vec3_compatible_type = vec3<_BaseType, _SimdType>;
        /// \brief Alias for the vec2 type compatible with this vec4 specialization.
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
        /// or as corner components (right, bottom).
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
        };

        /// \brief Construct a ZERO vec4 class
        ///
        /// The ZERO vec4 class has the point information in the origin (x=0,y=0,z=0,w=0)
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
        constexpr ITK_INLINE vec4() : array{0, 0, 0, 0} {}
        /// \brief Constructs a homogeneous 4D Vector
        ///
        /// Initialize the vec4 components with the same value (by scalar)
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
        ITK_INLINE vec4(const _BaseType &_v) : array{_v, _v, _v, _v} {}

        /// \brief Constructs a homogeneous 4D Vector
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

        /// \brief Constructs a homogeneous 4D Vector
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
        ITK_INLINE vec4(const _BaseType &_x, const _BaseType &_y, const _BaseType &_z, const _BaseType &_w) : array{_x, _y, _z, _w} {}

        /// \brief Constructs a homogeneous 4D Vector
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

        /// \brief Constructs a homogeneous 4D Vector
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
        ITK_INLINE vec4(const vec3_compatible_type &_xyz, const _BaseType &_w) : array{_xyz.x, _xyz.y, _xyz.z, _w} {}

        /// \brief Constructs a homogeneous 4D Vector
        ///
        /// Initialize the vec4 components from a vec3 xyz and an isolated w value,
        /// converting each value to the base type when necessary.
        ///
        /// this->xyz = xyz <br />
        /// this->w = w
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

        /// \brief Constructs a homogeneous 4D Vector
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
        ITK_INLINE vec4(const _BaseType &_x, const vec3_compatible_type &_yzw) : array{_x, _yzw.x, _yzw.y, _yzw.z} {}

        /// \brief Constructs a homogeneous 4D Vector
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

        /// \brief Constructs a homogeneous 4D Vector
        ///
        /// Initialize the vec4 components from two vec2 instances.
        ///
        /// this->x = a.x, this->y = a.y, this->z = b.x, this->w = b.y
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( vec2( 0.1f, 0.2f ), vec2( 0.3f, 0.4f ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a First vec2 providing x and y components
        /// \param b Second vec2 providing z and w components
        ///
        ITK_INLINE vec4(const vec2_compatible_type &a, const vec2_compatible_type &b) : array{a.x, a.y, b.x, b.y} {}

        /// \brief Constructs a homogeneous 4D Vector
        ///
        /// Initialize the vec4 components from two vec2 instances,
        /// converting each value to the base type when necessary.
        ///
        /// this->x = a.x, this->y = a.y, this->z = b.x, this->w = b.y
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec4 vec = vec4( vec2( 0.1, 0.2 ), vec2( 0.3f, 0.4f ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a First vec2 providing x and y components
        /// \param b Second vec2 providing z and w components
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

        /// \brief Constructs a homogeneous 4D Vector
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
            *this = v;
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
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
            return *this;
        }
        // constexpr ITK_INLINE vec4(const self_type& _v) :array{_v.x, _v.y, _v.z, _v.w} {}
        /// \brief Constructs a homogeneous 4D Vector from the subtraction b-a
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
        /*ITK_INLINE vec4(const self_type &a, const self_type &b)
        {
            x = b.x - a.x;
            y = b.y - a.y;
            z = b.z - a.z;
            w = b.w - a.w;
        }*/
        constexpr ITK_INLINE vec4(const self_type &a, const self_type &b) : array{b.x - a.x, b.y - a.y, b.z - a.z, b.w - a.w} {}
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

        /// \brief Assigns components from a vec4 of a different scalar/SIMD type
        ///
        /// Converts and assigns the components from another vec4 specialization
        /// (different _BaseType or _SimdType) to this instance.
        ///
        /// Example:
        ///
        /// \code
        /// vec4<float, SIMD_TYPE::NONE> a;
        /// vec4<double, SIMD_TYPE::NONE> b;
        /// a = b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param vec Vector of a different type to copy the components from
        /// \return A reference to the current instance after the assignment
        ///
        // inter SIMD types converting...
        template <typename _InputType, typename _InputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          (!std::is_same<_InputSimdTypeAux, _SimdType>::value ||
                           !std::is_same<_InputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE self_type& operator=(const vec4<_InputType, _InputSimdTypeAux> &vec)
        {
            *this = self_type((_BaseType)vec.x, (_BaseType)vec.y, (_BaseType)vec.z, (_BaseType)vec.w);
            return *this;
        }
        /// \brief Converts this vec4 to a vec4 of a different scalar/SIMD type
        ///
        /// Implicitly converts the components to another vec4 specialization
        /// (different _BaseType or _SimdType).
        ///
        /// Example:
        ///
        /// \code
        /// vec4<float, SIMD_TYPE::NONE> a;
        /// vec4<double, SIMD_TYPE::NONE> b = a;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A new vec4 with the converted component types
        ///
        // inter SIMD types converting...
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
        /// Compare two vectors using #EPSILON to see if they are NOT the same.
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
            x += v.x;
            y += v.y;
            z += v.z;
            w += v.w;
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
            x -= v.x;
            y -= v.y;
            z -= v.z;
            w -= v.w;
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
            return self_type(-x, -y, -z, -w);
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
            x *= v.x;
            y *= v.y;
            z *= v.z;
            w *= v.w;
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
            x /= v.x;
            y /= v.y;
            z /= v.z;
            w /= v.w;
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
            x += v;
            y += v;
            z += v;
            w += v;
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
            x -= v;
            y -= v;
            z -= v;
            w -= v;
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
            x *= v;
            y *= v;
            z *= v;
            w *= v;
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
            x /= v;
            y /= v;
            z /= v;
            w /= v;
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

        /// \brief Component-wise left shift assignment operator overload
        ///
        /// Shifts all integer components by the given number of bits.
        ///
        /// Example:
        ///
        /// \code
        /// vec4<int> vec;
        /// vec <<= 2;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param shift Number of bits to shift each component
        /// \return A reference to the current instance after the shift
        ///
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator<<=(int shift)
        {
            x <<= shift;
            y <<= shift;
            z <<= shift;
            w <<= shift;
            return *this;
        }
        /// \brief Component-wise right shift assignment operator overload
        ///
        /// Shifts all integer components by the given number of bits.
        ///
        /// Example:
        ///
        /// \code
        /// vec4<int> vec;
        /// vec >>= 2;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param shift Number of bits to shift each component
        /// \return A reference to the current instance after the shift
        ///
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator>>=(int shift)
        {
            x >>= shift;
            y >>= shift;
            z >>= shift;
            w >>= shift;
            return *this;
        }
        /// \brief Component-wise bitwise AND assignment operator overload
        ///
        /// Performs a bitwise AND between each component of this vector
        /// and the corresponding component of another vector.
        ///
        /// Example:
        ///
        /// \code
        /// vec4<int> a, b;
        /// a &= b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to perform bitwise AND with
        /// \return A reference to the current instance after the operation
        ///
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator&=(const self_type& v)
        {
            x &= v.x;
            y &= v.y;
            z &= v.z;
            w &= v.w;
            return *this;
        }
        /// \brief Component-wise bitwise OR assignment operator overload
        ///
        /// Performs a bitwise OR between each component of this vector
        /// and the corresponding component of another vector.
        ///
        /// Example:
        ///
        /// \code
        /// vec4<int> a, b;
        /// a |= b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to perform bitwise OR with
        /// \return A reference to the current instance after the operation
        ///
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator|=(const self_type& v)
        {
            x |= v.x;
            y |= v.y;
            z |= v.z;
            w |= v.w;
            return *this;
        }
        /// \brief Component-wise bitwise XOR assignment operator overload
        ///
        /// Performs a bitwise XOR between each component of this vector
        /// and the corresponding component of another vector.
        ///
        /// Example:
        ///
        /// \code
        /// vec4<int> a, b;
        /// a ^= b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to perform bitwise XOR with
        /// \return A reference to the current instance after the operation
        ///
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator^=(const self_type& v)
        {
            x ^= v.x;
            y ^= v.y;
            z ^= v.z;
            w ^= v.w;
            return *this;
        }
        /// \brief Component-wise bitwise NOT operator overload
        ///
        /// Negates all integer components using the bitwise NOT operator.
        ///
        /// Example:
        ///
        /// \code
        /// vec4<int> vec;
        /// vec4<int> result = ~vec;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A copy of the current instance with all components bitwise-notted
        ///
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type operator~() const
        {
            return self_type(~x, ~y, ~z, ~w);
        }
    };

}
