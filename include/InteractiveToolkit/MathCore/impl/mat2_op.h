#pragma once

#include "mat2_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

#include "quat_op.h"

namespace MathCore
{

    /// \brief Generic operations specialization for mat2 with no SIMD optimization.
    ///
    /// Provides generic (non-SIMD) utility functions for the mat2 class when
    /// SIMD optimizations are disabled (SIMD_TYPE::NONE). This specialization
    /// is selected via SFINAE when the _simd template parameter matches
    /// SIMD_TYPE::NONE.
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _type The scalar type of the mat2 components (e.g., float, double).
    /// \tparam _simd The SIMD strategy type; this specialization is selected when
    ///         _simd is SIMD_TYPE::NONE.
    /// \tparam _algorithm The algorithm type.
    ///
    template <typename _type, typename _simd, typename _algorithm>
    struct OP<mat2<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value>::type,
              _algorithm>
    {
    private:
        /// \brief Alias for the 2x2 matrix type.
        ///
        using typeMat2 = mat2<_type, _simd>;
        /// \brief Alias for the 2-component vector type.
        ///
        using type2 = vec2<_type, _simd>;
        /// \brief Alias for the fully specialized OP struct type.
        ///
        using self_type = OP<typeMat2>;

    public:
        /// \brief Returns the next representable floating-point value after each component.
        ///
        /// For each component of each row, returns the next floating-point value
        /// in the direction of positive infinity.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a = mat2( 1.0f, 2.0f, 3.0f, 4.0f );
        ///
        /// mat2 result = next( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input matrix
        /// \return The next representable value for each component
        ///
        static ITK_INLINE typeMat2 next(const typeMat2 &p) noexcept
        {
            return typeMat2(
                OP<type2>::next(p[0]),
                OP<type2>::next(p[1]));
        }

        /// \brief Returns the previous representable floating-point value before each component.
        ///
        /// For each component of each row, returns the previous floating-point value
        /// in the direction of negative infinity.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a = mat2( 1.0f, 2.0f, 3.0f, 4.0f );
        ///
        /// mat2 result = previous( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input matrix
        /// \return The previous representable value for each component
        ///
        static ITK_INLINE typeMat2 previous(const typeMat2 &p) noexcept
        {
            return typeMat2(
                OP<type2>::previous(p[0]),
                OP<type2>::previous(p[1]));
        }

        /// \brief Returns the next representable floating-point value after each component in the direction of a target.
        ///
        /// For each component of each row, returns the next floating-point value after p
        /// in the direction of _to.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a = mat2( 1.0f, 2.0f, 3.0f, 4.0f );
        /// mat2 target = mat2( 5.0f, 0.0f, 1.0f, 6.0f );
        ///
        /// mat2 result = next_after( a, target );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input matrix
        /// \param _to The target direction matrix
        /// \return The next representable value for each component in the direction of _to
        ///
        static ITK_INLINE typeMat2 next_after(const typeMat2 &p, const typeMat2 &_to) noexcept
        {
            return typeMat2(
                OP<type2>::next_after(p[0], _to[0]),
                OP<type2>::next_after(p[1], _to[1]));
        }

        /// \brief Clamp values in a component-wise fashion
        ///
        /// For each component of the matrix, evaluate:
        /// ```
        ///     if min < value then return min
        ///     if max > value then return max
        ///     else return value
        /// ```
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 result;
        /// // result = mat2( 50, 3, 50, 3 )
        /// result = clamp( mat2( 300, 3, 300, 3 ), mat2( 0, -1, 0, -1 ), mat2( 50, 5, 50, 5 ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param value The value to evaluate
        /// \param min The min threshold
        /// \param max The max threshold
        /// \return The evaluated value
        ///
        static ITK_INLINE typeMat2 clamp(const typeMat2 &value, const typeMat2 &min, const typeMat2 &max) noexcept
        {
            return typeMat2(
                OP<type2>::clamp(value[0], min[0], max[0]),
                OP<type2>::clamp(value[1], min[1], max[1]));
        }

        /// \brief Computes the dot product between two matrices
        ///
        /// The dot product is a single value computed from the two matrices,
        /// by summing the dot product of each corresponding row.
        ///
        /// <pre>
        /// dot(a, b) = dot(a[0], b[0]) + dot(a[1], b[1])
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a, b;
        /// float result = dot( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \return The dot product between the two matrices
        ///
        static ITK_INLINE _type dot(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            _type dota = OP<type2>::dot(a[0], b[0]);
            _type dotb = OP<type2>::dot(a[1], b[1]);
            return dota + dotb;
        }

        /// \brief Normalize a matrix
        ///
        /// Returns a matrix with unit magnitude in the same "direction" of the parameter.
        ///
        /// result = mat/|mat|
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a;
        ///
        /// mat2 a_normalized = normalize( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The matrix to normalize
        /// \return The unit matrix
        ///
        static ITK_INLINE typeMat2 normalize(const typeMat2 &m) noexcept
        {
            _type mag2 = self_type::dot(m, m);
            _type mag2_rsqrt = OP<_type, void, _algorithm>::rsqrt(mag2);
            return m * mag2_rsqrt;
        }

        /// \brief Computes the squared length of a matrix
        ///
        /// The squared length of a matrix 'a' is:
        ///
        /// |a|^2
        ///
        /// It is cheaper to compute this value than the length of 'a'.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 input;
        ///
        /// float result = sqrLength(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The matrix
        /// \return The squared length
        ///
        static ITK_INLINE _type sqrLength(const typeMat2 &a) noexcept
        {
            return self_type::dot(a, a);
        }

        /// \brief Computes the length of a matrix
        ///
        /// The length of a matrix 'a' is:
        ///
        /// |a|
        ///
        /// This computation uses the sqrtf, and it consumes a lot of cycles to compute.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 input;
        ///
        /// float result = length(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The matrix
        /// \return The length
        ///
        static ITK_INLINE _type length(const typeMat2 &a) noexcept
        {
            return OP<_type>::sqrt(self_type::dot(a, a));
        }

        /// \brief Computes the squared distance between two matrices
        ///
        /// The squared distance is the Euclidean distance, without the square root:
        ///
        /// |b-a|^2
        ///
        /// It is cheaper to compute this value than the distance from 'a' to 'b'.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a, b;
        ///
        /// float result = sqrDistance( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \return The squared distance between a and b
        ///
        static ITK_INLINE _type sqrDistance(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            typeMat2 ab = b - a;
            return self_type::dot(ab, ab);
        }

        /// \brief Computes the distance between two matrices
        ///
        /// The distance is the Euclidean distance:
        ///
        /// |b-a|
        ///
        /// This computation uses the sqrtf, and it consumes a lot of cycles to compute.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a, b;
        ///
        /// float result = distance( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \return The distance between a and b
        ///
        static ITK_INLINE _type distance(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            typeMat2 ab = b - a;
            return OP<_type>::sqrt(self_type::dot(ab, ab));
        }

        /// \brief Return the greater value from the parameter
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 input;
        ///
        /// float max = maximum( input );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Set of values to test
        /// \return The greater value from the parameter
        ///
        static ITK_INLINE _type maximum(const typeMat2 &a) noexcept
        {
            type2 max_a = OP<type2>::maximum(a[0], a[1]);
            return OP<type2>::maximum(max_a);
        }

        /// \brief Component-wise maximum value from two matrices
        ///
        /// Return the maximum value considering each component of the matrix.
        ///
        /// result: mat2( maximum(a[0],b[0]), maximum(a[1],b[1]) )
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a, b;
        ///
        /// mat2 result = maximum( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A matrix
        /// \param b A matrix
        /// \return The maximum value for each matrix component
        ///
        static ITK_INLINE typeMat2 maximum(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            return typeMat2(OP<type2>::maximum(a[0], b[0]),
                            OP<type2>::maximum(a[1], b[1]));
        }

        /// \brief Return the smaller value from the parameter
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a;
        ///
        /// float result = minimum( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Set of values to test
        /// \return The smaller value from the parameter
        ///
        static ITK_INLINE _type minimum(const typeMat2 &a) noexcept
        {
            type2 max_a = OP<type2>::minimum(a[0], a[1]);
            return OP<type2>::minimum(max_a);
        }

        /// \brief Component-wise minimum value from two matrices
        ///
        /// Return the minimum value considering each component of the matrix.
        ///
        /// result: mat2( minimum(a[0],b[0]), minimum(a[1],b[1]) )
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a, b;
        ///
        /// mat2 result = minimum( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A matrix
        /// \param b A matrix
        /// \return The minimum value for each matrix component
        ///
        static ITK_INLINE typeMat2 minimum(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            return typeMat2(OP<type2>::minimum(a[0], b[0]),
                            OP<type2>::minimum(a[1], b[1]));
        }

        /// \brief Compute the absolute value of a matrix (magnitude)
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 input = mat2( -10, 20, -30, 40 );
        ///
        /// // result = mat2( 10, 20, 30, 40 )
        /// mat2 result = abs( input );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A matrix
        /// \return mat2( |a[0]|, |a[1]| )
        ///
        static ITK_INLINE typeMat2 abs(const typeMat2 &a) noexcept
        {
            return typeMat2(OP<type2>::abs(a[0]),
                            OP<type2>::abs(a[1]));
        }

        /// \brief Computes the linear interpolation
        ///
        /// When the factor is between 0 and 1, it returns the convex relation (linear interpolation) between a and b.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a = mat2( 0.0f, 0.0f, 0.0f, 0.0f );
        /// mat2 b = mat2( 100.0f, 100.0f, 100.0f, 100.0f );
        ///
        /// // result = mat2( 75.0f, 75.0f, 75.0f, 75.0f )
        /// mat2 result = lerp( a, b, 0.75f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Origin Matrix
        /// \param b Target Matrix
        /// \param factor The amount (%) to leave the Origin to the Target.
        /// \return The interpolation result
        ///
        static ITK_INLINE typeMat2 lerp(const typeMat2 &a, const typeMat2 &b, const _type &factor) noexcept
        {
            //  return a+(b-a)*factor;
            return a * ((_type)1 - factor) + (b * factor);
        }

        /// \brief Computes the result of the interpolation based on the baricentric coordinate (uv) considering 3 points
        ///
        /// It is possible to discover the value of 'u' and 'v' by using the triangle area formula.
        ///
        /// After that it is possible to use this function to interpolate normals, colors, etc... based on the baricentric coordinate uv
        ///
        /// Note: If the uv were calculated in Euclidean space of a triangle, then interpolation of colors, normals or coordinates are not affected by the perspective projection.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// // point inside the triangle
        /// vec3 p;
        ///
        /// // triangle vertex
        /// vec3 a, b, c;
        ///
        /// vec3 crossvec = cross(b-a, c-a);
        /// vec3 cross_unit = normalize( crossvec );
        /// float signed_triangle_area = dot( crossvec, cross_unit ) * 0.5f;
        ///
        /// crossvec = cross(c-a, c-p);
        ///
        /// float u = ( dot( crossvec, cross_unit )  * 0.5f ) / signed_triangle_area;
        ///
        /// crossvec = cross(b-a, p-b);
        ///
        /// float v = ( dot( crossvec, cross_unit )  * 0.5f ) / signed_triangle_area;
        ///
        /// // now the color we want to interpolate
        /// mat2 colorA, colorB, colorC;
        ///
        /// mat2 colorResult = barylerp(u, v, colorA, colorB, colorC);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param u The u component of a baricentric coord
        /// \param v The v component of a baricentric coord
        /// \param v0 The first matrix to interpolate
        /// \param v1 The second matrix to interpolate
        /// \param v2 The third matrix to interpolate
        /// \return A matrix interpolated based on uv considering the 3 matrices of the parameter
        ///
        static ITK_INLINE typeMat2 barylerp(const _type &u, const _type &v, const typeMat2 &v0, const typeMat2 &v1, const typeMat2 &v2) noexcept
        {
            // return v0*(1-uv[0]-uv[1])+v1*uv[0]+v2*uv[1];
            return v0 * ((_type)1 - u - v) + v1 * u + v2 * v;
        }

        /// \brief Computes the result of the bilinear interpolation over a square patch with 4 points
        ///
        /// The bilinear interpolation is useful to compute colors between pixels in an image.
        ///
        /// This implementation considers that the square formed by the four points is a square.
        ///
        /// If you try to interpolate values of a non square area, you will have a result, but it might be weird.
        ///
        /// <pre>
        /// dx - [0..1]
        /// dy - [0..1]
        ///
        ///  D-f(0,1) ---*----- C-f(1,1)
        ///     |        |         |
        ///     |        |         |
        /// .   *--------P---------*   P = (dx,dy)
        ///     |        |         |
        ///     |        |         |
        ///  A-f(0,0) ---*----- B-f(1,0)
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 dataA = mat2(0,0,0,0);
        /// mat2 dataB = mat2(1,0,1,0);
        /// mat2 dataC = mat2(1,1,1,1);
        /// mat2 dataD = mat2(0,1,0,1);
        ///
        /// // result = mat2( 0.5f, 0.5f, 0.5f, 0.5f )
        /// mat2 result = blerp(dataA,dataB,dataC,dataD,0.5f,0.5f);
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param A The lower-left matrix
        /// \param B The lower-right matrix
        /// \param C The upper-right matrix
        /// \param D The upper-left matrix
        /// \param dx The x axis interpolation factor
        /// \param dy The y axis interpolation factor
        /// \return A matrix interpolated based on dxdy considering the 4 matrices of the parameter
        ///
        static ITK_INLINE typeMat2 blerp(const typeMat2 &A, const typeMat2 &B, const typeMat2 &C, const typeMat2 &D,
                                         const _type &dx, const _type &dy) noexcept
        {
            _type omdx = (_type)1 - dx,
                  omdy = (_type)1 - dy;
            return (omdx * omdy) * A + (omdx * dy) * D + (dx * omdy) * B + (dx * dy) * C;
        }

        /// \brief Extracts the X axis (first row) of a matrix
        ///
        /// Returns the first row of the matrix as a vec2, which represents
        /// the X axis of the transformation.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 m = mat2( 1.0f, 2.0f, 3.0f, 4.0f );
        ///
        /// // result = vec2( 1.0f, 2.0f )
        /// vec2 xAxis = extractXaxis( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The X axis (first row) of the matrix
        ///
        static ITK_INLINE type2 extractXaxis(const typeMat2 &m) noexcept
        {
            return m[0];
        }

        /// \brief Extracts the Y axis (second row) of a matrix
        ///
        /// Returns the second row of the matrix as a vec2, which represents
        /// the Y axis of the transformation.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 m = mat2( 1.0f, 2.0f, 3.0f, 4.0f );
        ///
        /// // result = vec2( 3.0f, 4.0f )
        /// vec2 yAxis = extractYaxis( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The Y axis (second row) of the matrix
        ///
        static ITK_INLINE type2 extractYaxis(const typeMat2 &m) noexcept
        {
            return m[1];
        }

        /// \brief Computes the transpose of a matrix
        ///
        /// The transpose of a matrix is obtained by swapping its rows and columns.
        ///
        /// <pre>
        ///     | a1  a2 |       | a1  b1 |
        /// m = | b1  b2 |  =>  | a2  b2 |
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 m = mat2( 1.0f, 2.0f, 3.0f, 4.0f );
        ///
        /// // result = mat2( 1.0f, 3.0f, 2.0f, 4.0f )
        /// mat2 result = transpose( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The transposed matrix
        ///
        static ITK_INLINE typeMat2 transpose(const typeMat2 &m) noexcept
        {
            return typeMat2(m.a1, m.a2,
                            m.b1, m.b2);
        }

        /// \brief Computes the determinant of a matrix
        ///
        /// The determinant of a 2x2 matrix is:
        ///
        /// <pre>
        ///     | a1  a2 |
        /// det = | b1  b2 | = a1 * b2 - b1 * a2
        /// </pre>
        ///
        /// The determinant can be used to determine if a matrix is invertible
        /// (non-zero determinant) and to compute the inverse.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 m = mat2( 1.0f, 2.0f, 3.0f, 4.0f );
        ///
        /// // result = 1.0f * 4.0f - 3.0f * 2.0f = -2.0f
        /// float det = determinant( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The determinant of the matrix
        ///
        static ITK_INLINE _type determinant(const typeMat2 &m) noexcept
        {
            return (m.a1 * m.b2 - m.b1 * m.a2);
        }

        /// \brief Move from current to target, considering the max variation
        ///
        /// This function could be used as a constant motion interpolation<br />
        /// between two values considering the delta time and max speed variation.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// PlatformTime timer;
        /// float moveSpeed;
        /// mat2 current;
        /// mat2 target;
        ///
        /// {
        ///     timer.update();
        ///     ...
        ///     // current will be modified to be the target,
        ///     // but the delta time and move speed will make
        ///     // this transition smoother.
        ///     current = move( current, target, time.deltaTime * moveSpeed );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param current The current state
        /// \param target The target state
        /// \param maxDistanceVariation The max amount the current can be modified to reach target
        /// \return the lerp from current to target according max variation
        ///
        static ITK_INLINE typeMat2 move(const typeMat2 &current, const typeMat2 &target, const _type &maxDistanceVariation) noexcept
        {
            _type deltaDistance = self_type::distance(current, target);

            deltaDistance = OP<_type>::maximum(deltaDistance, maxDistanceVariation);
            // avoid division by zero
            // deltaDistance = self_type::maximum(deltaDistance, EPSILON<float>::high_precision);
            deltaDistance = OP<_type>::maximum(deltaDistance, FloatTypeInfo<_type>::min);
            return self_type::lerp(current, target, maxDistanceVariation / deltaDistance);

            // if (deltaDistance < maxDistanceVariation + EPSILON<_type>::high_precision)
            //     return target;
            // return lerp(current, target, maxDistanceVariation / deltaDistance);
        }

        /// \brief The sign of each component. ( v >= 0 ) ? 1 : -1
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a = mat2( -10, 20, -30, 40 );
        ///
        /// // result = mat2( -1, 1, -1, 1 )
        /// mat2 result = sign( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The sign of each component
        ///
        static ITK_INLINE typeMat2 sign(const typeMat2 &m) noexcept
        {
            return typeMat2(
                OP<type2>::sign(m[0]),
                OP<type2>::sign(m[1]));
        }

        /// \brief The floor of each component.
        ///
        /// Returns the largest integer value less than or equal to each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a = mat2( 1.7f, -1.2f, 2.3f, -0.5f );
        ///
        /// // result = mat2( 1, -2, 2, -1 )
        /// mat2 result = floor( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The floor of each component
        ///
        static ITK_INLINE typeMat2 floor(const typeMat2 &m) noexcept
        {
            return typeMat2(
                OP<type2>::floor(m[0]),
                OP<type2>::floor(m[1]));
        }

        /// \brief The ceil of each component.
        ///
        /// Returns the smallest integer value greater than or equal to each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a = mat2( 1.2f, -1.7f, 0.5f, -2.3f );
        ///
        /// // result = mat2( 2, -1, 1, -2 )
        /// mat2 result = ceil( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The ceil of each component
        ///
        static ITK_INLINE typeMat2 ceil(const typeMat2 &m) noexcept
        {
            return typeMat2(
                OP<type2>::ceil(m[0]),
                OP<type2>::ceil(m[1]));
        }

        /// \brief Round each component.
        ///
        /// Returns the nearest integer to each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a = mat2( 1.5f, -1.5f, 2.5f, -2.5f );
        ///
        /// // result = mat2( 2, -2, 3, -3 )
        /// mat2 result = round( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The rounded value of each component
        ///
        static ITK_INLINE typeMat2 round(const typeMat2 &m) noexcept
        {
            return typeMat2(
                OP<type2>::round(m[0]),
                OP<type2>::round(m[1]));
        }

        /// \brief fmod each component.
        ///
        /// Returns the floating-point remainder of a / b for each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 a = mat2( 10, 20, 30, 40 );
        /// mat2 b = mat2( 3, 7, 5, 8 );
        ///
        /// // result = mat2( 1, 6, 0, 0 )
        /// mat2 result = fmod( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The dividend matrix
        /// \param b The divisor matrix
        /// \return The fmod of each component pair
        ///
        static ITK_INLINE typeMat2 fmod(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            return typeMat2(
                OP<type2>::fmod(a[0], b[0]),
                OP<type2>::fmod(a[1], b[1]));
        }

        /// \brief Step function on each component. ( v >= threshold ) ? 1 : 0
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 threshold = mat2( 5, 5, 5, 5 );
        /// mat2 v = mat2( 3, 7, 1, 9 );
        ///
        /// // result = mat2( 0, 1, 0, 1 )
        /// mat2 result = step( threshold, v );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param threshold The threshold value
        /// \param v The input matrix
        /// \return 1 where v >= threshold, 0 otherwise, for each component
        ///
        static ITK_INLINE typeMat2 step(const typeMat2 &threshold, const typeMat2 &v) noexcept
        {
            return typeMat2(
                OP<type2>::step(threshold[0], v[0]),
                OP<type2>::step(threshold[1], v[1]));
        }

        /// \brief Smoothstep interpolation on each component
        ///
        /// For each component, computes a smooth (hermite) interpolation between 0 and 1,
        /// where the transition happens between edge0 and edge1:
        ///
        /// <pre>
        /// t = clamp( (x - edge0) / |edge1 - edge0|, 0, 1 )
        /// result = t * t * ( 3 - 2 * t )
        /// </pre>
        ///
        /// The result is 0 when x is at or below edge0, 1 when x is at or above edge1,
        /// and a smooth S-curve in between.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat2 edge0 = mat2( 0, 0, 0, 0 );
        /// mat2 edge1 = mat2( 10, 10, 10, 10 );
        /// mat2 x = mat2( 5, 5, 5, 5 );
        ///
        /// // result = mat2( 0.5, 0.5, 0.5, 0.5 )
        /// mat2 result = smoothstep( edge0, edge1, x );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param edge0 The lower edge of the transition
        /// \param edge1 The upper edge of the transition
        /// \param x The input value
        /// \return The smoothstep interpolated value for each component
        ///
        static ITK_INLINE typeMat2 smoothstep(const typeMat2 &edge0, const typeMat2 &edge1, const typeMat2 &x) noexcept
        {
            using type_info = FloatTypeInfo<_type>;
            typeMat2 dir = edge1 - edge0;

            _type length_dir = OP<_type>::maximum(self_type::length(dir), type_info::min);

            typeMat2 value = x - edge0;
            value *= (_type)1 / length_dir;

            typeMat2 t = self_type::clamp(value, typeMat2((_type)0), typeMat2((_type)1));
            return t * t * ((_type)3 - (_type)2 * t);
            // return typeMat2(
            //     OP<type2>::smoothstep(edge0[0], edge1[0], x[0]),
            //     OP<type2>::smoothstep(edge0[1], edge1[1], x[1]));
        }
    };

}