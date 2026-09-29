#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'mat3_op_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "mat3_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

// #include "quat_op.h" -- do not use any quat for mat3 OP

namespace MathCore
{

    /// \brief SIMD operations specialization for mat3 with float components.
    ///
    /// Provides SIMD-optimized utility functions for the mat3 class when the
    /// scalar type is float and the SIMD strategy is SSE or NEON. This
    /// specialization is selected via SFINAE when the _type template parameter
    /// is float and the _simd template parameter matches SIMD_TYPE::SSE or
    /// SIMD_TYPE::NEON.
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _type The scalar type of the mat3 components; this specialization
    ///         is selected when _type is float.
    /// \tparam _simd The SIMD strategy type; this specialization is selected when
    ///         _simd is SIMD_TYPE::SSE or SIMD_TYPE::NEON.
    /// \tparam _algorithm The algorithm type.
    ///
    template <typename _type, typename _simd, typename _algorithm>
    struct OP<mat3<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                  (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                   std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type,
              _algorithm>
    {
    private:
        /// \brief Alias for the 3x3 matrix type.
        ///
        using typeMat3 = mat3<_type, _simd>;
        /// \brief Alias for the 3-component vector type (a matrix column).
        ///
        using type3 = vec3<_type, _simd>;
        /// \brief Alias for the fully specialized OP struct type.
        ///
        using self_type = OP<typeMat3>;

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
        /// mat3 a;
        ///
        /// mat3 result = next( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input matrix
        /// \return The next representable value for each component
        ///
        static ITK_INLINE typeMat3 next(const typeMat3 &p) noexcept
        {
            return typeMat3(
                OP<type3>::next(p[0]),
                OP<type3>::next(p[1]),
                OP<type3>::next(p[2]));
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
        /// mat3 a;
        ///
        /// mat3 result = previous( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input matrix
        /// \return The previous representable value for each component
        ///
        static ITK_INLINE typeMat3 previous(const typeMat3 &p) noexcept
        {
            return typeMat3(
                OP<type3>::previous(p[0]),
                OP<type3>::previous(p[1]),
                OP<type3>::previous(p[2]));
        }

        /// \brief Returns the next representable floating-point value after each component in the direction of a target.
        ///
        /// For each component of the matrix, returns the next floating-point value
        /// after p in the direction of _to.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 a;
        /// mat3 target;
        ///
        /// mat3 result = next_after( a, target );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input matrix
        /// \param _to The target direction matrix
        /// \return The next representable value for each component in the direction of _to
        ///
        static ITK_INLINE typeMat3 next_after(const typeMat3 &p, const typeMat3 &_to) noexcept
        {
            return typeMat3(
                OP<type3>::next_after(p[0], _to[0]),
                OP<type3>::next_after(p[1], _to[1]),
                OP<type3>::next_after(p[2], _to[2]));
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
        /// mat3 result;
        /// result = clamp( value, min, max );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param value The value to evaluate
        /// \param min The min threshold
        /// \param max The max threshold
        /// \return The evaluated value
        ///
        static ITK_INLINE typeMat3 clamp(const typeMat3 &value, const typeMat3 &min, const typeMat3 &max) noexcept
        {
            return typeMat3(
                OP<type3>::clamp(value[0], min[0], max[0]),
                OP<type3>::clamp(value[1], min[1], max[1]),
                OP<type3>::clamp(value[2], min[2], max[2]));
        }

        /// \brief Computes the dot product between two matrices
        ///
        /// The dot product of two matrices is the sum of the products of their
        /// corresponding components (the Frobenius inner product):
        ///
        /// dot(a, b) = sum over all components (a[i][j] * b[i][j])
        ///
        /// It is computed here as the sum of the dot products of the three
        /// columns of each matrix.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 a, b;
        /// float result = dot( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \return The dot product between the two matrices
        ///
        static ITK_INLINE _type dot(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            _type dota = OP<type3>::dot(a[0], b[0]);
            _type dotb = OP<type3>::dot(a[1], b[1]);
            _type dotc = OP<type3>::dot(a[2], b[2]);
            return dota + dotb + dotc;
        }

        /// \brief Normalize a matrix
        ///
        /// Returns a matrix with the same component ratios but a Frobenius norm
        /// (length) of one.
        ///
        /// result = m / |m|
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// mat3 m_normalized = normalize( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The matrix to normalize
        /// \return The normalized matrix
        ///
        static ITK_INLINE typeMat3 normalize(const typeMat3 &m) noexcept
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
        /// mat3 input;
        ///
        /// float result = sqrLength(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The matrix
        /// \return The squared length
        ///
        static ITK_INLINE _type sqrLength(const typeMat3 &a) noexcept
        {
            return self_type::dot(a, a);
        }

        /// \brief Computes the length of a matrix
        ///
        /// The length of a matrix 'a' is:
        ///
        /// |a|
        ///
        /// This computation uses the sqrt, and it consumes a lot of cycles to compute.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 input;
        ///
        /// float result = length(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The matrix
        /// \return The length
        ///
        static ITK_INLINE _type length(const typeMat3 &a) noexcept
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
        /// mat3 a, b;
        ///
        /// float result = sqrDistance( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \return The squared distance between a and b
        ///
        static ITK_INLINE _type sqrDistance(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            typeMat3 ab = b - a;
            return self_type::dot(ab, ab);
        }

        /// \brief Computes the distance between two matrices
        ///
        /// The distance is the Euclidean distance between the two matrices:
        ///
        /// |b-a|
        ///
        /// This computation uses the sqrt, and it consumes a lot of cycles to compute.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 a, b;
        ///
        /// float result = distance( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \return The distance between a and b
        ///
        static ITK_INLINE _type distance(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            typeMat3 ab = b - a;
            return OP<_type>::sqrt(self_type::dot(ab, ab));
        }

        /// \brief Computes the maximum value across all components of a matrix
        ///
        /// Returns the largest component value found in the matrix.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 a;
        ///
        /// float result = maximum( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The matrix
        /// \return The maximum value across all components
        ///
        static ITK_INLINE _type maximum(const typeMat3 &a) noexcept
        {
            type3 max_a = OP<type3>::maximum(a[0], a[1]);
            type3 max_c = OP<type3>::maximum(max_a, a[2]);
            return OP<type3>::maximum(max_c);
        }

        /// \brief Computes the component-wise maximum of two matrices
        ///
        /// For each component, returns the larger value between a and b.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 a, b;
        ///
        /// mat3 result = maximum( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \return The component-wise maximum of a and b
        ///
        static ITK_INLINE typeMat3 maximum(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            return typeMat3(OP<type3>::maximum(a[0], b[0]),
                            OP<type3>::maximum(a[1], b[1]),
                            OP<type3>::maximum(a[2], b[2]));
        }

        /// \brief Computes the minimum value across all components of a matrix
        ///
        /// Returns the smallest component value found in the matrix.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 a;
        ///
        /// float result = minimum( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The matrix
        /// \return The minimum value across all components
        ///
        static ITK_INLINE _type minimum(const typeMat3 &a) noexcept
        {
            type3 max_a = OP<type3>::minimum(a[0], a[1]);
            type3 max_c = OP<type3>::minimum(max_a, a[2]);
            return OP<type3>::minimum(max_c);
        }

        /// \brief Computes the component-wise minimum of two matrices
        ///
        /// For each component, returns the smaller value between a and b.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 a, b;
        ///
        /// mat3 result = minimum( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \return The component-wise minimum of a and b
        ///
        static ITK_INLINE typeMat3 minimum(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            return typeMat3(OP<type3>::minimum(a[0], b[0]),
                            OP<type3>::minimum(a[1], b[1]),
                            OP<type3>::minimum(a[2], b[2]));
        }

        /// \brief Computes the absolute value of each component of a matrix
        ///
        /// For each component, returns its absolute (non-negative) value.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 a;
        ///
        /// mat3 result = abs( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The matrix
        /// \return The matrix with each component replaced by its absolute value
        ///
        static ITK_INLINE typeMat3 abs(const typeMat3 &a) noexcept
        {
            return typeMat3(OP<type3>::abs(a[0]),
                            OP<type3>::abs(a[1]),
                            OP<type3>::abs(a[2]));
        }

        /// \brief Linearly interpolates between two matrices
        ///
        /// For each component, returns a value between a and b, weighted by factor:
        ///
        /// result = a * (1 - factor) + b * factor
        ///
        /// When factor is 0 the result is a, and when factor is 1 the result is b.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 a, b;
        /// float factor = 0.5f;
        ///
        /// mat3 result = lerp( a, b, factor );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix
        /// \param b The second matrix
        /// \param factor The interpolation factor (0 to 1)
        /// \return The interpolated matrix
        ///
        static ITK_INLINE typeMat3 lerp(const typeMat3 &a, const typeMat3 &b, const _type &factor) noexcept
        {
            //  return a+(b-a)*factor;
            return a * ((_type)1 - factor) + (b * factor);
        }

        /// \brief Barycentrically interpolates between three matrices
        ///
        /// Returns a weighted combination of the three matrices using barycentric
        /// coordinates u and v:
        ///
        /// result = v0 * (1 - u - v) + v1 * u + v2 * v
        ///
        /// The weights (1 - u - v), u and v should sum to 1.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 v0, v1, v2;
        /// float u = 0.25f, v = 0.25f;
        ///
        /// mat3 result = barylerp( u, v, v0, v1, v2 );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param u The first barycentric coordinate
        /// \param v The second barycentric coordinate
        /// \param v0 The first matrix (weight 1 - u - v)
        /// \param v1 The second matrix (weight u)
        /// \param v2 The third matrix (weight v)
        /// \return The barycentrically interpolated matrix
        ///
        static ITK_INLINE typeMat3 barylerp(const _type &u, const _type &v, const typeMat3 &v0, const typeMat3 &v1, const typeMat3 &v2) noexcept
        {
            // return v0*(1-uv[0]-uv[1])+v1*uv[0]+v2*uv[1];
            return v0 * ((_type)1 - u - v) + v1 * u + v2 * v;
        }

        /// \brief Bilinearly interpolates between four matrices
        ///
        /// Returns a weighted combination of the four matrices A, B, C and D using
        /// the interpolation factors dx and dy:
        ///
        /// result = (1-dx)(1-dy) * A + (1-dx)dy * D + dx(1-dy) * B + dx*dy * C
        ///
        /// A, B, C and D are the four corners of the interpolation region.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 A, B, C, D;
        /// float dx = 0.5f, dy = 0.5f;
        ///
        /// mat3 result = blerp( A, B, C, D, dx, dy );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param A The first corner matrix
        /// \param B The second corner matrix
        /// \param C The third corner matrix
        /// \param D The fourth corner matrix
        /// \param dx The interpolation factor along the first axis
        /// \param dy The interpolation factor along the second axis
        /// \return The bilinearly interpolated matrix
        ///
        static ITK_INLINE typeMat3 blerp(const typeMat3 &A, const typeMat3 &B, const typeMat3 &C, const typeMat3 &D,
                                         const _type &dx, const _type &dy) noexcept
        {
            _type omdx = (_type)1 - dx,
                  omdy = (_type)1 - dy;
            return (omdx * omdy) * A + (omdx * dy) * D + (dx * omdy) * B + (dx * dy) * C;
        }

        /// \brief Extracts the rotation part of a matrix
        ///
        /// Returns a matrix that keeps only the rotation components of the input
        /// matrix, discarding any translation or scaling. The first two columns
        /// (a1, b1, a2, b2) are preserved and the third column is set to (0, 0, 1).
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// mat3 rotation = extractRotation( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The matrix with only the rotation components
        ///
        static ITK_INLINE typeMat3 extractRotation(const typeMat3 &m) noexcept
        {
#if defined(ITK_SSE2)
            __m128 a = m.array_sse[0];
            __m128 b = m.array_sse[1];

            a = _mm_and_ps(a, _vec2_valid_bits_sse);
            b = _mm_and_ps(b, _vec2_valid_bits_sse);

            return typeMat3(a, b, _vec4_0010_sse);
#elif defined(ITK_NEON)
            const float32x2_t _zero_v2 = vdup_n_f32(0.0f);
            return typeMat3(
                vcombine_f32(vget_low_f32(m.array_neon[0]), _zero_v2),
                vcombine_f32(vget_low_f32(m.array_neon[1]), _zero_v2),
                _neon_0010);
            // return typeMat3(m.a1, m.b1, 0,
            //                 m.a2, m.b2, 0,
            //                 0, 0, 1);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Extracts the X axis (first column) of a matrix
        ///
        /// Returns the first column of the matrix as a vec3, which represents
        /// the X axis of the transformation.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// vec3 xAxis = extractXaxis( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The X axis of the matrix as a vec3
        ///
        static ITK_INLINE type3 extractXaxis(const typeMat3 &m) noexcept
        {
            return m[0];
        }

        /// \brief Extracts the Y axis (second column) of a matrix
        ///
        /// Returns the second column of the matrix as a vec3, which represents
        /// the Y axis of the transformation.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// vec3 yAxis = extractYaxis( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The Y axis of the matrix as a vec3
        ///
        static ITK_INLINE type3 extractYaxis(const typeMat3 &m) noexcept
        {
            return m[1];
        }

        /// \brief Extracts the Z axis (third column) of a matrix
        ///
        /// Returns the third column of the matrix as a vec3, which represents
        /// the Z axis of the transformation.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// vec3 zAxis = extractZaxis( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The Z axis of the matrix as a vec3
        ///
        static ITK_INLINE type3 extractZaxis(const typeMat3 &m) noexcept
        {
            return m[2];
        }

        /// \brief Extracts the translation part of a matrix
        ///
        /// Returns the third column of the matrix as a vec3, which represents
        /// the translation component of the transformation.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// vec3 translation = extractTranslation( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The translation of the matrix as a vec3
        ///
        static ITK_INLINE type3 extractTranslation(const typeMat3 &m) noexcept
        {
            return m[2];
        }

        /// \brief Computes the transpose of a matrix
        ///
        /// Returns a matrix where the rows and columns of the input matrix are
        /// swapped:
        ///
        /// result[i][j] = m[j][i]
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// mat3 mT = transpose( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The transposed matrix
        ///
        static ITK_INLINE typeMat3 transpose(const typeMat3 &m) noexcept
        {
#if defined(ITK_SSE2)
            __m128 tmp0 = _mm_shuffle_ps(m.array_sse[0], m.array_sse[1], _MM_SHUFFLE(1, 0, 1, 0));
            __m128 tmp2 = _mm_shuffle_ps(m.array_sse[0], m.array_sse[1], _MM_SHUFFLE(3, 2, 3, 2));
            __m128 tmp1 = _mm_shuffle_ps(m.array_sse[2], _vec4_0001_sse, _MM_SHUFFLE(1, 0, 1, 0));
            __m128 tmp3 = _mm_shuffle_ps(m.array_sse[2], _vec4_0001_sse, _MM_SHUFFLE(3, 2, 3, 2));

            return typeMat3(
                _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(2, 0, 2, 0)),
                _mm_shuffle_ps(tmp0, tmp1, _MM_SHUFFLE(3, 1, 3, 1)),
                _mm_shuffle_ps(tmp2, tmp3, _MM_SHUFFLE(2, 0, 2, 0)));
#elif defined(ITK_NEON)
            float32x4x2_t ab = vtrnq_f32(m.array_neon[0], m.array_neon[1]);
            float32x4x2_t cd = vtrnq_f32(m.array_neon[2], _neon_0001);
            float32x4_t a_ = vcombine_f32(vget_low_f32(ab.val[0]), vget_low_f32(cd.val[0]));
            float32x4_t b_ = vcombine_f32(vget_low_f32(ab.val[1]), vget_low_f32(cd.val[1]));
            float32x4_t c_ = vcombine_f32(vget_high_f32(ab.val[0]), vget_high_f32(cd.val[0]));

            return typeMat3(a_, b_, c_);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Computes the determinant of a matrix
        ///
        /// The determinant is a scalar value that can be used to determine,
        /// among other things, whether the matrix is invertible:
        ///
        /// - det != 0 => the matrix is invertible
        /// - det == 0 => the matrix is singular (not invertible)
        ///
        /// For a rotation matrix the determinant is 1.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// float det = determinant( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input matrix
        /// \return The determinant of the matrix
        ///
        static ITK_INLINE _type determinant(const typeMat3 &m) noexcept
        {
            type3 aux = type3(m.c3, m.b3, m.c2) * type3(m.b2, m.c1, m.b1) - type3(m.b3, m.c3, m.b2) * type3(m.c2, m.b1, m.c1);
            // return OP<type3>::dot(m[0], aux);
#if defined(ITK_SSE2)
            return _mm_f32_read_0(dot_sse_3(m.array_sse[0], aux.array_sse));
#elif defined(ITK_NEON)
            return vgetq_lane_f32(dot_neon_3(m.array_neon[0], aux.array_neon), 0);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Extracts the Euler angles (roll, pitch, yaw) from a rotation matrix
        ///
        /// Decomposes the rotation matrix into its three Euler angles, written
        /// through the output parameters. The angles are expressed in radians.
        ///
        /// The decomposition handles the gimbal-lock (singular) case, where the
        /// pitch is close to +/- 90 degrees and roll and yaw cannot be uniquely
        /// determined; in that case yaw is set to 0.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        /// float roll, pitch, yaw;
        ///
        /// extractEuler( m, &roll, &pitch, &yaw );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The input rotation matrix
        /// \param roll Output roll angle in radians
        /// \param pitch Output pitch angle in radians
        /// \param yaw Output yaw angle in radians
        ///
        static ITK_INLINE void extractEuler(const typeMat3 &m, _type *roll, _type *pitch, _type *yaw) noexcept
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

        /// \brief Moves a matrix towards a target by a limited distance
        ///
        /// Returns a matrix that is at most maxDistanceVariation away from the
        /// current matrix, in the direction of the target. If the target is
        /// closer than maxDistanceVariation, the result is the target itself.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 current, target;
        /// float maxDistanceVariation = 0.1f;
        ///
        /// mat3 result = move( current, target, maxDistanceVariation );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param current The current matrix
        /// \param target The target matrix
        /// \param maxDistanceVariation The maximum distance to move towards the target
        /// \return The matrix moved towards the target by at most maxDistanceVariation
        ///
        static ITK_INLINE typeMat3 move(const typeMat3 &current, const typeMat3 &target, const _type &maxDistanceVariation) noexcept
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

        /// \brief Computes the sign of each component of a matrix
        ///
        /// For each component, returns -1, 0 or 1 depending on whether the
        /// component is negative, zero or positive.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// mat3 result = sign( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The matrix
        /// \return The matrix with each component replaced by its sign
        ///
        static ITK_INLINE typeMat3 sign(const typeMat3 &m) noexcept
        {
            return typeMat3(
                OP<type3>::sign(m[0]),
                OP<type3>::sign(m[1]),
                OP<type3>::sign(m[2]));
        }

        /// \brief Computes the floor of each component of a matrix
        ///
        /// For each component, returns the largest integer value less than or
        /// equal to the component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// mat3 result = floor( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The matrix
        /// \return The matrix with each component replaced by its floor value
        ///
        static ITK_INLINE typeMat3 floor(const typeMat3 &m) noexcept
        {
            return typeMat3(
                OP<type3>::floor(m[0]),
                OP<type3>::floor(m[1]),
                OP<type3>::floor(m[2]));
        }

        /// \brief Computes the ceiling of each component of a matrix
        ///
        /// For each component, returns the smallest integer value greater than
        /// or equal to the component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// mat3 result = ceil( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The matrix
        /// \return The matrix with each component replaced by its ceiling value
        ///
        static ITK_INLINE typeMat3 ceil(const typeMat3 &m) noexcept
        {
            return typeMat3(
                OP<type3>::ceil(m[0]),
                OP<type3>::ceil(m[1]),
                OP<type3>::ceil(m[2]));
        }

        /// \brief Computes the rounded value of each component of a matrix
        ///
        /// For each component, returns the nearest integer value to the component.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 m;
        ///
        /// mat3 result = round( m );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param m The matrix
        /// \return The matrix with each component replaced by its rounded value
        ///
        static ITK_INLINE typeMat3 round(const typeMat3 &m) noexcept
        {
            return typeMat3(
                OP<type3>::round(m[0]),
                OP<type3>::round(m[1]),
                OP<type3>::round(m[2]));
        }

        /// \brief Computes the floating-point remainder of a divided by b, component-wise
        ///
        /// For each component, returns the remainder of the division a / b.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 a, b;
        ///
        /// mat3 result = fmod( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first matrix (dividend)
        /// \param b The second matrix (divisor)
        /// \return The component-wise floating-point remainder of a divided by b
        ///
        static ITK_INLINE typeMat3 fmod(const typeMat3 &a, const typeMat3 &b) noexcept
        {
            return typeMat3(
                OP<type3>::fmod(a[0], b[0]),
                OP<type3>::fmod(a[1], b[1]),
                OP<type3>::fmod(a[2], b[2]));
        }

        /// \brief Computes the step function of each component of a matrix
        ///
        /// For each component, returns 0 if the threshold is greater than the
        /// value, and 1 otherwise:
        ///
        /// result = (threshold < v) ? 0 : 1
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat3 threshold, v;
        ///
        /// mat3 result = step( threshold, v );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param threshold The threshold matrix
        /// \param v The value matrix
        /// \return The component-wise step function of threshold and v
        ///
        static ITK_INLINE typeMat3 step(const typeMat3 &threshold, const typeMat3 &v) noexcept
        {
            return typeMat3(
                OP<type3>::step(threshold[0], v[0]),
                OP<type3>::step(threshold[1], v[1]),
                OP<type3>::step(threshold[2], v[2]));
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
        /// mat3 edge0, edge1, x;
        ///
        /// mat3 result = smoothstep( edge0, edge1, x );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param edge0 The lower edge of the interpolation range
        /// \param edge1 The upper edge of the interpolation range
        /// \param x The value to interpolate
        /// \return The component-wise smoothstep value, clamped to [0, 1]
        ///
        static ITK_INLINE typeMat3 smoothstep(const typeMat3 &edge0, const typeMat3 &edge1, const typeMat3 &x) noexcept
        {
            using type_info = FloatTypeInfo<_type>;
            typeMat3 dir = edge1 - edge0;

            _type length_dir = OP<_type>::maximum(self_type::length(dir), type_info::min);

            typeMat3 value = x - edge0;
            value *= (_type)1 / length_dir;

            typeMat3 t = self_type::clamp(value, typeMat3((_type)0), typeMat3((_type)1));
            return t * t * ((_type)3 - (_type)2 * t);
            // return typeMat4(
            //     OP<type3>::smoothstep(edge0[0], edge1[0], x[0]),
            //     OP<type3>::smoothstep(edge0[1], edge1[1], x[1]),
            //     OP<type3>::smoothstep(edge0[2], edge1[2], x[2]));
        }
    };

}