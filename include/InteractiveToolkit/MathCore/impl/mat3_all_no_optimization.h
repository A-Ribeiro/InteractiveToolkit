#pragma once

#include "../vec3.h"

#include "mat3_base.h"

namespace MathCore
{

    /// \brief Matrix with 3x3 components
    ///
    /// Matrix definition to work with rigid transformations
    ///
    /// The arithmetic operations are available through #INLINE_OPERATION_IMPLEMENTATION
    ///
    /// It is possible to use any arithmetic with mat3 and _BaseType combinations.
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
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _BaseType The scalar type of each component (e.g., float, double).
    /// \tparam _SimdType The SIMD strategy used for the matrix; this specialization
    ///         is selected when _SimdType is SIMD_TYPE::NONE (no SIMD optimization).
    ///
    template <typename _BaseType, typename _SimdType>
    class mat3<_BaseType, _SimdType,
               typename std::enable_if<
                   std::is_same<_SimdType, SIMD_TYPE::NONE>::value>::type>
    {
        /// \brief Alias for the fully specialized mat3 type.
        ///
        using self_type = mat3<_BaseType, _SimdType>;
        // force set vec3 to normal operation...
        using vec3_compatible_type = vec3<_BaseType, _SimdType>;

    public:
        /// \brief Number of rows of the matrix (always 3).
        ///
        static constexpr int rows = 3;
        /// \brief Number of columns of the matrix (always 3).
        ///
        static constexpr int cols = 3;

        /// \brief Total number of components stored by the matrix (always 9).
        ///
        static constexpr int array_count = 9;
        /// \brief Number of components per column (always 3).
        ///
        static constexpr int array_stride = 3;

        /// \brief Alias for the matrix type itself.
        ///
        using type = self_type;
        /// \brief The scalar type of each component.
        ///
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
        /// \brief Constructs an identity matrix 3x3
        ///
        /// This constructs an identity matrix
        ///
        /// <pre>
        /// | 1 0 0 |
        /// | 0 1 0 |
        /// | 0 0 1 |
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
        /// \brief Constructs a 3x3 matrix
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
        /// \param v Value to initialize the components
        ///
        ITK_INLINE mat3(const _BaseType &v) : array{v, v, v,
                                                    v, v, v,
                                                    v, v, v} {}

        /// \brief Constructs a 3x3 matrix
        ///
        /// Initialize all components of the matrix with the same value,
        /// converting the input value to the base type when necessary.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix = mat3( 10.0 );
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
        ITK_INLINE mat3(const _InputType &v) : self_type((_BaseType)v) {}
        //---------------------------------------------------------------------------
        /// \brief Constructs a 3x3 matrix
        ///
        /// Initialize the mat3 components from the parameters
        ///
        /// The visual is related to the matrix column major order.
        ///
        /// <pre>
        /// | a1 b1 c1 |
        /// | a2 b2 c2 |
        /// | a3 b3 c3 |
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix = mat3( 1.0f, 0.0f, 0.0f,
        ///                     0.0f, 1.0f, 0.0f,
        ///                     0.0f, 0.0f, 1.0f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _a1 Value to assign to the a1 component (row 0, column 0)
        /// \param _b1 Value to assign to the b1 component (row 0, column 1)
        /// \param _c1 Value to assign to the c1 component (row 0, column 2)
        /// \param _a2 Value to assign to the a2 component (row 1, column 0)
        /// \param _b2 Value to assign to the b2 component (row 1, column 1)
        /// \param _c2 Value to assign to the c2 component (row 1, column 2)
        /// \param _a3 Value to assign to the a3 component (row 2, column 0)
        /// \param _b3 Value to assign to the b3 component (row 2, column 1)
        /// \param _c3 Value to assign to the c3 component (row 2, column 2)
        ///
        ITK_INLINE mat3(const _BaseType &_a1, const _BaseType &_b1, const _BaseType &_c1,
                        const _BaseType &_a2, const _BaseType &_b2, const _BaseType &_c2,
                        const _BaseType &_a3, const _BaseType &_b3, const _BaseType &_c3) : array{_a1, _a2, _a3,
                                                                                                  _b1, _b2, _b3,
                                                                                                  _c1, _c2, _c3} {}

        /// \brief Constructs a 3x3 matrix
        ///
        /// Initialize the mat3 components from the parameters, converting
        /// each value to the base type when necessary.
        ///
        /// The visual is related to the matrix column major order.
        ///
        /// <pre>
        /// | a1 b1 c1 |
        /// | a2 b2 c2 |
        /// | a3 b3 c3 |
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix = mat3( 1.0, 0.0, 0.0,
        ///                     0.0, 1.0, 0.0,
        ///                     0.0, 0.0, 1.0 );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _a1 Value to assign to the a1 component (row 0, column 0)
        /// \param _b1 Value to assign to the b1 component (row 0, column 1)
        /// \param _c1 Value to assign to the c1 component (row 0, column 2)
        /// \param _a2 Value to assign to the a2 component (row 1, column 0)
        /// \param _b2 Value to assign to the b2 component (row 1, column 1)
        /// \param _c2 Value to assign to the c2 component (row 1, column 2)
        /// \param _a3 Value to assign to the a3 component (row 2, column 0)
        /// \param _b3 Value to assign to the b3 component (row 2, column 1)
        /// \param _c3 Value to assign to the c3 component (row 2, column 2)
        ///
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
        /// \brief Constructs a 3x3 matrix
        ///
        /// Initialize the mat3 components by copying other mat3 instance
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix_src = mat3( 1.0f, 0.0f, 0.0f,
        ///                         0.0f, 1.0f, 0.0f,
        ///                         0.0f, 0.0f, 1.0f );
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
        /// \param m Matrix to copy from
        ///
        ITK_INLINE mat3(const self_type &m)
        {
            *this = m;
        }
        /// \brief Assigns the components of another mat3 to this instance
        ///
        /// Copy the a1, a2, a3, b1, b2, b3, c1, c2 and c3 components
        /// from another mat3 instance.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix_a, matrix_b;
        ///
        /// matrix_a = matrix_b;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m Matrix to copy the components from
        /// \return A reference to the current instance after the assignment
        ///
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
        /// \brief Constructs a 3x3 matrix
        ///
        /// Initialize the mat3 components from vec3 parameters
        ///
        /// The first vec3 fills the first column, the second vec3 fills the
        /// second column and the third vec3 fills the third column of the matrix.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 col_a( 1.0f, 0.0f, 0.0f );
        /// vec3 col_b( 0.0f, 1.0f, 0.0f );
        /// vec3 col_c( 0.0f, 0.0f, 1.0f );
        ///
        /// mat3 matrix = mat3( col_a, col_b, col_c );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a vec3 used to fill the first column of the matrix
        /// \param b vec3 used to fill the second column of the matrix
        /// \param c vec3 used to fill the third column of the matrix
        ///
        ITK_INLINE mat3(const vec3_compatible_type &a, const vec3_compatible_type &b, const vec3_compatible_type &c) : array{a.x, a.y, a.z,
                                                                                                                             b.x, b.y, b.z,
                                                                                                                             c.x, c.y, c.z} {}

        //---------------------------------------------------------------------------
        /// \brief Matrix multiplication
        ///
        /// Makes the full 3x3 matrix multiplication
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
        /// matrix(1,0) = 1.0f;
        ///
        /// float v = matrix(2,2);
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
        /// \brief Matrix access based on X (row) and Y (column)
        ///
        /// Example:
        ///
        /// \code
        ///
        /// const mat3 matrix;
        ///
        /// float v = matrix(1,1);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _row The row to get the element at index
        /// \param _col The column to get the element at index
        /// \return A reference to the matrix element
        ///
        ITK_INLINE const _BaseType &operator()(const int _row, const int _col) const
        {
            return array[_col * 3 + _row];
        }
        //---------------------------------------------------------------------------
        /// \brief Matrix column access based
        ///
        /// Access one of the 3 columns of the matrix as a vec3 type
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
        /// matrix[1] = translate_vec;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _col The column to get
        /// \return A reference to the matrix column as vec3
        ///
        ITK_INLINE vec3_compatible_type &operator[](const int _col)
        {
            return *((vec3_compatible_type *)&array[_col * 3]);
        }

        /// \brief Matrix column access based
        ///
        /// Access one of the 3 columns of the matrix as a vec3 type
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
        /// \return A reference to the matrix column as vec3
        ///
        ITK_INLINE const vec3_compatible_type &operator[](const int _col) const
        {
            return *((vec3_compatible_type *)&array[_col * 3]);
        }
        //---------------------------------------------------------------------------
        /// \brief Compare matrices considering #EPSILON (equal)
        ///
        /// Compare two matrices using #EPSILON to see if they are the same.
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
        /// \param v Matrix to compare against
        /// \return true if the values are the same considering #EPSILON
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

        /// \brief Compare matrices (equal) for non floating point types
        ///
        /// Compare two matrices using strict equality (==) on each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3<int> matrix_a, matrix_b;
        ///
        /// if ( matrix_a == matrix_b ){
        ///     //do something
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Matrix to compare against
        /// \return true if the values are the same
        ///
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

        /// \brief Assigns the components of a mat3 with a different type/SIMD strategy
        ///
        /// Convert the components of another mat3 instance (different base type
        /// and/or SIMD strategy) to this instance's base type and assign them.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3<float> mat_f;
        /// mat3<double> mat_d;
        ///
        /// mat_d = mat_f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m Matrix to copy the components from (converted to the base type)
        /// \return A reference to the current instance after the assignment
        ///
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
        /// \brief Converts the mat3 to another mat3 with a different type/SIMD strategy
        ///
        /// Implicit conversion operator that converts the components to the
        /// output base type and returns a new mat3 instance.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3<float> mat_f;
        /// mat3<double> mat_d = mat_f;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A mat3 instance with the converted components
        ///
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

        /// \brief Compare matrices considering #EPSILON (not equal)
        ///
        /// Compare two matrices using #EPSILON to see if they are different.
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
        /// \param v Matrix to compare against
        /// \return true if the values are not the same considering #EPSILON
        ///
        ITK_INLINE bool operator!=(const self_type &v) const
        {
            return !((*this) == v);
        }

        /// \brief Component-wise sum (add) operator overload
        ///
        /// Increment the matrix by the components of another matrix
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
        /// \param v Matrix to increment the current matrix instance
        /// \return A reference to the current instance after the increment
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

        /// \brief Component-wise subtract operator overload
        ///
        /// Decrement the matrix by the components of another matrix
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
        /// \param v Matrix to decrement the current matrix instance
        /// \return A reference to the current instance after the decrement
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

        /// \brief Component-wise unary minus (negation) operator overload
        ///
        /// Returns a copy of the matrix with all components negated.
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
        /// \return A copy of the current instance after the negation operation
        ///
        ITK_INLINE self_type operator-() const
        {
            return self_type(-a1, -b1, -c1,
                             -a2, -b2, -c2,
                             -a3, -b3, -c3);
        }

        /// \brief Component-wise divide operator overload
        ///
        /// Divide the matrix by the components of another matrix
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
        /// \param v Matrix to divide the current matrix instance
        /// \return A reference to the current instance after the division
        ///
        ITK_INLINE self_type &operator/=(const self_type &v)
        {
            (*this) *= v.inverse();
            return *this;
        }

        /// \brief Compute the inverse of the matrix
        ///
        /// Returns the inverse matrix using the determinant.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix;
        ///
        /// mat3 inv = matrix.inverse();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The inverse of the current matrix
        ///
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

        /// \brief Compute the inverse transpose of the 2x2 sub-matrix
        ///
        /// Returns the inverse transpose of the upper-left 2x2 sub-matrix,
        /// keeping the third row and column as the identity. This is useful
        /// for inverting rotation+scale mat3 representations.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 matrix;
        ///
        /// mat3 inv = matrix.inverse_transpose_2x2();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return The inverse transpose of the 2x2 sub-matrix
        ///
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

        /// \brief Single value increment (add, sum) operator overload
        ///
        /// Increment the matrix components by a single value (scalar)
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
        /// \param v Value to increment all components of the current matrix instance
        /// \return A reference to the current instance after the increment
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

        /// \brief Single value decrement (subtract) operator overload
        ///
        /// Decrement the matrix components by a single value (scalar)
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
        /// \param v Value to decrement all components of the current matrix instance
        /// \return A reference to the current instance after the decrement
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

        /// \brief Single value multiply operator overload
        ///
        /// Multiply the matrix components by a single value (scalar)
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
        /// \param v Value to multiply all components of the current matrix instance
        /// \return A reference to the current instance after the multiply
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

        /// \brief Single value division operator overload
        ///
        /// Divides the matrix components by a single value (scalar)
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
        /// \param v Value to divide all components of the current matrix instance
        /// \return A reference to the current instance after the division
        ///
        template <class _Type = _BaseType, typename std::enable_if<std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type &operator/=(const _BaseType &v)
        {
            _BaseType factor = _BaseType(1) / v;
            a1 *= factor;
            a2 *= factor;
            a3 *= factor;
            b1 *= factor;
            b2 *= factor;
            b3 *= factor;
            c1 *= factor;
            c2 *= factor;
            c3 *= factor;
            return *this;
        }
        /// \brief Single value division operator overload (integral types only)
        ///
        /// Divides the matrix components by a single value (scalar)
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
        /// \param v Value to divide all components of the current matrix instance
        /// \return A reference to the current instance after the division
        ///
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
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

        /// \brief Component-wise left shift operator overload (integral types only)
        ///
        /// Shifts the matrix components to the left by the given number of bits
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3<int> matrix;
        ///
        /// matrix <<= 2;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param shift Number of bits to shift the components to the left
        /// \return A reference to the current instance after the shift
        ///
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
        /// \brief Component-wise right shift operator overload (integral types only)
        ///
        /// Shifts the matrix components to the right by the given number of bits
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3<int> matrix;
        ///
        /// matrix >>= 2;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param shift Number of bits to shift the components to the right
        /// \return A reference to the current instance after the shift
        ///
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
        /// \brief Component-wise bitwise AND operator overload (integral types only)
        ///
        /// Apply the bitwise AND between the matrix components and a single value
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3<int> matrix;
        ///
        /// matrix &= 0xFF;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to apply the bitwise AND with the current matrix instance
        /// \return A reference to the current instance after the operation
        ///
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
        /// \brief Component-wise bitwise OR operator overload (integral types only)
        ///
        /// Apply the bitwise OR between the matrix components and a single value
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3<int> matrix;
        ///
        /// matrix |= 0xFF;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to apply the bitwise OR with the current matrix instance
        /// \return A reference to the current instance after the operation
        ///
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
        /// \brief Component-wise bitwise XOR operator overload (integral types only)
        ///
        /// Apply the bitwise XOR between the matrix components and a single value
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3<int> matrix;
        ///
        /// matrix ^= 0xFF;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v Value to apply the bitwise XOR with the current matrix instance
        /// \return A reference to the current instance after the operation
        ///
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
        /// \brief Component-wise bitwise NOT (complement) operator overload (integral types only)
        ///
        /// Returns a copy of the matrix with all components bit-inverted.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3<int> matrix;
        ///
        /// matrix = ~matrix;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \return A copy of the current instance after the complement operation
        ///
        template <class _Type = _BaseType, typename std::enable_if<!std::is_floating_point<_Type>::value, bool>::type = true>
        ITK_INLINE self_type operator~() const
        {
            return self_type(~a1, ~b1, ~c1,
                             ~a2, ~b2, ~c2,
                             ~a3, ~b3, ~c3);
        }
    };
}
