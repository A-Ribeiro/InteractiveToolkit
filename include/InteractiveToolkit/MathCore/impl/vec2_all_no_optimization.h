#pragma once

#include "vec2_base.h"

namespace MathCore
{
    // #pragma pack(push, AlignBytesReference)

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
    ///
    /// vec2 a, b, result;
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
    class vec2<_BaseType, _SimdType,
               typename std::enable_if<
                   std::is_same<_SimdType, SIMD_TYPE::NONE>::value>::type>
    {
        /// \brief Alias for the fully specialized vec2 type.
        ///
        using self_type = vec2<_BaseType, _SimdType>;

    public:
        /// \brief Number of components stored by the vector (always 2).
        ///
        static constexpr int array_count = 2;
        /// \brief Alias for the vector type itself.
        ///
        using type = self_type;
        /// \brief The scalar type of each component.
        ///
        using element_type = _BaseType;

        /// \brief Union providing multiple views of the two components.
        ///
        /// The components can be accessed as a C array (array),
        /// as named components (x, y) or as size components (width, height).
        ///
        union
        {
            /// \brief The components as a C array (index 0 = x, index 1 = y).
            ///
            _BaseType array[2];
            struct
            {
                /// \brief The X component of the vector.
                ///
                _BaseType x;
                /// \brief The Y component of the vector.
                ///
                _BaseType y;
            };
            struct
            {
                /// \brief The X component viewed as a width value.
                ///
                _BaseType width;
                /// \brief The Y component viewed as a height value.
                ///
                _BaseType height;
            };
        };

        /// \brief Construct a ZERO vec2 class
        ///
        /// The ZERO vec2 class has the point information in the origin (x=0,y=0)
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2 vec = vec2();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        /*ITK_INLINE vec2()
        {
            x = y = _BaseType();
        }*/
        ITK_INLINE vec2() : x(0), y(0) {}
        /// \brief Constructs a bidimensional Vector
        ///
        /// Initialize the vec2 components with the same value (by scalar)
        ///
        /// X = v and Y = v
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2 vec = vec2( 0.5f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to initialize the components
        ///
        ITK_INLINE vec2(const _BaseType &_v) : x(_v), y(_v) {}

        /// \brief Constructs a bidimensional Vector
        ///
        /// Initialize the vec2 components with the same value (by scalar),
        /// converting the input value to the base type when necessary.
        ///
        /// X = v and Y = v
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2 vec = vec2( 0.5 );
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
        ITK_INLINE vec2(const _InputType &v) : self_type((_BaseType)v) {}

        /// \brief Constructs a bidimensional Vector
        ///
        /// Initialize the vec2 components from the parameters
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2 vec = vec2( 0.1f, 0.2f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the X component of the vector
        /// \param y Value to assign to the Y component of the vector
        ///
        ITK_INLINE vec2(const _BaseType &_x, const _BaseType &_y) : x(_x), y(_y) {}

        /// \brief Constructs a bidimensional Vector
        ///
        /// Initialize the vec2 components from the parameters, converting
        /// each value to the base type when necessary.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2 vec = vec2( 0.1, 0.2 );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param x Value to assign to the X component of the vector
        /// \param y Value to assign to the Y component of the vector
        ///
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

        /// \brief Constructs a bidimensional Vector
        ///
        /// Initialize the vec2 components from another vec2 instance by copy
        ///
        /// Example:
        ///
        /// \code
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
        /// \param v Vector to copy from
        ///
        ITK_INLINE vec2(const self_type &v)
        {
            *this = v;
        }
        /// \brief Assigns the components of another vec2 to this instance
        ///
        /// Copy the X and Y components from another vec2 instance.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2 vec_a, vec_b;
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
            return *this;
        }
        /// \brief Constructs a bidimensional Vector from the subtraction b - a
        ///
        /// Initialize the vec2 components from two other vectors using the equation: <br />
        /// this = b - a
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2 vec_a, vec_b;
        ///
        /// vec2 vec_a_to_b = vec2( vec_a, vec_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Origin vector
        /// \param b Destination vector
        ///
        constexpr ITK_INLINE vec2(const self_type &a, const self_type &b) : x(b.x - a.x), y(b.y - a.y) {}
        /// \brief Compare vectors considering #EPSILON (equal)
        ///
        /// Compare two vectors using #EPSILON to see if they are the same.
        ///
        /// Example:
        ///
        /// \code
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
        template <class _Type = _BaseType,
                  typename std::enable_if<
                      std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE bool operator==(const self_type &v) const
        {
            // _BaseType accumulator = _BaseType();
            // for (int i = 0; i < 2; i++)
            //     accumulator += OP<_BaseType>::abs(array[i] - v.array[i]);
            // // accumulator += (std::abs)(array[i] - v.array[i]);
            // return accumulator <= EPSILON<_BaseType>::high_precision;
            bool equal = true;
            for (int i = 0; i < 2; i++)
                equal = equal && OP<float>::compare_almost_equal(array[i], v.array[i]);
            return equal;
        }

        /// \brief Compare vectors (equal) for non floating point types
        ///
        /// Compare two vectors using strict equality (==) on each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2<int> vec_a, vec_b;
        ///
        /// if ( vec_a == vec_b ){
        ///     //do something
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Vector to compare against
        /// \return true if the values are the same
        ///
        template <class _Type = _BaseType,
                  typename std::enable_if<
                  !std::is_floating_point<_Type>::value, bool>::type = true>
                  // std::is_integral<_Type>::value, bool>::type = true>
        ITK_INLINE bool operator==(const self_type &v) const
        {
            bool equal = true;
            for (int i = 0; i < 2; i++)
                equal = equal && (array[i] == v.array[i]);
            return equal;
        }

        /// \brief Assigns the components of a vec2 with a different type/SIMD strategy
        ///
        /// Convert the components of another vec2 instance (different base type
        /// and/or SIMD strategy) to this instance's base type and assign them.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2<float> vec_f;
        /// vec2<double> vec_d;
        ///
        /// vec_d = vec_f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param vec Vector to copy the components from (converted to the base type)
        /// \return A reference to the current instance after the assignment
        ///
        // inter SIMD types converting...
        template <typename _InputType, typename _InputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          (!std::is_same<_InputSimdTypeAux, _SimdType>::value ||
                           !std::is_same<_InputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE self_type& operator=(const vec2<_InputType, _InputSimdTypeAux> &vec)
        {
            *this = self_type((_BaseType)vec.x, (_BaseType)vec.y);
            return *this;
        }
        /// \brief Converts the vec2 to another vec2 with a different type/SIMD strategy
        ///
        /// Implicit conversion operator that converts the components to the
        /// output base type and returns a new vec2 instance.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2<float> vec_f;
        /// vec2<double> vec_d = vec_f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A vec2 instance with the converted components
        ///
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
        /// Compare two vectors using #EPSILON to see if they are different.
        ///
        /// Example:
        ///
        /// \code
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
            x += v.x;
            y += v.y;
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
            x -= v.x;
            y -= v.y;
            return (*this);
        }

        /// \brief Component-wise unary minus (negation) operator overload
        ///
        /// Returns a copy of the vector with all components negated.
        ///
        /// Example:
        ///
        /// \code
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
            return self_type(-x, -y);
        }

        /// \brief Component-wise multiply operator overload
        ///
        /// Multiply the vector by the components of another vector
        ///
        /// Example:
        ///
        /// \code
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
        ITK_INLINE self_type &operator*=(const self_type &v)
        {
            x *= v.x;
            y *= v.y;
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
            x /= v.x;
            y /= v.y;
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
            x += v;
            y += v;
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
            x -= v;
            y -= v;
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
        /// vec2 vec;
        ///
        /// vec *= 0.5f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to multiply all components of the current vector instance
        /// \return A reference to the current instance after the multiply
        ///
        ITK_INLINE self_type &operator*=(const _BaseType &v)
        {
            x *= v;
            y *= v;
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
            x /= v;
            y /= v;
            return (*this);
        }

        /// \brief Index the components of the vec2 as a C array
        ///
        /// Example:
        ///
        /// \code
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


        /// \brief Component-wise left shift operator overload (integral types only)
        ///
        /// Shifts the vector components to the left by the given number of bits
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2<int> vec;
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
        /// vec2<int> vec;
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
        /// vec2<int> vec, vec_b;
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
        /// vec2<int> vec, vec_b;
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
        /// vec2<int> vec, vec_b;
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
            return *this;
        }
        /// \brief Component-wise bitwise NOT operator overload (integral types only)
        ///
        /// Applies the bitwise NOT (~) operator to each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec2<int> vec;
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
            return self_type(~x, ~y);
        }


    };

    // INLINE_OPERATION_IMPLEMENTATION(vec2)

    // #pragma pack(pop)

}
