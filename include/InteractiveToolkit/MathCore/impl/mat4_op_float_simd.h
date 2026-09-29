#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'mat4_op_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "mat4_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    /// \brief SIMD operations specialization for mat4 with float components.
    ///
    /// Provides SIMD-optimized utility functions for the mat4 class when the
    /// scalar type is float and a SIMD strategy is enabled (SSE or NEON). This
    /// specialization is selected via SFINAE when the _type template parameter
    /// is float and the _simd template parameter matches SIMD_TYPE::SSE or
    /// SIMD_TYPE::NEON.
    ///
    /// The matrix is stored in column-major order, where each column is a vec4:
    ///
    /// <pre>
    /// | a1 b1 c1 d1 |
    /// | a2 b2 c2 d2 |
    /// | a3 b3 c3 d3 |
    /// | a4 b4 c4 d4 |
    /// </pre>
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _type The scalar type of the mat4 components; this specialization
    ///         is selected when _type is float.
    /// \tparam _simd The SIMD strategy type; this specialization is selected when
    ///         _simd is SIMD_TYPE::SSE or SIMD_TYPE::NEON.
    /// \tparam _algorithm The algorithm type.
    ///
    template <typename _type, typename _simd, typename _algorithm>
    struct OP<mat4<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                  (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                   std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type,
              _algorithm>
    {
    private:
        /// \brief Alias for the 4x4 matrix type.
        ///
        using typeMat4 = mat4<_type, _simd>;
        /// \brief Alias for the 4-component vector type (a matrix column).
        ///
        using type4 = vec4<_type, _simd>;
        /// \brief Alias for the fully specialized OP struct type.
        ///
        using self_type = OP<typeMat4>;

    public:
        /// \brief Returns the next representable floating-point value after each component.
        ///
        /// For each component of the matrix, returns the next floating-point value
        /// in the direction of positive infinity.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 a;
        ///
        /// mat4 result = next( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input matrix
        /// \return The next representable value for each component
        ///
        static ITK_INLINE typeMat4 next(const typeMat4 &p) noexcept
        {
            return typeMat4(
                OP<type4>::next(p[0]),
                OP<type4>::next(p[1]),
                OP<type4>::next(p[2]),
                OP<type4>::next(p[3]));
        }

        /// \brief Returns the previous representable floating-point value before each component.
        ///
        /// For each component of the matrix, returns the previous floating-point value
        /// in the direction of negative infinity.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 a;
        ///
        /// mat4 result = previous( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input matrix
        /// \return The previous representable value for each component
        ///
        static ITK_INLINE typeMat4 previous(const typeMat4 &p) noexcept
        {
            return typeMat4(
                OP<type4>::previous(p[0]),
                OP<type4>::previous(p[1]),
                OP<type4>::previous(p[2]),
                OP<type4>::previous(p[3]));
        }

        /// \brief Returns the next representable floating-point value after each component in the direction of a target.
        ///
        /// For each component, returns the next floating-point value after p in the direction of _to.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 a;
        /// mat4 target;
        ///
        /// mat4 result = next_after( a, target );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input matrix
        /// \param _to The target direction matrix
        /// \return The next representable value for each component in the direction of _to
        ///
        static ITK_INLINE typeMat4 next_after(const typeMat4 &p, const typeMat4 &_to) noexcept
        {
            return typeMat4(
                OP<type4>::next_after(p[0], _to[0]),
                OP<type4>::next_after(p[1], _to[1]),
                OP<type4>::next_after(p[2], _to[2]),
                OP<type4>::next_after(p[3], _to[3]));
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
        /// mat4 result;
        /// result = clamp( mat4( 300, 3 ), mat4( 0, -1 ), mat4( 50, 5 ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param value The value to evaluate
        /// \param min The min threshold
        /// \param max The max threshold
        /// \return The evaluated value
        ///
        static ITK_INLINE typeMat4 clamp(const typeMat4 &value, const typeMat4 &min, const typeMat4 &max) noexcept
        {
            return typeMat4(
                OP<type4>::clamp(value[0], min[0], max[0]),
                OP<type4>::clamp(value[1], min[1], max[1]),
                OP<type4>::clamp(value[2], min[2], max[2]),
                OP<type4>::clamp(value[3], min[3], max[3]));
        }

        /// \brief Computes the dot product between two matrices
        ///
        /// The dot product is a single value computed from the two matrices.
        /// It is the sum of the dot products of each corresponding column:
        ///
        /// dot(a, b) = dot(a[0], b[0]) + dot(a[1], b[1]) + dot(a[2], b[2]) + dot(a[3], b[3])
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 a, b;
        /// float result = dot( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \return The dot product between the two matrices
        ///
        static ITK_INLINE _type dot(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            _type dota = OP<type4>::dot(a[0], b[0]);
            _type dotb = OP<type4>::dot(a[1], b[1]);
            _type dotc = OP<type4>::dot(a[2], b[2]);
            _type dotd = OP<type4>::dot(a[3], b[3]);
            return dota + dotb + dotc + dotd;
        }

        /// \brief Normalize a matrix
        ///
        /// Returns a matrix scaled so that its length (see \ref length) is one.
        ///
        /// result = m/|m|
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 m;
        ///
        /// mat4 m_normalized = normalize( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The matrix to normalize
        /// \return The normalized matrix
        ///
        static ITK_INLINE typeMat4 normalize(const typeMat4 &m) noexcept
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
        /// mat4 input;
        ///
        /// float result = sqrLength(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The matrix
        /// \return The squared length
        ///
        static ITK_INLINE _type sqrLength(const typeMat4 &a) noexcept
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
        /// mat4 input;
        ///
        /// float result = length(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The matrix
        /// \return The length
        ///
        static ITK_INLINE _type length(const typeMat4 &a) noexcept
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
        /// mat4 a, b;
        ///
        /// float result = sqrDistance( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \return The squared distance between a and b
        ///
        static ITK_INLINE _type sqrDistance(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            typeMat4 ab = b - a;
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
        /// mat4 a, b;
        ///
        /// float result = distance( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \return The distance between a and b
        ///
        static ITK_INLINE _type distance(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            typeMat4 ab = b - a;
            return OP<_type>::sqrt(self_type::dot(ab, ab));
        }

        /// \brief Return the greater value from the parameter
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 input;
        ///
        /// float max = maximum( input );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Set of values to test
        /// \return The greater value from the parameter
        ///
        static ITK_INLINE _type maximum(const typeMat4 &a) noexcept
        {
            type4 max_a = OP<type4>::maximum(a[0], a[1]);
            type4 max_b = OP<type4>::maximum(a[2], a[3]);
            type4 max_c = OP<type4>::maximum(max_a, max_b);
            return OP<type4>::maximum(max_c);
        }

        /// \brief Component-wise maximum value from two matrices
        ///
        /// Return the maximum value considering each component of the matrix.
        ///
        /// result: mat4( maximum(a[0],b[0]), maximum(a[1],b[1]), maximum(a[2],b[2]), maximum(a[3],b[3]) )
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 a, b;
        ///
        /// mat4 result = maximum( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A matrix
        /// \param b A matrix
        /// \return The maximum value for each matrix component
        ///
        static ITK_INLINE typeMat4 maximum(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            return typeMat4(OP<type4>::maximum(a[0], b[0]),
                            OP<type4>::maximum(a[1], b[1]),
                            OP<type4>::maximum(a[2], b[2]),
                            OP<type4>::maximum(a[3], b[3]));
        }

        /// \brief Return the smaller value from the parameter
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 input;
        ///
        /// float min = minimum( input );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Set of values to test
        /// \return The smaller value from the parameter
        ///
        static ITK_INLINE _type minimum(const typeMat4 &a) noexcept
        {
            type4 max_a = OP<type4>::minimum(a[0], a[1]);
            type4 max_b = OP<type4>::minimum(a[2], a[3]);
            type4 max_c = OP<type4>::minimum(max_a, max_b);
            return OP<type4>::minimum(max_c);
        }

        /// \brief Component-wise minimum value from two matrices
        ///
        /// Return the minimum value considering each component of the matrix.
        ///
        /// result: mat4( minimum(a[0],b[0]), minimum(a[1],b[1]), minimum(a[2],b[2]), minimum(a[3],b[3]) )
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 a, b;
        ///
        /// mat4 result = minimum( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A matrix
        /// \param b A matrix
        /// \return The minimum value for each matrix component
        ///
        static ITK_INLINE typeMat4 minimum(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            return typeMat4(OP<type4>::minimum(a[0], b[0]),
                            OP<type4>::minimum(a[1], b[1]),
                            OP<type4>::minimum(a[2], b[2]),
                            OP<type4>::minimum(a[3], b[3]));
        }

        /// \brief Compute the absolute value of a matrix (magnitude)
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 input;
        ///
        /// mat4 result = abs( input );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A matrix
        /// \return mat4( |a[0]|, |a[1]|, |a[2]|, |a[3]| )
        ///
        static ITK_INLINE typeMat4 abs(const typeMat4 &a) noexcept
        {
            return typeMat4(OP<type4>::abs(a[0]),
                            OP<type4>::abs(a[1]),
                            OP<type4>::abs(a[2]),
                            OP<type4>::abs(a[3]));
        }

        /// \brief Computes the linear interpolation
        ///
        /// When the factor is between 0 and 1, it returns the convex relation (linear interpolation) between a and b.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 a;
        /// mat4 b;
        ///
        /// mat4 result = lerp( a, b, 0.75f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Origin matrix
        /// \param b Target matrix
        /// \param factor The amount (%) to leave the Origin to the Target.
        /// \return The interpolation result
        ///
        static ITK_INLINE typeMat4 lerp(const typeMat4 &a, const typeMat4 &b, const _type &factor) noexcept
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
        /// float u, v;
        /// mat4 v0, v1, v2;
        ///
        /// mat4 result = barylerp( u, v, v0, v1, v2 );
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
        static ITK_INLINE typeMat4 barylerp(const _type &u, const _type &v, const typeMat4 &v0, const typeMat4 &v1, const typeMat4 &v2) noexcept
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
        /// mat4 dataA, dataB, dataC, dataD;
        ///
        /// mat4 result = blerp( dataA, dataB, dataC, dataD, 0.5f, 0.5f );
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
        static ITK_INLINE typeMat4 blerp(const typeMat4 &A, const typeMat4 &B, const typeMat4 &C, const typeMat4 &D,
                                         const _type &dx, const _type &dy) noexcept
        {
            _type omdx = (_type)1 - dx,
                  omdy = (_type)1 - dy;
            return (omdx * omdy) * A + (omdx * dy) * D + (dx * omdy) * B + (dx * dy) * C;
        }

        /// \brief Extracts the rotation part of a matrix
        ///
        /// Returns a new matrix that keeps only the rotation (the 3x3 upper-left
        /// block) of the input matrix, discarding the translation column and the
        /// bottom row. The result is a pure rotation matrix:
        ///
        /// <pre>
        /// | a1 b1 c1 0 |
        /// | a2 b2 c2 0 |
        /// | a3 b3 c3 0 |
        /// | 0  0  0  1 |
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 m;
        ///
        /// mat4 rotation = extractRotation( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return A matrix containing only the rotation part of m
        ///
        static ITK_INLINE typeMat4 extractRotation(const typeMat4 &m) noexcept
        {
#if defined(ITK_SSE2)
            __m128 a = _mm_and_ps(m.array_sse[0], _vec3_valid_bits_sse);
            __m128 b = _mm_and_ps(m.array_sse[1], _vec3_valid_bits_sse);
            __m128 c = _mm_and_ps(m.array_sse[2], _vec3_valid_bits_sse);

            // a = _mm_and_ps(m.array_sse[0], _vec3_valid_bits_sse);
            // b = _mm_and_ps(m.array_sse[1], _vec3_valid_bits_sse);
            // c = _mm_and_ps(m.array_sse[2], _vec3_valid_bits_sse);

            return typeMat4(a, b, c, _vec4_0001_sse);
#elif defined(ITK_NEON)
            typeMat4 r(
                vsetq_lane_f32(0.0f, m.array_neon[0], 3),
                vsetq_lane_f32(0.0f, m.array_neon[1], 3),
                vsetq_lane_f32(0.0f, m.array_neon[2], 3),
                _neon_0001);

            // r.array_neon[0][3] = 0;
            // r.array_neon[1][3] = 0;
            // r.array_neon[2][3] = 0;

            // r.array_neon[0] = vsetq_lane_f32(0.0f, r.array_neon[0], 3);
            // r.array_neon[1] = vsetq_lane_f32(0.0f, r.array_neon[1], 3);
            // r.array_neon[2] = vsetq_lane_f32(0.0f, r.array_neon[2], 3);

            return r;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Extracts the 2D rotation part of a matrix
        ///
        /// Returns a new matrix that keeps only the 2D rotation (the 2x2 upper-left
        /// block, i.e. the x-y plane) of the input matrix, discarding the rest.
        /// The result is a pure 2D rotation matrix:
        ///
        /// <pre>
        /// | a1 b1 0 0 |
        /// | a2 b2 0 0 |
        /// | 0  0  1 0 |
        /// | 0  0  0 1 |
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 m;
        ///
        /// mat4 rotation_2d = extractRotation_2x2( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return A matrix containing only the 2D rotation part of m
        ///
        static ITK_INLINE typeMat4 extractRotation_2x2(const typeMat4 &m) noexcept
        {
#if defined(ITK_SSE2)
            __m128 a = _mm_and_ps(m.array_sse[0], _vec2_valid_bits_sse);
            __m128 b = _mm_and_ps(m.array_sse[1], _vec2_valid_bits_sse);

            // a = _mm_and_ps(m.array_sse[0], _vec3_valid_bits_sse);
            // b = _mm_and_ps(m.array_sse[1], _vec3_valid_bits_sse);
            // c = _mm_and_ps(m.array_sse[2], _vec3_valid_bits_sse);

            return typeMat4(a, b, _vec4_0010_sse, _vec4_0001_sse);
#elif defined(ITK_NEON)
            const float32x2_t _zero_v2 = vdup_n_f32(0.0f);
            typeMat4 r(
                vcombine_f32(vget_low_f32(m.array_neon[0]), _zero_v2),
                vcombine_f32(vget_low_f32(m.array_neon[1]), _zero_v2),
                _neon_0010,
                _neon_0001);

            // r.array_neon[0][3] = 0;
            // r.array_neon[1][3] = 0;
            // r.array_neon[2][3] = 0;

            // r.array_neon[0] = vsetq_lane_f32(0.0f, r.array_neon[0], 3);
            // r.array_neon[1] = vsetq_lane_f32(0.0f, r.array_neon[1], 3);
            // r.array_neon[2] = vsetq_lane_f32(0.0f, r.array_neon[2], 3);

            return r;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Extracts the x-axis (first column) of a matrix
        ///
        /// Returns the first column of the matrix, which represents the x-axis
        /// of the transformation.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 m;
        ///
        /// vec4 xaxis = extractXaxis( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The x-axis (first column) of the matrix
        ///
        static ITK_INLINE type4 extractXaxis(const typeMat4 &m) noexcept
        {
            return m[0];
        }

        /// \brief Extracts the y-axis (second column) of a matrix
        ///
        /// Returns the second column of the matrix, which represents the y-axis
        /// of the transformation.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 m;
        ///
        /// vec4 yaxis = extractYaxis( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The y-axis (second column) of the matrix
        ///
        static ITK_INLINE type4 extractYaxis(const typeMat4 &m) noexcept
        {
            return m[1];
        }

        /// \brief Extracts the z-axis (third column) of a matrix
        ///
        /// Returns the third column of the matrix, which represents the z-axis
        /// of the transformation.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 m;
        ///
        /// vec4 zaxis = extractZaxis( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The z-axis (third column) of the matrix
        ///
        static ITK_INLINE type4 extractZaxis(const typeMat4 &m) noexcept
        {
            return m[2];
        }

        /// \brief Extracts the translation (fourth column) of a matrix
        ///
        /// Returns the fourth column of the matrix, which represents the
        /// translation part of the transformation.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 m;
        ///
        /// vec4 translation = extractTranslation( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The translation (fourth column) of the matrix
        ///
        static ITK_INLINE type4 extractTranslation(const typeMat4 &m) noexcept
        {
            return m[3];
        }

        /// \brief Computes the transpose of a matrix
        ///
        /// The transpose of a matrix swaps its rows and columns. The element at
        /// position (row, column) becomes the element at (column, row).
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 m;
        ///
        /// mat4 m_t = transpose( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The transposed matrix
        ///
        static ITK_INLINE typeMat4 transpose(const typeMat4 &m) noexcept
        {
#if defined(ITK_SSE2)
            __m128 tmp0 = _mm_shuffle_ps(m.array_sse[0], m.array_sse[1], _MM_SHUFFLE(1, 0, 1, 0));
            __m128 tmp2 = _mm_shuffle_ps(m.array_sse[0], m.array_sse[1], _MM_SHUFFLE(3, 2, 3, 2));
            __m128 tmp1 = _mm_shuffle_ps(m.array_sse[2], m.array_sse[3], _MM_SHUFFLE(1, 0, 1, 0));
            __m128 tmp3 = _mm_shuffle_ps(m.array_sse[2], m.array_sse[3], _MM_SHUFFLE(3, 2, 3, 2));

            return typeMat4(
                _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(2, 0, 2, 0)),
                _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(3, 1, 3, 1)),
                _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(2, 0, 2, 0)),
                _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(3, 1, 3, 1)));
#elif defined(ITK_NEON)
            float32x4x2_t ab = vtrnq_f32(m.array_neon[0], m.array_neon[1]);
            float32x4x2_t cd = vtrnq_f32(m.array_neon[2], m.array_neon[3]);
            float32x4_t a_ = vcombine_f32(vget_low_f32(ab.val[0]), vget_low_f32(cd.val[0]));
            float32x4_t b_ = vcombine_f32(vget_low_f32(ab.val[1]), vget_low_f32(cd.val[1]));
            float32x4_t c_ = vcombine_f32(vget_high_f32(ab.val[0]), vget_high_f32(cd.val[0]));
            float32x4_t d_ = vcombine_f32(vget_high_f32(ab.val[1]), vget_high_f32(cd.val[1]));

            return typeMat4(a_, b_, c_, d_);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Computes the determinant of a matrix
        ///
        /// The determinant is a scalar value that can be computed from the
        /// elements of a square matrix. It is useful to test if a matrix is
        /// invertible (a matrix is invertible when its determinant is not zero)
        /// and to compute the inverse of a matrix.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 m;
        ///
        /// float det = determinant( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The determinant of the matrix
        ///
        static ITK_INLINE _type determinant(const typeMat4 &m) noexcept
        {
#if defined(ITK_SSE2)
            // SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
            // SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
            // SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];

            // SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
            // SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
            // SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

            // First 2 columns
            __m128 Swp2A = _mm_shuffle_ps(m.array_sse[2], m.array_sse[2], _MM_SHUFFLE(0, 1, 1, 2));
            __m128 Swp3A = _mm_shuffle_ps(m.array_sse[3], m.array_sse[3], _MM_SHUFFLE(3, 2, 3, 3));
            __m128 MulA = _mm_mul_ps(Swp2A, Swp3A);

            // Second 2 columns
            __m128 Swp2B = _mm_shuffle_ps(m.array_sse[2], m.array_sse[2], _MM_SHUFFLE(3, 2, 3, 3));
            __m128 Swp3B = _mm_shuffle_ps(m.array_sse[3], m.array_sse[3], _MM_SHUFFLE(0, 1, 1, 2));
            __m128 MulB = _mm_mul_ps(Swp2B, Swp3B);

            // Columns subtraction
            __m128 SubE = _mm_sub_ps(MulA, MulB);

            // Last 2 rows
            __m128 Swp2C = _mm_shuffle_ps(m.array_sse[2], m.array_sse[2], _MM_SHUFFLE(0, 0, 1, 2));
            __m128 Swp3C = _mm_shuffle_ps(m.array_sse[3], m.array_sse[3], _MM_SHUFFLE(1, 2, 0, 0));
            __m128 MulC = _mm_mul_ps(Swp2C, Swp3C);
            __m128 SubF = _mm_sub_ps(_mm_movehl_ps(MulC, MulC), MulC);

            // vec4(
            //	+ (m[1][1] * SubFactor00 - m[1][2] * SubFactor01 + m[1][3] * SubFactor02),
            //	- (m[1][0] * SubFactor00 - m[1][2] * SubFactor03 + m[1][3] * SubFactor04),
            //	+ (m[1][0] * SubFactor01 - m[1][1] * SubFactor03 + m[1][3] * SubFactor05),
            //	- (m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05));

            __m128 SubFacA = _mm_shuffle_ps(SubE, SubE, _MM_SHUFFLE(2, 1, 0, 0));
            __m128 SwpFacA = _mm_shuffle_ps(m.array_sse[1], m.array_sse[1], _MM_SHUFFLE(0, 0, 0, 1));
            __m128 MulFacA = _mm_mul_ps(SwpFacA, SubFacA);

            __m128 SubTmpB = _mm_shuffle_ps(SubE, SubF, _MM_SHUFFLE(0, 0, 3, 1));
            __m128 SubFacB = _mm_shuffle_ps(SubTmpB, SubTmpB, _MM_SHUFFLE(3, 1, 1, 0)); // SubF[0], SubE[3], SubE[3], SubE[1];
            __m128 SwpFacB = _mm_shuffle_ps(m.array_sse[1], m.array_sse[1], _MM_SHUFFLE(1, 1, 2, 2));
            __m128 MulFacB = _mm_mul_ps(SwpFacB, SubFacB);

            __m128 SubRes = _mm_sub_ps(MulFacA, MulFacB);

            __m128 SubTmpC = _mm_shuffle_ps(SubE, SubF, _MM_SHUFFLE(1, 0, 2, 2));
            __m128 SubFacC = _mm_shuffle_ps(SubTmpC, SubTmpC, _MM_SHUFFLE(3, 3, 2, 0));
            __m128 SwpFacC = _mm_shuffle_ps(m.array_sse[1], m.array_sse[1], _MM_SHUFFLE(2, 3, 3, 3));
            __m128 MulFacC = _mm_mul_ps(SwpFacC, SubFacC);

            __m128 AddRes = _mm_add_ps(SubRes, MulFacC);
            //__m128 DetCof = _mm_mul_ps(AddRes, _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f));

            const __m128 SignMask = _mm_set_ps(-0.0f, 0.0f, -0.0f, 0.0f);

            //__m128 DetCof = _mm_mul_ps(AddRes, _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f));
            __m128 DetCof = _mm_xor_ps(AddRes, SignMask);

            // return m[0][0] * DetCof[0]
            //	 + m[0][1] * DetCof[1]
            //	 + m[0][2] * DetCof[2]
            //	 + m[0][3] * DetCof[3];

            __m128 Det0 = dot_sse_4(m.array_sse[0], DetCof);

            return _mm_f32_read_0(Det0);
#elif defined(ITK_NEON)

            // T SubFactor00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
            // T SubFactor01 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
            // T SubFactor02 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
            // T SubFactor03 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
            // T SubFactor04 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
            // T SubFactor05 = m[2][0] * m[3][1] - m[3][0] * m[2][1];

            // First 2 columns
            float32x4_t Swp2A = vshuffle_0112(m.array_neon[2]);
            float32x4_t Swp3A = vshuffle_3233(m.array_neon[3]);
            float32x4_t MulA = vmulq_f32(Swp2A, Swp3A);

            // Second 2 columns
            float32x4_t Swp2B = vshuffle_3233(m.array_neon[2]);
            float32x4_t Swp3B = vshuffle_0112(m.array_neon[3]);
            float32x4_t MulB = vmulq_f32(Swp2B, Swp3B);

            // Columns subtraction
            float32x4_t SubE = vsubq_f32(MulA, MulB);

            // Last 2 rows
            float32x4_t Swp2C = vshuffle_0012(m.array_neon[2]);
            float32x4_t Swp3C = vshuffle_1200(m.array_neon[3]);
            float32x4_t MulC = vmulq_f32(Swp2C, Swp3C);
            float32x4_t SubF = vsubq_f32(vmovehl(MulC, MulC), MulC);

            // vec4(
            //	+ (m[1][1] * SubFactor00 - m[1][2] * SubFactor01 + m[1][3] * SubFactor02),
            //	- (m[1][0] * SubFactor00 - m[1][2] * SubFactor03 + m[1][3] * SubFactor04),
            //	+ (m[1][0] * SubFactor01 - m[1][1] * SubFactor03 + m[1][3] * SubFactor05),
            //	- (m[1][0] * SubFactor02 - m[1][1] * SubFactor04 + m[1][2] * SubFactor05));

            float32x4_t SubFacA = vshuffle_2100(SubE);
            float32x4_t SwpFacA = vshuffle_0001(m.array_neon[1]);
            float32x4_t MulFacA = vmulq_f32(SwpFacA, SubFacA);

            float32x4_t SubTmpB = vshuffle_0031(SubE, SubF);
            float32x4_t SubFacB = vshuffle_3110(SubTmpB); // SubF[0], SubE[3], SubE[3], SubE[1];
            float32x4_t SwpFacB = vshuffle_1122(m.array_neon[1]);
            float32x4_t MulFacB = vmulq_f32(SwpFacB, SubFacB);

            float32x4_t SubRes = vsubq_f32(MulFacA, MulFacB);

            float32x4_t SubTmpC = vshuffle_1022(SubE, SubF);
            float32x4_t SubFacC = vshuffle_3320(SubTmpC);
            float32x4_t SwpFacC = vshuffle_2333(m.array_neon[1]);
            float32x4_t MulFacC = vmulq_f32(SwpFacC, SubFacC);

            float32x4_t AddRes = vaddq_f32(SubRes, MulFacC);
            //__m128 DetCof = _mm_mul_ps(AddRes, _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f));

            // const float32x4_t SignMask = (float32x4_t){1.0f, -1.0f, 1.0f, -1.0f};
            const uint32x4_t SignMask = vreinterpretq_u32_f32(
                (float32x4_t){0.0f, -0.0f, 0.0f, -0.0f});

            //__m128 DetCof = _mm_mul_ps(AddRes, _mm_setr_ps(1.0f, -1.0f, 1.0f, -1.0f));
            // float32x4_t DetCof = vmulq_f32(AddRes, SignMask);
            float32x4_t DetCof = vreinterpretq_f32_u32(
                veorq_u32(vreinterpretq_u32_f32(AddRes), SignMask));

            // return m[0][0] * DetCof[0]
            //	 + m[0][1] * DetCof[1]
            //	 + m[0][2] * DetCof[2]
            //	 + m[0][3] * DetCof[3];

            float32x4_t Det0 = dot_neon_4(m.array_neon[0], DetCof);

            return vgetq_lane_f32(Det0, 0);

#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Extracts the Euler angles (roll, pitch, yaw) from a rotation matrix
        ///
        /// Decomposes the rotation part of the matrix into three Euler angles
        /// (roll, pitch and yaw) in radians, using the ZYX (yaw-pitch-roll)
        /// convention.
        ///
        /// <pre>
        /// Reference:
        /// https://www.learnopencv.com/rotation-matrix-to-euler-angles/
        /// </pre>
        ///
        /// \note When the matrix is singular (pitch is close to +/- 90 degrees),
        ///       the roll and yaw cannot be determined independently, so the yaw
        ///       is set to zero.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 m;
        /// float roll, pitch, yaw;
        ///
        /// extractEuler( m, &roll, &pitch, &yaw );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input rotation matrix
        /// \param roll Output parameter that will hold the roll angle in radians
        /// \param pitch Output parameter that will hold the pitch angle in radians
        /// \param yaw Output parameter that will hold the yaw angle in radians
        ///
        static ITK_INLINE void extractEuler(const typeMat4 &m, _type *roll, _type *pitch, _type *yaw) noexcept
        {
            //
            // https://www.learnopencv.com/rotation-matrix-to-euler-angles/
            //
            _type sy = OP<_type>::sqrt(m.a1 * m.a1 + m.a2 * m.a2);

            bool singular = sy < EPSILON<_type>::high_precision; // 1e-6f; // If

            float x, y, z;
            if (!singular)
            {
                x = OP<_type>::atan2(m.b3, m.c3);
                y = OP<_type>::atan2(-m.a3, sy);
                z = OP<_type>::atan2(m.a2, m.a1);
            }
            else
            {
                x = OP<_type>::atan2(-m.c2, m.b2);
                y = OP<_type>::atan2(-m.a3, sy);
                z = 0;
            }

            *roll = x;
            *pitch = y;
            *yaw = z;
        }

        /// \brief Moves a matrix towards a target, limited by a maximum distance variation
        ///
        /// Returns a matrix that is at most maxDistanceVariation away from current,
        /// moving in the direction of target. If the distance between current and
        /// target is smaller than maxDistanceVariation, the result is the target.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 current, target;
        /// float maxStep = 10.0f;
        ///
        /// mat4 result = move( current, target, maxStep );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param current The current matrix
        /// \param target The target matrix
        /// \param maxDistanceVariation The maximum distance that the result can move from current
        /// \return The matrix moved towards target, limited by maxDistanceVariation
        ///
        static ITK_INLINE typeMat4 move(const typeMat4 &current, const typeMat4 &target, const _type &maxDistanceVariation) noexcept
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
        /// mat4 a;
        ///
        /// mat4 result = sign( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The sign of each component
        ///
        static ITK_INLINE typeMat4 sign(const typeMat4 &m) noexcept
        {
            return typeMat4(
                OP<type4>::sign(m[0]),
                OP<type4>::sign(m[1]),
                OP<type4>::sign(m[2]),
                OP<type4>::sign(m[3]));
        }

        /// \brief The floor of each component.
        ///
        /// Returns the largest integer value less than or equal to each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 a;
        ///
        /// mat4 result = floor( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The floor of each component
        ///
        static ITK_INLINE typeMat4 floor(const typeMat4 &m) noexcept
        {
            return typeMat4(
                OP<type4>::floor(m[0]),
                OP<type4>::floor(m[1]),
                OP<type4>::floor(m[2]),
                OP<type4>::floor(m[3]));
        }

        /// \brief The ceil of each component.
        ///
        /// Returns the smallest integer value greater than or equal to each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 a;
        ///
        /// mat4 result = ceil( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The ceil of each component
        ///
        static ITK_INLINE typeMat4 ceil(const typeMat4 &m) noexcept
        {
            return typeMat4(
                OP<type4>::ceil(m[0]),
                OP<type4>::ceil(m[1]),
                OP<type4>::ceil(m[2]),
                OP<type4>::ceil(m[3]));
        }

        /// \brief Round each component.
        ///
        /// Returns the nearest integer to each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 a;
        ///
        /// mat4 result = round( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The rounded value of each component
        ///
        static ITK_INLINE typeMat4 round(const typeMat4 &m) noexcept
        {
            return typeMat4(
                OP<type4>::round(m[0]),
                OP<type4>::round(m[1]),
                OP<type4>::round(m[2]),
                OP<type4>::round(m[3]));
        }

        /// \brief fmod each component.
        ///
        /// Returns the floating-point remainder of a / b for each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 a;
        /// mat4 b;
        ///
        /// mat4 result = fmod( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The dividend matrix
        /// \param b The divisor matrix
        /// \return The fmod of each component pair
        ///
        static ITK_INLINE typeMat4 fmod(const typeMat4 &a, const typeMat4 &b) noexcept
        {
            return typeMat4(
                OP<type4>::fmod(a[0], b[0]),
                OP<type4>::fmod(a[1], b[1]),
                OP<type4>::fmod(a[2], b[2]),
                OP<type4>::fmod(a[3], b[3]));
        }

        /// \brief Step function on each component. ( v >= threshold ) ? 1 : 0
        ///
        /// Returns 1 where v >= threshold, 0 otherwise, for each component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 threshold;
        /// mat4 v;
        ///
        /// mat4 result = step( threshold, v );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param threshold The threshold matrix
        /// \param v The input matrix
        /// \return 1 where v >= threshold, 0 otherwise, for each component
        ///
        static ITK_INLINE typeMat4 step(const typeMat4 &threshold, const typeMat4 &v) noexcept
        {
            return typeMat4(
                OP<type4>::step(threshold[0], v[0]),
                OP<type4>::step(threshold[1], v[1]),
                OP<type4>::step(threshold[2], v[2]),
                OP<type4>::step(threshold[3], v[3]));
        }

        /// \brief Computes the smoothstep function of each component of a matrix
        ///
        /// For each component, returns a smooth interpolation between 0 and 1
        /// based on the position of x between edge0 and edge1. The result is
        /// clamped to the range [0, 1].
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 edge0, edge1, x;
        ///
        /// mat4 result = smoothstep( edge0, edge1, x );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param edge0 The lower edge of the interpolation range
        /// \param edge1 The upper edge of the interpolation range
        /// \param x The value to interpolate
        /// \return The component-wise smoothstep value, clamped to [0, 1]
        ///
        static ITK_INLINE typeMat4 smoothstep(const typeMat4 &edge0, const typeMat4 &edge1, const typeMat4 &x) noexcept
        {
            using type_info = FloatTypeInfo<_type>;
            typeMat4 dir = edge1 - edge0;

            _type length_dir = OP<_type>::maximum(self_type::length(dir), type_info::min);

            typeMat4 value = x - edge0;
            value *= (_type)1 / length_dir;

            typeMat4 t = self_type::clamp(value, typeMat4((_type)0), typeMat4((_type)1));
            return t * t * ((_type)3 - (_type)2 * t);
            // return typeMat4(
            //     OP<type4>::smoothstep(edge0[0], edge1[0], x[0]),
            //     OP<type4>::smoothstep(edge0[1], edge1[1], x[1]),
            //     OP<type4>::smoothstep(edge0[2], edge1[2], x[2]),
            //     OP<type4>::smoothstep(edge0[3], edge1[3], x[3]));
        }
    };

}