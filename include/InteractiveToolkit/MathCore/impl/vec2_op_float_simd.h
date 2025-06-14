#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'vec2_op_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "vec2_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    template <typename _type, typename _simd, typename _algorithm>
    struct OP<vec2<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                  (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                   std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type,
              _algorithm>
    {
    private:
        using type2 = vec2<_type, _simd>;
        using self_type = OP<type2>;

    public:
        /// \brief Clamp values in a component wise fashion
        ///
        /// For each component of the vector, evaluate:
        /// ```
        ///     if min < value then return min
        ///     if max > value then return max
        ///     else return value
        /// ```
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 result;
        /// // result = vec2 ( 50, 3 )
        /// result = clamp( vec2( 300 , 3 ), vec2( 0, -1 ) , vec2( 50, 5 ) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param value The value to evaluate
        /// \param min The min threshold
        /// \param max The max threshold
        /// \return The evaluated value
        ///
        static ITK_INLINE type2 clamp(const type2 &value, const type2 &min, const type2 &max) noexcept
        {
#if defined(ITK_SSE2)
            return clamp_sse_4(value.array_sse, min.array_sse, max.array_sse);
#elif defined(ITK_NEON)
            return clamp_neon_4_v2(value.array_neon, min.array_neon, max.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Computes the dot product between two vectors
        ///
        /// The dot product is a single value computed from the two vectors.
        ///
        /// <pre>
        /// It can be interpreted as:
        ///
        /// -The squared length of the vector modulos
        ///  when the two vectors are the same:
        ///    dot(a,a) = |a|^2 = x^2+y^2+z^2
        ///
        /// -The length of a projection
        ///  when some vector is an unit vector:
        ///    dot(a,UnitV) = dot(UnitV,a) =
        ///    signed length of 'a' after the projection in the UnitV
        /// =================================================================
        ///      / a
        ///     /
        ///     ----->UnitV
        ///
        ///      /
        ///     /
        ///     -- projected 'a' over UnitV
        ///    |  |
        ///    length of the projection with positive sign
        /// =================================================================
        ///   \ a
        ///    \                                                            .
        ///     ----->UnitV
        ///
        ///   \                                                             .
        ///    \                                                            .
        ///   -- projected 'a' over UnitV
        ///  |  |
        ///  length of the projection with negative sign
        /// =================================================================
        /// -The hemisfere side of two vectors:
        ///    dot(a,b) = dot(b,a) = value
        ///        -- value < 0 => they are in the opose direction.
        ///        -- value > 0 => they are in the same direction.
        ///        -- value = 0 => they are orthogonal(90 degrees) vectors.
        ///
        /// -The dot product computes the equation:
        ///    dot(a,b) = cos(angle between a and b) * |a| * |b|
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a, b;
        /// float result = dot( a, b ) ;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first vector
        /// \param b The second vector
        /// \return The dot product between the two vectors
        ///
        static ITK_INLINE _type dot(const type2 &a, const type2 &b) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            __m128 dp = _mm_mul_ps(a.array_sse, b.array_sse);
            dp = _mm_hadd_ps(dp, dp);
#else
            __m128 dp = _mm_dp_ps(a.array_sse, b.array_sse, 0x33);
#endif
            return _mm_f32_read_0(dp);
#elif defined(ITK_NEON)
            float32x2_t rc = dot_neon_2(a.array_neon, b.array_neon);
            // float32x2_t mul0 = vmul_f32(a.array_neon, b.array_neon);
            // float32x2_t sum = vpadd_f32(mul0, mul0);
            return vget_lane_f32(rc, 0);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Normalize a vector
        ///
        /// Returns a unit vector in the same direction of the parameter.
        ///
        /// result = vec/|vec|
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a;
        ///
        /// vec2 a_normalized = normalize( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param vec The vector to normalize
        /// \return The unit vector
        ///
        static ITK_INLINE type2 normalize(const type2 &vec) noexcept
        {
            _type mag2 = self_type::dot(vec, vec);
            _type mag2_rsqrt = OP<_type, void, _algorithm>::rsqrt(mag2);
            return vec * mag2_rsqrt;
        }

        /// \brief Compute the angle in radians between two vectors
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a = vec2( 1, 0 );
        /// vec2 b = vec2( 0, 10 );
        ///
        /// float angle_radians = angleBetween(a, b);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The source vector
        /// \param b The target vector
        /// \return Angle in radians
        ///
        static ITK_INLINE _type angleBetween(const type2 &a, const type2 &b) noexcept
        {
            _type cosA = OP<_type>::clamp(
                self_type::dot(self_type::normalize(a), self_type::normalize(b)),
                (_type)-1, (_type)1);
            return OP<_type>::acos(cosA);
        }

        static ITK_INLINE _type cross_z_mag(const type2 &a, const type2 &b) noexcept
        {
            return (a.x * b.y - a.y * b.x);
        }

        /// \brief Computes the orientation of three points in 2D space
        ///
        /// The orientation is computed as the cross product of the vectors formed by the points.
        /// It returns a positive value if the points are oriented counter-clockwise,
        /// a negative value if they are oriented clockwise, and zero if they are collinear.
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first point
        /// \param b The second point
        /// \param c The third point
        /// \return A positive value if the points are oriented counter-clockwise,
        ///         a negative value if they are oriented clockwise, and zero if they are collinear.
        ///
        static ITK_INLINE _type orientation(const type2 &a, const type2 &b, const type2 &c) noexcept
        {
            type2 ab = b - a;
            type2 ac = c - a;
            return cross_z_mag(ab, ac);
        }

        /// \brief Checks if a point is inside a triangle defined by three points in 2D space
        /// 
        /// The function uses the orientation of the points to determine if the point is inside the triangle.
        ///
        /// \author Alessandro Ribeiro
        /// \param p The point to check
        /// \param a The first vertex of the triangle
        /// \param b The second vertex of the triangle
        /// \param c The third vertex of the triangle
        /// \return true if the point is inside the triangle, false otherwise
        ///
        static ITK_INLINE bool point_inside_triangle(const type2 &p,
                                                     const type2 &a, const type2 &b, const type2 &c)
        {
            float d1 = orientation(p, a, b);
            float d2 = orientation(p, b, c);
            float d3 = orientation(p, c, a);

            bool has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
            bool has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

            return !(has_neg && has_pos);
        }

        static ITK_INLINE type2 cross_z_up(const type2 &a) noexcept
        {
            return type2(-a.y, a.x);
        }

        static ITK_INLINE type2 cross_z_down(const type2 &a) noexcept
        {
            return type2(a.y, -a.x);
        }

        /// \brief Computes the reflected vector 'a' related to a normal N
        ///
        /// The reflection of a vector is another vector with the same length, but the direction is
        /// modified by the surface normal (after hit the surface).
        ///
        /// <pre>
        ///          \  |
        ///           \ |w
        ///           a\\|a
        /// Normal <----|l
        ///            /|l
        ///           / |
        ///          /  |
        /// reflected   |
        ///
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a = vec2( 1 , -1 );
        /// vec2 normal = vec2( 0 ,1 );
        ///
        /// // result  = vec2( 1, 1 )
        /// vec2 reflected = reflect(a, normal);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The incident vector
        /// \param N The normal of a surface (unit vector)
        /// \return The reflected vector 'a' considering the normal N
        ///
        static ITK_INLINE type2 reflect(const type2 &a, const type2 &N) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            __m128 dt = _mm_mul_ps(a.array_sse, N.array_sse);
            dt = _mm_hadd_ps(dt, dt);
#else
            __m128 dt = _mm_dp_ps(a.array_sse, N.array_sse, 0x33);
#endif
            __m128 mul0 = _mm_mul_ps(dt, N.array_sse);
            __m128 mul1 = _mm_mul_ps(mul0, _vec3_two_sse);
            return _mm_sub_ps(a.array_sse, mul1);
#elif defined(ITK_NEON)

            float32x2_t rc = dot_neon_2(a.array_neon, N.array_neon);
            rc = vmul_f32(rc, vset1_v2((_type)2));
            rc = vmul_f32(rc, N.array_neon);

            return vsub_f32(a.array_neon, rc);
            // return (a - N * ((_type)2 * self_type::dot(a, N)));
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief snell law refraction, vector implementation
        ///
        /// from input ray, normal, ni and nr calculate the refracted vector
        /// ni = source index of refraction (iOr)
        /// nr = target index of refraction (iOr)
        ///
        /// The function can lead to return false when occurs the total internal reflection. <br />
        /// This case may occurrs when you exit a ray from a more dense environment to a less dense environment.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// float ior_air = 1.0f;
        /// float ior_water = 1.33f;
        /// vec2 rayDirection = normalize( vec2( 1 , -1 ) );
        /// vec2 normal = vec2( 0 ,1 );
        ///
        /// vec2 refracted;
        ///
        /// // compute the refracted ray that comes from air to the water surface
        /// if ( refract(rayDirection, normal, ior_air, ior_water, &refracted) ){
        ///     // the refracted has the value of the direction in the second environment
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param rayDir Incident ray direction
        /// \param normal The normal of a surface (unit vector)
        /// \param ni source index of refraction
        /// \param nr target index of refraction
        /// \param vOut output vector
        /// \return true if vector is calculated, false if this is a total internal reflection case
        static ITK_INLINE bool refract_old(const type2 &rayDir, const type2 &normal, const _type &ni, const _type &nr, type2 *vOut)
        {

            type2 L = self_type::normalize(-rayDir);

            _type ni_nr = ni / nr;
            _type cos_i = self_type::dot(normal, L);

            _type cos_r = (_type)1 - ni_nr * ni_nr * ((_type)1 - cos_i * cos_i);

            if (cos_r <= 0)
                return false;

            cos_r = OP<_type>::sqrt(cos_r);
            type2 T = (ni_nr * cos_i - cos_r) * normal - ni_nr * L;
            *vOut = self_type::normalize(T);
            return true;
        }

        static ITK_INLINE type2 refract(const type2 &rayDir, const type2 &normal, const _type &ni, const _type &nr)
        {

            type2 L = self_type::normalize(-rayDir);

            _type ni_nr = ni / nr;
            _type cos_i = self_type::dot(normal, L);

            _type cos_r = (_type)1 - ni_nr * ni_nr * ((_type)1 - cos_i * cos_i);

            // if (cos_r <= 0)
            //     return false;
            _type select = OP<_type>::step(cos_r, 0);

            cos_r = OP<_type>::sqrt(cos_r);
            type2 T = (ni_nr * cos_i - cos_r) * normal - ni_nr * L;
            T = self_type::normalize(T);

            type2 rlct = self_type::reflect(rayDir, normal);

            return self_type::lerp(T, rlct, select);
        }

        /// \brief Computes the squared length of a vector
        ///
        /// The squared length of a vector 'a' is:
        ///
        /// |a|^2
        ///
        /// It is cheaper to compute this value than the length of 'a'.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 input;
        ///
        /// float result = sqrLength(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The Vector
        /// \return The squared length
        ///
        static ITK_INLINE _type sqrLength(const type2 &a) noexcept
        {
            return self_type::dot(a, a);
        }

        /// \brief Computes the length of a vector
        ///
        /// The length of a vector 'a' is:
        ///
        /// |a|
        ///
        /// This computation uses the sqrtf, and it consumes a lot of cicles to compute.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 input;
        ///
        /// float result = length(input);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The Vector
        /// \return The length
        ///
        static ITK_INLINE _type length(const type2 &a) noexcept
        {
            return OP<_type>::sqrt(self_type::dot(a, a));
        }

        /// \brief Computes the squared distance between two vectors
        ///
        /// The squared distance is the euclidian distance, without the square root:
        ///
        /// |b-a|^2
        ///
        /// It is cheaper to compute this value than the distance from 'a' to 'b'.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a, b;
        ///
        /// float result = sqrDistance( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The First vector
        /// \param b The Second vector
        /// \return The squared distance between a and b
        ///
        static ITK_INLINE _type sqrDistance(const type2 &a, const type2 &b) noexcept
        {
            type2 ab = b - a;
            return self_type::dot(ab, ab);
        }

        /// \brief Computes the distance between two vectors
        ///
        /// The squared distance is the euclidian distance:
        ///
        /// |b-a|
        ///
        /// This computation uses the sqrtf, and it consumes a lot of cicles to compute.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 a, b;
        ///
        /// float result = distance( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The First vector
        /// \param b The Second vector
        /// \return The distance between a and b
        ///
        static ITK_INLINE _type distance(const type2 &a, const type2 &b) noexcept
        {
            type2 ab = b - a;
            return OP<_type>::sqrt(self_type::dot(ab, ab));
        }

        /// \brief Computes the projection of a vector over a unit vector
        ///
        /// The projection result is a vector parallel to the unitV
        ///
        /// <pre>
        /// ex.:
        ///         /                 |
        ///     a  /      | unitV     | vOut
        ///       o       o           o
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a, unitV;
        ///
        /// unitV = normalize( unitV );
        ///
        /// vec2 vOout = parallelComponent( a, unitV );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The vector to decompose
        /// \param unitV The base vector (must be unit)
        /// \return The projection of 'a' over unitV
        ///
        static ITK_INLINE type2 parallelComponent(const type2 &a, const type2 &unitV) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            __m128 dot0 = _mm_mul_ps(a.array_sse, unitV.array_sse);
            dot0 = _mm_hadd_ps(dot0, dot0);
#else
            __m128 dot0 = _mm_dp_ps(a.array_sse, unitV.array_sse, 0x33);
#endif
            __m128 mul0 = _mm_mul_ps(unitV.array_sse, dot0);
            return mul0;
#elif defined(ITK_NEON)
            float32x2_t rc = dot_neon_2(a.array_neon, unitV.array_neon);
            return vmul_f32(rc, unitV.array_neon);
            // return unitV * (self_type::dot(a, unitV));
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Computes a vector perpendicular to the projection of a vector over a unit vector
        ///
        /// The vector perpendicular to the projection result is a vector normal to the unitV
        ///
        /// <pre>
        /// ex.:
        ///         /
        ///     a  /      | unitV
        ///       o       o           o-- vOut
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a, unitV;
        ///
        /// unitV = normalize( unitV );
        ///
        /// vec2 vOout = perpendicularComponent( a, unitV );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The vector to decompose
        /// \param unitV The base vector (must be unit)
        /// \return The normal to the projection of 'a' over unitV
        ///
        static ITK_INLINE type2 perpendicularComponent(const type2 &a, const type2 &unitV) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            __m128 dot0 = _mm_mul_ps(a.array_sse, unitV.array_sse);
            dot0 = _mm_hadd_ps(dot0, dot0);
#else
            __m128 dot0 = _mm_dp_ps(a.array_sse, unitV.array_sse, 0x33);
#endif
            __m128 mul0 = _mm_mul_ps(unitV.array_sse, dot0);
            __m128 sub = _mm_sub_ps(a.array_sse, mul0);
            return sub;
#elif defined(ITK_NEON)
            float32x2_t rc = dot_neon_2(a.array_neon, unitV.array_neon);
            rc = vmul_f32(rc, unitV.array_neon);
            return vsub_f32(a.array_neon, rc);
            // return a - unitV * (self_type::dot(a, unitV));
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Computes both: a vector perpendicular and a parallel to the projection of a vector over a unit vector
        ///
        /// The vector perpendicular to the projection result is a vector normal to the unitV
        ///
        /// <pre>
        /// ex.:
        ///         /
        ///     a  /      | unitV
        ///       o       o           o-- perpendicular
        /// </pre>
        ///
        /// The projection result is a vector parallel to the unitV
        ///
        /// <pre>
        /// ex.:
        ///         /                 |
        ///     a  /      | unitV     | parallel
        ///       o       o           o
        /// </pre>
        ///
        /// This function do a vector decomposition in two other vectors according the unitV.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a, unitV;
        ///
        /// unitV = normalize( unitV );
        ///
        /// vec2 perpendicular, parallel;
        ///
        /// vecDecomp( a, unitV, &perpendicular, &parallel );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The vector to decompose
        /// \param unitV The base vector (must be unit)
        /// \param perpendicular It is a return parameter, thats will hold the computed perpendicular vector
        /// \param parallel It is a return parameter, thats will hold the computed parallel vector
        ///
        static ITK_INLINE void vecDecomp(const type2 &a, const type2 &unitV,
                                         type2 *perpendicular, type2 *parallel) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            __m128 dot0 = _mm_mul_ps(a.array_sse, unitV.array_sse);
            dot0 = _mm_hadd_ps(dot0, dot0);
#else
            __m128 dot0 = _mm_dp_ps(a.array_sse, unitV.array_sse, 0x33);
#endif
            __m128 mul0 = _mm_mul_ps(unitV.array_sse, dot0);
            __m128 sub = _mm_sub_ps(a.array_sse, mul0);

            parallel->array_sse = mul0;
            perpendicular->array_sse = sub;
#elif defined(ITK_NEON)

            float32x2_t parallel_ = dot_neon_2(a.array_neon, unitV.array_neon);
            parallel_ = vmul_f32(parallel_, unitV.array_neon);

            float32x2_t perp_ = vsub_f32(a.array_neon, parallel_);

            parallel->array_neon = parallel_;
            perpendicular->array_neon = perp_;

            //*parallel = unitV * (self_type::dot(a, unitV));
            //*perpendicular = a - *parallel;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Test a point and clips its values according the euclidian distance from another point
        ///
        /// The quadratic clamp can be used to make limits like circular limits or spherical limits.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 center = vec2( 0, 0 );
        /// float maxRadius = 10.0f;
        ///
        /// vec2 point = vec2( 100, 0 );
        ///
        /// // result = vec2 ( 10, 0 )
        /// vec2 result = quadraticClamp( center, point, maxRadius );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param point The point to clip
        /// \param center The center to compute the euclidian distance
        /// \param maxRadius The max distance that the point can be from the center
        /// \return The point if it is below the distance or a point in the same line of the point, but with distance from center equals to maxRadius
        ///
        static ITK_INLINE type2 quadraticClamp(const type2 &center, const type2 &point, const _type maxRadius) noexcept
        {
            using type_info = FloatTypeInfo<_type>;

            type2 direction = point - center;

            _type length = self_type::length(direction);
            _type length_avoid_div_zero = OP<_type>::maximum(length, type_info::min);

            // normalize direction
            direction *= (_type)1 / length_avoid_div_zero;

            _type select_result = OP<_type>::step(maxRadius, length);

            return self_type::lerp(point, center + direction * maxRadius, select_result);

            // if (length > maxRadius)
            //     return center + direction * (maxRadius / length);
            // return point;
        }

        /// \brief Return the greater value from the parameter
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 input;
        ///
        /// float max = maximum( input );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Set of values to test
        /// \return The greater value from the parameter
        ///
        static ITK_INLINE _type maximum(const type2 &a) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_f32_read_0(max_sse_2(a.array_sse));
#elif defined(ITK_NEON)
            float32x2_t max_neon = vmax_f32(a.array_neon,
                                            vdup_lane_f32(a.array_neon, 1)
                                            // vshuffle_1111(a.array_neon)
            );
            return vget_lane_f32(max_neon, 0);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Component-wise maximum value from two vectors
        ///
        ///  Return the maximum value considering each component of the vector.
        ///
        /// result: vec2( maximum(a.x,b.x), maximum(a.y,b.y) )
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a, b;
        ///
        /// vec2 result = maximum( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A vector
        /// \param b A vector
        /// \return The maximum value for each vector component
        ///
        static ITK_INLINE type2 maximum(const type2 &a, const type2 &b) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_max_ps(a.array_sse, b.array_sse);
#elif defined(ITK_NEON)
            return vmax_f32(a.array_neon, b.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Return the smaller value from the parameter
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a;
        ///
        /// float result = minimum( a );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Set of values to test
        /// \return The smaller value from the parameter
        ///
        static ITK_INLINE _type minimum(const type2 &a) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_f32_read_0(min_sse_2(a.array_sse));
#elif defined(ITK_NEON)
            float32x2_t min_neon = vmin_f32(a.array_neon,
                                            vdup_lane_f32(a.array_neon, 1)
                                            // vshuffle_1111(a.array_neon)
            );
            return vget_lane_f32(min_neon, 0);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Component-wise minimum value from two vectors
        ///
        ///  Return the minimum value considering each component of the vector.
        ///
        /// result: vec2( minimum(a.x,b.x), minimum(a.y,b.y) )
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a, b;
        ///
        /// vec2 result = minimum( a, b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A vector
        /// \param b A vector
        /// \return The minimum value for each vector component
        ///
        static ITK_INLINE type2 minimum(const type2 &a, const type2 &b) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_min_ps(a.array_sse, b.array_sse);
#elif defined(ITK_NEON)
            return vmin_f32(a.array_neon, b.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Compute the absolute value of a vector (magnitude)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 input = vec2( -10, 20 );
        ///
        /// // result = vec2( 10, 20 )
        /// vec2 result = absv( input );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a A vector
        /// \return vec2( |a.x|, |a.y| )
        ///
        static ITK_INLINE type2 abs(const type2 &a) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_andnot_ps(_vec4_sign_mask_sse, a.array_sse);
#elif defined(ITK_NEON)
            return vabs_f32(a.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Computes the linear interpolation
        ///
        /// When the fator is between 0 and 1, it returns the convex relation (linear interpolation) between a and b.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a = vec2( 0.0f );
        /// vec2 b = vec2( 100.0f );
        ///
        /// // result = vec2( 75.0f, 75.0f )
        /// vec2 result = lerp( a, b, 0.75f );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Origin Vector
        /// \param b Target Vector
        /// \param factor The amount (%) to leave the Origin to the Target.
        /// \return The interpolation result
        ///
        static ITK_INLINE type2 lerp(const type2 &a, const type2 &b, const _type &factor) noexcept
        {
            //  return a+(b-a)*fator;
            return a * ((_type)1 - factor) + (b * factor);
        }

        /// \brief Compute the spherical linear interpolation between 2 vec3
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 a = vec2( 1, 0 );
        /// vec2 b = vec2( 0, 10 );
        ///
        /// // 75% spherical interpolation from a to b
        /// vec2 result = slerp(a, b, 0.75f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The source vector
        /// \param b The target vector
        /// \param lerp The amount of interpolation
        /// \return The spherical interpolation of the source and target vectors
        ///
        static ITK_INLINE type2 slerp(const type2 &a, const type2 &b, const _type &_lerp) noexcept
        {
            _type lerp = OP<_type>::clamp(_lerp, (_type)0, (_type)1);

            _type _cos = OP<_type>::clamp(
                self_type::dot(self_type::normalize(a), self_type::normalize(b)),
                (_type)-1, (_type)1);
            _type angle_rad = OP<_type>::acos(_cos);

            // sin is always positive
            //_type _sin = OP<_type>::sin(angle_rad);
            _type _sin = OP<_type>::sqrt((_type)1 - _cos * _cos);

            // if (OP<_type>::abs(_sin) < EPSILON<_type>::high_precision)
            //     return self_type::lerp(a, b, lerp);

            // _type _sin_abs = OP<_type>::abs(_sin);
            // _type aux = OP<_type>::maximum(_sin_abs, FloatTypeInfo<_type>::min );
            // _sin = OP<_type>::copy_sign( aux, _sin );

            _type select = OP<_type>::step(_sin, EPSILON<_type>::high_precision);
            _sin = OP<_type>::maximum(_sin, FloatTypeInfo<_type>::min);
            _type _1_over_sin = (_type)1 / _sin;
            _type a_factor = OP<_type>::sin(((_type)1 - lerp) * angle_rad) * _1_over_sin;
            _type b_factor = OP<_type>::sin(lerp * angle_rad) * _1_over_sin;

            // select a_factor and b_factor
            //_type select = OP<float>::step(_sin_abs, EPSILON<_type>::high_precision);
            a_factor = OP<_type>::lerp(a_factor, (_type)1 - lerp, select);
            b_factor = OP<_type>::lerp(b_factor, lerp, select);

            return a * a_factor + b * b_factor;
        }

        /// \brief Computes the result of the interpolation based on the baricentric coordinate (uv) considering 3 points
        ///
        /// It is possible to discover the value of 'u' and 'v' by using the triangle area formula.
        ///
        /// After that it is possible to use this function to interpolate normals, colors, etc... based on the baricentric coorginate uv
        ///
        /// Note: If the uv were calculated in euclidian space of a triangle, then interpolation of colors, normals or coordinates are not affected by the perspective projection.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
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
        /// vec2 colorA, colorB, colorC;
        ///
        /// vec2 colorResult = barylerp(u, v, colorA, colorB, colorC);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param u The u component of a baricentric coord
        /// \param v The v component of a baricentric coord
        /// \param v0 The first vector to interpolate
        /// \param v1 The second vector to interpolate
        /// \param v2 The third vector to interpolate
        /// \return A vector interpolated based on uv considering the 3 vectors of the parameter
        ///
        static ITK_INLINE type2 barylerp(const _type &u, const _type &v, const type2 &v0, const type2 &v1, const type2 &v2) noexcept
        {
            // return v0*(1-uv[0]-uv[1])+v1*uv[0]+v2*uv[1];
            return v0 * ((_type)1 - u - v) + v1 * u + v2 * v;
        }

        /// \brief Computes the result of the bilinear interpolation over a square patch with 4 points
        ///
        /// The bilinear interpolation is usefull to compute colors between pixels in a image.
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
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 dataA = vec2(0,0);
        /// vec2 dataB = vec2(1,0);
        /// vec2 dataC = vec2(1,1);
        /// vec2 dataD = vec2(0,1);
        ///
        /// // result = vec2( 0.5f, 0.5f )
        /// vec2 result = blerp(dataA,dataB,dataC,dataD,0.5f,0.5f);
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param A The lower-left vector
        /// \param B The lower-right vector
        /// \param C The upper-right vector
        /// \param D The upper-left vector
        /// \param dx The x axis interpolation factor
        /// \param dy The y axis interpolation factor
        /// \return A vector interpolated based on dxdy considering the 4 vectors of the parameter
        ///
        static ITK_INLINE type2 blerp(const type2 &A, const type2 &B, const type2 &C, const type2 &D,
                                      const _type &dx, const _type &dy) noexcept
        {
            _type omdx = (_type)1 - dx,
                  omdy = (_type)1 - dy;
            return (omdx * omdy) * A + (omdx * dy) * D + (dx * omdy) * B + (dx * dy) * C;
        }

        /// \brief Computes the result of the spline interpolation using the CatmullRom aproach
        ///
        /// The spline is a curve based in four points. The CatmullRom aproach makes the curve walk through the control points.
        ///
        /// It can be used to make smooth curves in paths.
        ///
        /// The values interpolated will be between the 2nd and 3rd control points.
        ///
        /// <pre>
        /// t - [0..1]
        ///
        /// q(t) = 0.5 * (1.0f,t,t2,t3)  *
        ///
        /// [  0  2  0  0 ]  [P0]
        /// [ -1  0  1  0 ]* [P1]
        /// [  2 -5  4 -1 ]  [P2]
        /// [ -1  3 -3  1 ]  [P3]
        ///
        /// q(t) = 0.5 *((2 * P1) +
        ///              (-P0 + P2) * t +
        ///              (2*P0 - 5*P1 + 4*P2 - P3) * t2 +
        ///              (-P0 + 3*P1- 3*P2 + P3) * t3)
        /// </pre>
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec2 p0,p1,p2,p3;
        ///
        /// //the result is inside the range of p1 (0%) to p2 (100%)
        /// vec2 result = splineCatmullRom(p0,p1,p2,p3,0.75f);
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param P0 The 1st control point
        /// \param P1 The 2nd control point
        /// \param P2 The 3rd control point
        /// \param P3 The 4th control point
        /// \param t The interpolation value
        /// \return A vector interpolated based on t considering the 4 control points
        ///
        static ITK_INLINE type2 splineCatmullRom(const type2 &P0, const type2 &P1, const type2 &P2, const type2 &P3, const _type &t) noexcept
        {
            _type tt = t * t;
            _type ttt = tt * t;
            return (_type)0.5 * (((_type)2 * P1) +
                                 (P2 - P0) * t +
                                 ((_type)2 * P0 - (_type)5 * P1 + (_type)4 * P2 - P3) * (tt) +
                                 ((_type)3 * P1 - P0 - (_type)3 * P2 + P3) * (ttt));
        }

        /// \brief Move from current to target, considering the max variation
        ///
        /// This function could be used as a constant motion interpolation<br />
        /// between two values considering the delta time and max speed variation.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// PlatformTime timer;
        /// float moveSpeed;
        /// vec2 current;
        /// vec2 target;
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
        static ITK_INLINE type2 move(const type2 &current, const type2 &target, const _type &maxDistanceVariation) noexcept
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

        /// \brief Move from current to target, considering the max variation in angle.
        ///
        /// This function could be used as a constant angular motion interpolation<br />
        /// between two values considering the delta time and max angle speed variation.
        ///
        /// Uses the spherical linear interpolation function.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// PlatformTime timer;
        /// float moveSpeedAngle;
        /// vec2 current;
        /// vec2 target;
        ///
        /// {
        ///     timer.update();
        ///     ...
        ///     // current will be modified to be the target,
        ///     // but the delta time and move speed will make
        ///     // this transition smoother.
        ///     current = moveSlerp( current, target, time.deltaTime * moveSpeedAngle );
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param currentParam The current state
        /// \param targetParam The target state
        /// \param maxAngleVariation The max amount the current can be modified to reach target
        /// \return the slerp from current to target according max variation
        ///
        static ITK_INLINE type2 moveSlerp(const type2 &currentParam, const type2 &targetParam, const _type &maxAngleVariation) noexcept
        {
            type2 current = currentParam;
            type2 target = targetParam;
            // const float EPSILON = 1e-6f;

            _type lengthCurrent = OP<type2>::length(current);
            _type lengthTarget = OP<type2>::length(target);

            // //trying to interpolate from zero vector
            // if (lengthCurrent < EPSILON2)
            //     return targetParam;
            // //trying to interpolate to zero vector
            // if (lengthTarget < EPSILON2)
            //     return currentParam;

            // avoid division by 0
            lengthCurrent = OP<_type>::maximum(lengthCurrent, FloatTypeInfo<_type>::min);
            lengthTarget = OP<_type>::maximum(lengthTarget, FloatTypeInfo<_type>::min);

            current *= ((_type)1 / lengthCurrent);
            target *= ((_type)1 / lengthTarget);

            // float deltaAngle = angleBetween(current, target);
            _type deltaAngle = OP<_type>::acos(OP<_type>::clamp(OP<type2>::dot(current, target), (_type)-1, (_type)1));
            // if (deltaAngle < maxAngleVariation + EPSILON)
            //     return target;
            deltaAngle = OP<_type>::maximum(deltaAngle, maxAngleVariation);
            deltaAngle = OP<_type>::maximum(deltaAngle, FloatTypeInfo<_type>::min);

            // how to generate these values:
            //
            // mat2<double,SIMD_TYPE::NONE> rot = GEN<mat2<double,SIMD_TYPE::NONE>>::zRotate(
            //     OP<double>::deg_2_rad(0.125));
            // vec2<double, SIMD_TYPE::NONE> a_v2(1,0), b_v2(-1,0);
            // printf("_cns_x: %e \n", rot.a1);
            // printf("_cns_y: %e \n", rot.a2);
            // a_v2 = rot * a_v2;
            // printf("_cns_deltaAngle(rad): %f \n", OP<decltype(a_v2)>::angleBetween(a_v2,b_v2));
            // printf("_cns_deltaAngle(deg): %f \n", OP<double>::rad_2_deg(OP<decltype(a_v2)>::angleBetween(a_v2,b_v2)));
            const _type _cns_x = (_type)9.999976e-01;
            const _type _cns_y = (_type)2.181660e-03;
            const _type _cns_deltaAngle = (_type)3.139411;
            // 180º case interpolation -- force one orientation based on eulerAngle
            // if (deltaAngle >= CONSTANT<_type>::PI - EPSILON<_type>::high_precision)
            // if (deltaAngle >= _cns_deltaAngle)
            // {
            //     _type auxX = current.x * _cns_x - current.y * _cns_y;
            //     _type auxY = current.x * _cns_y + current.y * _cns_x;
            //     current.x = auxX;
            //     current.y = auxY;
            //     deltaAngle = _cns_deltaAngle;
            // }

            // branch-less implementation
            _type select_delta_angle = OP<_type>::step(_cns_deltaAngle, deltaAngle);
            type2 _current_180_deg_case(
                current.x * _cns_x - current.y * _cns_y,
                current.x * _cns_y + current.y * _cns_x);
            current = OP<type2>::lerp(current, _current_180_deg_case, select_delta_angle);
            deltaAngle = OP<_type>::lerp(deltaAngle, _cns_deltaAngle, select_delta_angle);

            _type lrpFactor = maxAngleVariation / deltaAngle;
            type2 result = OP<type2>::slerp(current, target, lrpFactor);

            result = OP<type2>::normalize(result) * OP<_type>::lerp(lengthCurrent, lengthTarget, lrpFactor);

            return result;
        }

        /// \brief The sign of each component. ( v >= 0 ) ? 1 : -1
        ///
        static ITK_INLINE type2 sign(const type2 &v) noexcept
        {
#if defined(ITK_SSE2)
            __m128 sign_aux = _mm_and_ps(v.array_sse, _vec2_sign_mask_sse);
            __m128 sign = _mm_or_ps(sign_aux, _vec2_one_sse);
            return sign;
#elif defined(ITK_NEON)
            const int32x2_t v2_mask = vdup_n_s32(0x80000000);
            const int32x2_t v2_one = vreinterpret_s32_f32(vdup_n_f32(1.0f));

            int32x2_t sign_aux = vreinterpret_s32_f32(v.array_neon);
            sign_aux = vand_s32(sign_aux, v2_mask);
            int32x2_t sign = vorr_s32(sign_aux, v2_one);

            return vreinterpret_f32_s32(sign);
            // return type2(
            //     OP<_type>::sign(v.x),
            //     OP<_type>::sign(v.y));
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief The floor of each component.
        ///
        static ITK_INLINE type2 floor(const type2 &v) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            return _sse2_mm_floor_ps(v.array_sse);
#else
            return _mm_floor_ps(v.array_sse);
#endif
#elif defined(ITK_NEON)
#if defined(__aarch64__)
            return vrndm_f32(v.array_neon);
#else
            return type2(
                OP<_type>::floor(v.x),
                OP<_type>::floor(v.y));
#endif
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief The ceil of each component.
        ///
        static ITK_INLINE type2 ceil(const type2 &v) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            return _sse2_mm_ceil_ps(v.array_sse);
#else
            return _mm_ceil_ps(v.array_sse);
#endif
#elif defined(ITK_NEON)
#if defined(__aarch64__)
            return vrndp_f32(v.array_neon);
#else
            return type2(
                OP<_type>::ceil(v.x),
                OP<_type>::ceil(v.y));
#endif
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Round each component.
        ///
        static ITK_INLINE type2 round(const type2 &v) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            return _sse2_mm_round_ps(v.array_sse);
#else
            return _mm_round_ps(v.array_sse, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
#endif
#elif defined(ITK_NEON)
#if defined(__aarch64__)
            return vrndn_f32(v.array_neon);
#else
            return type2(
                OP<_type>::round(v.x),
                OP<_type>::round(v.y));
#endif
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief fmod each component.
        ///
        static ITK_INLINE type2 fmod(const type2 &a, const type2 &b) noexcept
        {
#if defined(ITK_SSE2)

            // float f = (a / b);
            __m128 f = _mm_div_ps(a.array_sse, b.array_sse);

            // float r = (float)(int)f;
            __m128 r = _mm_cvtepi32_ps(_mm_cvttps_epi32(f));

            // two possible values:
            // - 8388608.f (23bits)
            // - 2147483648.f (31bits)
            // Any value greater than this, will have integral mantissa...
            // and no decimal part
            //
            // uint32_t &r_uint = *(uint32_t *)&r;
            // uint32_t mask = -(int)(8388608.f > OP<float>::abs(f));
            // r_uint = r_uint & mask;

            // if ((abs(f) > 2**31 )) r = f;
            const __m128 _sign_bit = _mm_set1_ps(-0.f);
            const __m128 _max_f = _mm_set1_ps(8388608.f);
            __m128 m = _mm_cmpgt_ps(_max_f, _mm_andnot_ps(_sign_bit, f));
            r = _mm_and_ps(m, r);

            // return a - r * b;
            __m128 result = _mm_mul_ps(r, b.array_sse);

            result = _mm_sub_ps(a.array_sse, result);
            return result;

#elif defined(ITK_NEON)

            // float f = (a / b);
            float32x2_t f = vdiv_f32(a.array_neon, b.array_neon);

            // float r = (float)(int)f;
            uint32x2_t r = vreinterpret_u32_f32(vcvt_f32_s32(vcvt_s32_f32(f)));

            // two possible values:
            // - 8388608.f (23bits)
            // - 2147483648.f (31bits)
            // Any value greater than this, will have integral mantissa...
            // and no decimal part
            //
            // uint32_t &r_uint = *(uint32_t *)&r;
            // uint32_t mask = -(int)(8388608.f > OP<float>::abs(f));
            // r_uint = r_uint & mask;

            // if ((abs(f) > 2**31 )) r = f;
            // const __m128 _sign_bit = _mm_set1_ps(-0.f);
            const float32x2_t _max_f = vdup_n_f32(8388608.f);
            uint32x2_t m = vcgt_f32(_max_f, vabs_f32(f));
            r = vand_u32(m, r);

            // return a - r * b;
            float32x2_t result = vmul_f32(vreinterpret_f32_u32(r), b.array_neon);

            result = vsub_f32(a.array_neon, result);
            return result;
            // return type2(
            //     OP<_type>::fmod(a.x, b.x),
            //     OP<_type>::fmod(a.y, b.y));
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Step function on each component. ( v >= threshould ) ? 1 : 0
        ///
        static ITK_INLINE type2 step(const type2 &threshould, const type2 &v) noexcept
        {
#if defined(ITK_SSE2)
            __m128 _cmp = _mm_cmpge_ps(v.array_sse, threshould.array_sse);
            __m128 _rc = _mm_and_ps(_cmp, _vec2_one_sse);
            return _rc;
            // type2 _sub = v - threshould;
            // type2 _sign = self_type::sign(_sub);
            // return self_type::maximum(_sign, _vec4_zero_sse);
#elif defined(ITK_NEON)
            uint32x2_t _cmp = vcge_f32(v.array_neon, threshould.array_neon);
            uint32x2_t _rc = vand_u32(_cmp, _vec2_one_u);
            return vreinterpret_f32_u32(_rc);
            // type2 _sub = v - threshould;
            // type2 _sign = self_type::sign(_sub);
            // const float32x2_t _vec2_zero = vset1_v2(0.0f);
            // return self_type::maximum(_sign, _vec2_zero);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE type2 smoothstep(const type2 &edge0, const type2 &edge1, const type2 &x) noexcept
        {
            using type_info = FloatTypeInfo<_type>;
            type2 dir = edge1 - edge0;

            _type length_dir = OP<_type>::maximum(self_type::length(dir), type_info::min);

            type2 value = x - edge0;
            value *= (_type)1 / length_dir;

            type2 t = self_type::clamp(value, type2((_type)0), type2((_type)1));
            return t * t * ((_type)3 - (_type)2 * t);
        }
    };

}