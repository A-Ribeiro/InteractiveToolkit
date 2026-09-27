#pragma once

#include "../vec2.h"

#include "vec3_base.h"

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
    ///
    /// vec3 a, b, result;
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
    class vec3<_BaseType, _SimdType,
               typename std::enable_if<
                   std::is_same<_SimdType, SIMD_TYPE::NONE>::value>::type>
    {
        /// \brief Alias for the fully specialized vec3 type.
        ///
        using self_type = vec3<_BaseType, _SimdType>;
        /// \brief Alias for the compatible vec2 type used by some constructors.
        ///
        using vec2_compatible_type = vec2<_BaseType, _SimdType>;

    public:
        /// \brief Number of components stored by the vector (always 3).
        ///
        static constexpr int array_count = 3;
        /// \brief Alias for the vector type itself.
        ///
        using type = self_type;
        /// \brief The scalar type of each component.
        ///
        using element_type = _BaseType;

        /// \brief Union providing multiple views of the three components.
        ///
        /// The components can be accessed as a C array (array),
        /// as named components (x, y, z) or as color components (r, g, b).
        ///
        union
        {
            /// \brief The components as a C array (index 0 = x, index 1 = y, index 2 = z).
            ///
            _BaseType array[3];
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
            };
        };

        /// \brief Construct a ZERO vec3 class
        ///
        /// The ZERO vec3 class has the point information in the origin (x=0,y=0,z=0)
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 vec = vec3();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        constexpr ITK_INLINE vec3() : array{0, 0, 0} {}
        //constexpr ITK_INLINE vec3() : x(0), y(0), z(0) {}
        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components with the same value (by scalar)
        ///
        /// X = v, Y = v and Z = v
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 vec = vec3( 0.5f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to initialize the components
        ///
        ITK_INLINE vec3(const _BaseType &_v) : array{_v, _v, _v} {}

        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components with the same value (by scalar),
        /// converting the input value to the base type when necessary.
        ///
        /// X = v, Y = v and Z = v
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 vec = vec3( 0.5 );
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
        ITK_INLINE vec3(const _InputType &v) : self_type((_BaseType)v) {}

        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from the parameters
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 vec = vec3( 0.1f, 0.2f, 0.3f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the X component of the vector
        /// \param y Value to assign to the Y component of the vector
        /// \param z Value to assign to the Z component of the vector
        ///
        ITK_INLINE vec3(const _BaseType &_x, const _BaseType &_y, const _BaseType &_z) : array{_x, _y, _z} {}

        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from the parameters, converting
        /// each value to the base type when necessary.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 vec = vec3( 0.1, 0.2, 0.3 );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the X component of the vector
        /// \param y Value to assign to the Y component of the vector
        /// \param z Value to assign to the Z component of the vector
        ///
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
        ///
        /// vec3 vec = vec3( vec2( 0.1f, 0.2f ), 0.3f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param xy Vector 2D to assign to the components x and y of the instance respectively
        /// \param z Value to assign to the component z of the instance
        ///
        ITK_INLINE vec3(const vec2_compatible_type &_xy, const _BaseType &_z) : array{_xy.x, _xy.y, _z} {}

        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from a vec2 xy and an isolated z value,
        /// converting each value to the appropriate base type when necessary.
        ///
        /// this->xy = xy <br />
        /// this->z = z
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 vec = vec3( vec2( 0.1f, 0.2f ), 0.3 );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param xy Vector 2D to assign to the components x and y of the instance respectively
        /// \param z Value to assign to the component z of the instance
        ///
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
        ///
        /// vec3 vec = vec3( 0.1f, vec2( 0.2f, 0.3f ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the component x of the instance
        /// \param yz Vector 2D to assign to the components y and z of the instance respectively
        ///
        ITK_INLINE vec3(const _BaseType &_x, const vec2_compatible_type &_yz) : array{_x, _yz.x, _yz.y} {}

        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from an isolated x value and a vec2 yz,
        /// converting each value to the appropriate base type when necessary.
        ///
        /// this->x = x <br />
        /// this->yz = yz
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 vec = vec3( 0.1, vec2( 0.2f, 0.3f ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the component x of the instance
        /// \param yz Vector 2D to assign to the components y and z of the instance respectively
        ///
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

        /// \brief Constructs a tridimensional Vector
        ///
        /// Initialize the vec3 components from another vec3 instance by copy
        ///
        /// Example:
        ///
        /// \code
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
        /// \param v Vector to copy from
        ///
        ITK_INLINE vec3(const self_type &v)
        {
            *this = v;
        }
        /// \brief Assigns the components of another vec3 to this instance
        ///
        /// Copy the X, Y and Z components from another vec3 instance.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 vec_a, vec_b;
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
            return *this;
        }
        // constexpr ITK_INLINE vec3(const self_type& _v) :array{_v.x, _v.y, _v.z} {}
        /// \brief Constructs a tridimensional Vector from the subtraction b-a
        ///
        /// Initialize the vec3 components from two other vectors using the equation: <br />
        /// this = b - a
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 vec_a, vec_b;
        ///
        /// vec3 vec_a_to_b = vec3( vec_a, vec_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Origin vector
        /// \param b Destination vector
        ///
        ITK_INLINE vec3(const self_type &a, const self_type &b) : array{b.x - a.x, b.y - a.y, b.z - a.z} {}
        /// \brief Compare vectors considering #EPSILON (equal)
        ///
        /// Compare two vectors using #EPSILON to see if they are the same.
        ///
        /// Example:
        ///
        /// \code
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
        template <class _Type = _BaseType,
                  typename std::enable_if<
                      std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE bool operator==(const self_type &v) const
        {
            // _BaseType accumulator = _BaseType();
            // for (int i = 0; i < 3; i++)
            //     accumulator += OP<_BaseType>::abs(array[i] - v.array[i]);
            // // accumulator += (std::abs)(array[i] - v.array[i]);
            // return accumulator <= EPSILON<_BaseType>::high_precision;
            bool equal = true;
            for (int i = 0; i < 3; i++)
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
            for (int i = 0; i < 3; i++)
                equal = equal && (array[i] == v.array[i]);
            return equal;
        }

        // inter SIMD types converting...
        /// \brief Assign the components of an instance with a different type.
        ///
        /// Convert the components of another vec3 instance (different base type
        /// and/or SIMD strategy) to this instance's base type and assign them.
        ///
        /// Example:
        ///
        /// \code
        /// vec3<float> obj_f;
        /// vec3<double> obj_d;
        /// obj_d = obj_f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param vec Instance to copy from (converted to the base type).
        /// \return A reference to the current instance after the assignment.
        ///
        template <typename _InputType, typename _InputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          (!std::is_same<_InputSimdTypeAux, _SimdType>::value ||
                           !std::is_same<_InputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE self_type& operator=(const vec3<_InputType, _InputSimdTypeAux> &vec)
        {
            *this = self_type((_BaseType)vec.x, (_BaseType)vec.y, (_BaseType)vec.z);
            return *this;
        }
        // inter SIMD types converting...
        /// \brief Convert the instance to a different vec3 type.
        ///
        /// Implicit conversion operator that converts the components to the
        /// output base type and returns a new instance.
        ///
        /// Example:
        ///
        /// \code
        /// vec3<float> obj_f;
        /// vec3<double> obj_d = obj_f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return An instance with the converted components.
        ///
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
            x += v.x;
            y += v.y;
            z += v.z;
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
            x -= v.x;
            y -= v.y;
            z -= v.z;
            return (*this);
        }
        /// \brief Component-wise minus operator overload
        ///
        /// Negates the vector components with the unary minus operator
        ///
        /// Example:
        ///
        /// \code
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
            return self_type(-x, -y, -z);
        }
        /// \brief Component-wise multiply operator overload
        ///
        /// Multiply the vector by the components of another vector
        ///
        /// Example:
        ///
        /// \code
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
        ITK_INLINE self_type &operator*=(const self_type &v)
        {
            x *= v.x;
            y *= v.y;
            z *= v.z;
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
            x /= v.x;
            y /= v.y;
            z /= v.z;
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
            x += v;
            y += v;
            z += v;
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
            x -= v;
            y -= v;
            z -= v;
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
        /// vec3 vec;
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
            x /= v;
            y /= v;
            z /= v;
            return (*this);
        }
        /// \brief Index the components of the vec3 as a C array
        ///
        /// Example:
        ///
        /// \code
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

        /// \brief Component-wise left shift operator overload (integral types only)
        ///
        /// Shifts the vector components to the left by the given number of bits
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3<int> vec;
        ///
        /// vec <<= 2;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param shift Number of bits to shift the components to the left
        /// \return A reference to the current instance after the shift
        ///
        template <class _Type = _BaseType,typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator<<=(int shift)
        {
            x <<= shift;
            y <<= shift;
            z <<= shift;
            return *this;
        }
        /// \brief Component-wise right shift operator overload (integral types only)
        ///
        /// Shifts the vector components to the right by the given number of bits
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3<int> vec;
        ///
        /// vec >>= 2;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param shift Number of bits to shift the components to the right
        /// \return A reference to the current instance after the shift
        ///
        template <class _Type = _BaseType,typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator>>=(int shift)
        {
            x >>= shift;
            y >>= shift;
            z >>= shift;
            return *this;
        }
        /// \brief Component-wise bitwise AND operator overload (integral types only)
        ///
        /// Apply the bitwise AND between the vector components and the components
        /// of another vector
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3<int> vec, vec_b;
        ///
        /// vec &= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to apply the bitwise AND with the current vector instance
        /// \return A reference to the current instance after the operation
        ///
        template <class _Type = _BaseType,typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator&=(const self_type& v)
        {
            x &= v.x;
            y &= v.y;
            z &= v.z;
            return *this;
        }
        /// \brief Component-wise bitwise OR operator overload (integral types only)
        ///
        /// Apply the bitwise OR between the vector components and the components
        /// of another vector
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3<int> vec, vec_b;
        ///
        /// vec |= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to apply the bitwise OR with the current vector instance
        /// \return A reference to the current instance after the operation
        ///
        template <class _Type = _BaseType,typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator|=(const self_type& v)
        {
            x |= v.x;
            y |= v.y;
            z |= v.z;
            return *this;
        }
        /// \brief Component-wise bitwise XOR operator overload (integral types only)
        ///
        /// Apply the bitwise XOR between the vector components and the components
        /// of another vector
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3<int> vec, vec_b;
        ///
        /// vec ^= vec_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to apply the bitwise XOR with the current vector instance
        /// \return A reference to the current instance after the operation
        ///
        template <class _Type = _BaseType,typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator^=(const self_type& v)
        {
            x ^= v.x;
            y ^= v.y;
            z ^= v.z;
            return *this;
        }
        /// \brief Component-wise bitwise NOT operator overload (integral types only)
        ///
        /// Negates the vector components with the bitwise NOT operator
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3<int> vec;
        ///
        /// vec = ~vec;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A copy of the current instance after the bitwise NOT operation
        ///
        template <class _Type = _BaseType,typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type operator~() const
        {
            return self_type(~x, ~y, ~z);
        }
    };

}
