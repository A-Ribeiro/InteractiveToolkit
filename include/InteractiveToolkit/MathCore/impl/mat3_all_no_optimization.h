#pragma once

#include "../vec3.h"

#include "mat3_base.h"

namespace MathCore
{

    /*
    struct  {
                float _11, _21, _31, _41,
                    _12, _22, _32, _42,
                    _13, _23, _33, _43,
                    _14, _24, _34, _44;
            };
            struct  {
                float a1, a2, a3, a4,
                    b1, b2, b3, b4,
                    c1, c2, c3, c4,
                    d1, d2, d3, d4;
            };
    */

    /// \brief Matrix with 4x4 components
    ///
    /// Matrix definition to work with rigid transformations
    ///
    /// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
    ///
    /// It is possible to use any arithmetic with mat3 and _BaseType Combinations.
    ///
    /// Example:
    ///
    /// \code
            ///
    /// mat3 a, b, result;
    ///
    /// result = ( a * 0.25f + b * 0.75f ) * 2.0f + 1.0f;
    /// \endcode
    ///
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename _BaseType, typename _SimdType>
    class mat3<_BaseType, _SimdType,
               typename std::enable_if<
                   std::is_same<_SimdType, SIMD_TYPE::NONE>::value>::type>
    {
        using self_type = mat3<_BaseType, _SimdType>;
        using vec3_compatible_type = vec3<_BaseType, _SimdType>;

    public:
        static constexpr int rows = 3;
        static constexpr int cols = 3;

        static constexpr int array_count = 9;
        static constexpr int array_stride = 3;

        using type = self_type;
        using element_type = _BaseType;

        union
        {
            struct
            {
                _BaseType a1, a2, a3,
                    b1, b2, b3,
                    c1, c2, c3;
            };
            _BaseType array[9];
            // column-major (OpenGL like matrix byte order)
            //  x  y  z  w
            //  0  4  8 12
            //  1  5  9 13
            //  2  6 10 14
            //  3  7 11 15
        };

        //---------------------------------------------------------------------------
        /// \brief Constructs an identity matrix 4x4
        ///
        /// This construct an identity matrix
        ///
        /// <pre>
        /// | 1 0 0 0 |
        /// | 0 1 0 0 |
        /// | 0 0 1 0 |
        /// | 0 0 0 1 |
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix = mat3();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ITK_INLINE mat3() : array{1, 0, 0,
                                  0, 1, 0,
                                  0, 0, 1} {}
        //---------------------------------------------------------------------------
        /// \brief Constructs a 4x4 matrix
        ///
        /// Initialize all components of the matrix with the same value
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix = mat3( 10.0f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param value Value to initialize the components
        ///
        ITK_INLINE mat3(const _BaseType &v) : array{v, v, v,
                                                    v, v, v,
                                                    v, v, v} {}

        template <typename _InputType,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          !std::is_same<_InputType, _BaseType>::value,
                      bool>::type = true>
        ITK_INLINE mat3(const _InputType &v) : self_type((_BaseType)v) {}
        //---------------------------------------------------------------------------
        /// \brief Constructs a 4x4 matrix
        ///
        /// Initialize the mat3 components from the parameters
        ///
        /// The visual is related to the matrix column major order.
        ///
        /// <pre>
        /// | a1 b1 c1 d1 |
        /// | a2 b2 c2 d2 |
        /// | a3 b3 c3 d3 |
        /// | a4 b4 c4 d4 |
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix = mat3( 1.0f, 0.0f, 0.0f, 0.0f,
        ///                     0.0f, 1.0f, 0.0f, 0.0f,
        ///                     0.0f, 0.0f, 1.0f, 0.0f,
        ///                     0.0f, 0.0f, 0.0f, 1.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ITK_INLINE mat3(const _BaseType &_a1, const _BaseType &_b1, const _BaseType &_c1,
                        const _BaseType &_a2, const _BaseType &_b2, const _BaseType &_c2,
                        const _BaseType &_a3, const _BaseType &_b3, const _BaseType &_c3) : array{_a1, _a2, _a3,
                                                                                                  _b1, _b2, _b3,
                                                                                                  _c1, _c2, _c3} {}

        template <typename __a1, typename __b1, typename __c1,
                  typename __a2, typename __b2, typename __c2,
                  typename __a3, typename __b3, typename __c3,
                  typename std::enable_if<
                      std::is_convertible<__a1, _BaseType>::value &&
                          std::is_convertible<__a2, _BaseType>::value &&
                          std::is_convertible<__a3, _BaseType>::value &&

                          std::is_convertible<__b1, _BaseType>::value &&
                          std::is_convertible<__b2, _BaseType>::value &&
                          std::is_convertible<__b3, _BaseType>::value &&

                          std::is_convertible<__c1, _BaseType>::value &&
                          std::is_convertible<__c2, _BaseType>::value &&
                          std::is_convertible<__c3, _BaseType>::value &&

                          !(std::is_same<__a1, _BaseType>::value && std::is_same<__b1, _BaseType>::value && std::is_same<__c1, _BaseType>::value &&
                            std::is_same<__a2, _BaseType>::value && std::is_same<__b2, _BaseType>::value && std::is_same<__c2, _BaseType>::value &&
                            std::is_same<__a3, _BaseType>::value && std::is_same<__b3, _BaseType>::value && std::is_same<__c3, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE mat3(const __a1 &_a1, const __b1 &_b1, const __c1 &_c1,
                        const __a2 &_a2, const __b2 &_b2, const __c2 &_c2,
                        const __a3 &_a3, const __b3 &_b3, const __c3 &_c3) : self_type((_BaseType)_a1, (_BaseType)_b1, (_BaseType)_c1,
                                                                                       (_BaseType)_a2, (_BaseType)_b2, (_BaseType)_c2,
                                                                                       (_BaseType)_a3, (_BaseType)_b3, (_BaseType)_c3)
        {
        }

        //---------------------------------------------------------------------------
        /// \brief Constructs a 4x4 matrix
        ///
        /// Initialize the mat3 components by copying other mat3 instance
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix_src = mat3( 1.0f, 0.0f, 0.0f, 0.0f,
        ///                         0.0f, 1.0f, 0.0f, 0.0f,
        ///                         0.0f, 0.0f, 1.0f, 0.0f,
        ///                         0.0f, 0.0f, 0.0f, 1.0f);
        ///
        /// mat3 matrix = mat3( matrix_src );
        ///
        /// mat3 matrix_a = matrix_src;
        ///
        /// mat3 matrix_b;
        /// matrix_b = matrix_src;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m Matrix to assign to the instance
        ///
        ITK_INLINE mat3(const self_type &m)
        {
            *this = m;
        }
        ITK_INLINE self_type& operator=(const self_type &m)
        {
            a1 = m.a1;
            a2 = m.a2;
            a3 = m.a3;

            b1 = m.b1;
            b2 = m.b2;
            b3 = m.b3;

            c1 = m.c1;
            c2 = m.c2;
            c3 = m.c3;

            return *this;
        }

        //---------------------------------------------------------------------------
        /// \brief Constructs a 4x4 matrix
        ///
        /// Initialize the mat3 components from vec3 parameters
        ///
        /// \author Alessandro Ribeiro
        /// \param m Matrix to assign to the instance
        ///
        /*ITK_INLINE mat3(const vec3_compatible_type &a, const vec3_compatible_type &b, const vec3_compatible_type &c)
        {
            a1 = a.x;
            a2 = a.y;
            a3 = a.z;

            b1 = b.x;
            b2 = b.y;
            b3 = b.z;

            c1 = c.x;
            c2 = c.y;
            c3 = c.z;
        }*/
        ITK_INLINE mat3(const vec3_compatible_type &a, const vec3_compatible_type &b, const vec3_compatible_type &c) : array{a.x, a.y, a.z,
                                                                                                                             b.x, b.y, b.z,
                                                                                                                             c.x, c.y, c.z} {}

        //---------------------------------------------------------------------------
        /// \brief Matrix multiplication
        ///
        /// Makes the full 4x4 matrix multiplication
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix, other_matrix;
        ///
        /// matrix *= other_matrix;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param M the matrix to be multiplied by the current instance
        /// \return A reference to the multiplied matrix current instance
        ///
        ITK_INLINE self_type &operator*=(const self_type &M)
        {
            _BaseType a, b, c;
            a = a1;
            b = b1;
            c = c1;

            a1 = (a * M.a1 + b * M.a2 + c * M.a3);
            b1 = (a * M.b1 + b * M.b2 + c * M.b3);
            c1 = (a * M.c1 + b * M.c2 + c * M.c3);

            a = a2;
            b = b2;
            c = c2;

            a2 = (a * M.a1 + b * M.a2 + c * M.a3);
            b2 = (a * M.b1 + b * M.b2 + c * M.b3);
            c2 = (a * M.c1 + b * M.c2 + c * M.c3);

            a = a3;
            b = b3;
            c = c3;

            a3 = (a * M.a1 + b * M.a2 + c * M.a3);
            b3 = (a * M.b1 + b * M.b2 + c * M.b3);
            c3 = (a * M.c1 + b * M.c2 + c * M.c3);

            return *this;
        }
        //---------------------------------------------------------------------------
        /// \brief Matrix access based on X (row) and Y (column)
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix;
        ///
        /// matrix(3,0) = 1.0f;
        ///
        /// float v = matrix(3,3);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _row The row to get the element at index
        /// \param _col The column to get the element at index
        /// \return A reference to the matrix element
        ///
        ITK_INLINE _BaseType &operator()(const int _row, const int _col)
        {
            return array[_col * 3 + _row];
        }
        ITK_INLINE const _BaseType &operator()(const int _row, const int _col) const
        {
            return array[_col * 3 + _row];
        }
        //---------------------------------------------------------------------------
        /// \brief Matrix column access based
        ///
        /// Acess one of the 4 columns of the matrix as a vec3 type
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix;
        /// vec3 translate_vec;
        ///
        /// vec3 forward = matrix[2];
        ///
        /// matrix[3] = toPtn4( translate_vec );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _col The column to get
        /// \return A reference to the matrix row as vec3
        ///
        ITK_INLINE vec3_compatible_type &operator[](const int _col)
        {
            return *((vec3_compatible_type *)&array[_col * 3]);
        }

        /// \brief Matrix row access based
        ///
        /// Acess one of the 4 columns of the matrix as a vec3 type
        ///
        /// Example:
        ///
        /// \code
        ///
        /// void process_matrix( const mat3 &matrix ) {
        ///     vec3 forward = matrix[2];
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _col The column to get
        /// \return A reference to the matrix row as vec3
        ///
        ITK_INLINE const vec3_compatible_type &operator[](const int _col) const
        {
            return *((vec3_compatible_type *)&array[_col * 3]);
        }
        //---------------------------------------------------------------------------
        /// \brief Compare two matrix using the #EPSILON constant
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix_a, matrix_b;
        ///
        /// if ( matrix_a == matrix_b ){
        ///     //do something
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The other matrix to compare with
        /// \return true: the matrix is equal, considering the #EPSILON
        ///
        template <class _Type = _BaseType,
                  typename std::enable_if<
                      std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE bool operator==(const self_type &v) const
        {
            // _BaseType accumulator = _BaseType();
            // for (int i = 0; i < 9; i++)
            //     accumulator += OP<_BaseType>::abs(array[i] - v.array[i]);
            // // accumulator += (std::abs)(array[i] - v.array[i]);
            // return accumulator <= EPSILON<_BaseType>::high_precision;
            bool equal = true;
            for (int i = 0; i < 9; i++)
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
            for (int i = 0; i < 9; i++)
                equal = equal && (array[i] == v.array[i]);
            return equal;
        }

        // inter SIMD types converting...
        template <typename _InputType, typename _InputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_InputType, _BaseType>::value &&
                          (!std::is_same<_InputSimdTypeAux, _SimdType>::value ||
                           !std::is_same<_InputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE self_type& operator=(const mat3<_InputType, _InputSimdTypeAux> &m)
        {
            *this = self_type(
                (_BaseType)m.a1, (_BaseType)m.b1, (_BaseType)m.c1,
                (_BaseType)m.a2, (_BaseType)m.b2, (_BaseType)m.c2,
                (_BaseType)m.a3, (_BaseType)m.b3, (_BaseType)m.c3);
            return *this;
        }
        // inter SIMD types converting...
        template <typename _OutputType, typename _OutputSimdTypeAux,
                  typename std::enable_if<
                      std::is_convertible<_BaseType, _OutputType>::value &&
                          !(std::is_same<_OutputSimdTypeAux, _SimdType>::value &&
                            std::is_same<_OutputType, _BaseType>::value),
                      bool>::type = true>
        ITK_INLINE operator mat3<_OutputType, _OutputSimdTypeAux>() const
        {
            return mat3<_OutputType, _OutputSimdTypeAux>(
                (_OutputType)a1, (_OutputType)b1, (_OutputType)c1,
                (_OutputType)a2, (_OutputType)b2, (_OutputType)c2,
                (_OutputType)a3, (_OutputType)b3, (_OutputType)c3);
        }

        /// \brief Compare two matrix using the #EPSILON constant
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix_a, matrix_b;
        ///
        /// if ( matrix_a != matrix_b ){
        ///     //do something
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The other matrix to compare with
        /// \return true: the matrix is not equal, considering the #EPSILON
        ///
        ITK_INLINE bool operator!=(const self_type &v) const
        {
            return !((*this) == v);
        }

        /// \brief Component-wise add elements of the matrix
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix, matrix_b;
        ///
        /// matrix += matrix_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The other matrix used to add values
        /// \return The matrix with the sum result
        ///
        ITK_INLINE self_type &operator+=(const self_type &v)
        {
            a1 += v.a1;
            a2 += v.a2;
            a3 += v.a3;

            b1 += v.b1;
            b2 += v.b2;
            b3 += v.b3;

            c1 += v.c1;
            c2 += v.c2;
            c3 += v.c3;

            return *this;
        }

        /// \brief Component-wise subtract elements of the matrix
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix, matrix_b;
        ///
        /// matrix -= matrix_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The other matrix used to subtract values
        /// \return The matrix with the subtract result
        ///
        ITK_INLINE self_type &operator-=(const self_type &v)
        {
            a1 -= v.a1;
            a2 -= v.a2;
            a3 -= v.a3;

            b1 -= v.b1;
            b2 -= v.b2;
            b3 -= v.b3;

            c1 -= v.c1;
            c2 -= v.c2;
            c3 -= v.c3;

            return *this;
        }

        /// \brief Component-wise change signal
        ///
        /// Change the signal of each element in the matrix
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix;
        ///
        /// matrix = -matrix;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the signal changed
        ///
        ITK_INLINE self_type operator-() const
        {
            return self_type(-a1, -b1, -c1,
                             -a2, -b2, -c2,
                             -a3, -b3, -c3);
        }

        /// \brief Component-wise divide element
        ///
        /// Make the division operation on each element of the matrix
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix, matrix_b;
        ///
        /// matrix /= matrix_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the division result
        ///
        ITK_INLINE self_type &operator/=(const self_type &v)
        {
            (*this) *= v.inverse();
            return *this;
        }

        ITK_INLINE self_type inverse() const
        {
            _BaseType aux1 = c3 * b2 - b3 * c2;
            _BaseType aux2 = b3 * c1 - c3 * b1;
            _BaseType aux3 = c2 * b1 - b2 * c1;

            _BaseType det = (a1 * aux1 + a2 * aux2 + a3 * aux3);

            // check det
            // MATH_CORE_THROW_RUNTIME_ERROR(det == 0, "trying to invert a singular matrix\n");
            _BaseType sign_det = OP<_BaseType>::sign(det);
            det = OP<_BaseType>::maximum(OP<_BaseType>::abs(det), FloatTypeInfo<_BaseType>::min);
            det = sign_det / det;

            return self_type(
                det * aux1, det * aux2, det * aux3,
                det * (a3 * c2 - c3 * a2), det * (c3 * a1 - a3 * c1), det * (a2 * c1 - c2 * a1),
                det * (b3 * a2 - a3 * b2), det * (a3 * b1 - b3 * a1), det * (b2 * a1 - a2 * b1));
        }

        //
        // can be used for inverse
        // transpose rotation+scale mat3 representations
        //
        ITK_INLINE mat3 inverse_transpose_2x2() const
        {
            _BaseType det = (a1 * b2 - b1 * a2);

            // MATH_CORE_THROW_RUNTIME_ERROR(det == 0, "trying to invert a singular matrix\n");

            _BaseType sign_det = OP<_BaseType>::sign(det);
            det = OP<_BaseType>::maximum(OP<_BaseType>::abs(det), FloatTypeInfo<_BaseType>::min);
            det = sign_det / det;

            return self_type(+b2 * det, -a2 * det, 0,
                             -b1 * det, +a1 * det, 0,
                             0, 0, 1);
        }

        /// \brief Add (sum) matrix with a scalar
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix;
        ///
        /// matrix += 5.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the sum of the elements
        ///
        ITK_INLINE self_type &operator+=(const _BaseType &v)
        {
            a1 += v;
            a2 += v;
            a3 += v;

            b1 += v;
            b2 += v;
            b3 += v;

            c1 += v;
            c2 += v;
            c3 += v;

            return *this;
        }

        /// \brief Subtract matrix with a scalar
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix;
        ///
        /// matrix -= 5.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the subtract of the elements
        ///
        ITK_INLINE self_type &operator-=(const _BaseType &v)
        {
            a1 -= v;
            a2 -= v;
            a3 -= v;

            b1 -= v;
            b2 -= v;
            b3 -= v;

            c1 -= v;
            c2 -= v;
            c3 -= v;

            return *this;
        }

        /// \brief Multiply matrix elements with a scalar
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix;
        ///
        /// matrix *= 5.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the multiplication of the elements
        ///
        ITK_INLINE self_type &operator*=(const _BaseType &v)
        {
            a1 *= v;
            a2 *= v;
            a3 *= v;

            b1 *= v;
            b2 *= v;
            b3 *= v;

            c1 *= v;
            c2 *= v;
            c3 *= v;

            return *this;
        }

        /// \brief Divide matrix elements with a scalar
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix;
        ///
        /// matrix /= 5.0f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The matrix with the division of the elements
        ///
        ITK_INLINE self_type &operator/=(const _BaseType &v)
        {
            a1 /= v;
            a2 /= v;
            a3 /= v;

            b1 /= v;
            b2 /= v;
            b3 /= v;

            c1 /= v;
            c2 /= v;
            c3 /= v;

            return *this;
        }

        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator<<=(int shift)
        {
            a1 <<= shift;
            a2 <<= shift;
            a3 <<= shift;

            b1 <<= shift;
            b2 <<= shift;
            b3 <<= shift;

            c1 <<= shift;
            c2 <<= shift;
            c3 <<= shift;

            return *this;
        }
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator>>=(int shift)
        {
            a1 >>= shift;
            a2 >>= shift;
            a3 >>= shift;

            b1 >>= shift;
            b2 >>= shift;
            b3 >>= shift;

            c1 >>= shift;
            c2 >>= shift;
            c3 >>= shift;

            return *this;
        }
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator&=(const _BaseType &v)
        {
            a1 &= v;
            a2 &= v;
            a3 &= v;

            b1 &= v;
            b2 &= v;
            b3 &= v;

            c1 &= v;
            c2 &= v;
            c3 &= v;

            return *this;
        }
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator|=(const _BaseType &v)
        {
            a1 |= v;
            a2 |= v;
            a3 |= v;

            b1 |= v;
            b2 |= v;
            b3 |= v;

            c1 |= v;
            c2 |= v;
            c3 |= v;

            return *this;
        }
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator^=(const _BaseType &v)
        {
            a1 ^= v;
            a2 ^= v;
            a3 ^= v;

            b1 ^= v;
            b2 ^= v;
            b3 ^= v;

            c1 ^= v;
            c2 ^= v;
            c3 ^= v;

            return *this;
        }
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type operator~() const
        {
            return self_type(~a1, ~b1, ~c1,
                             ~a2, ~b2, ~c2,
                             ~a3, ~b3, ~c3);
        }
    };
}
