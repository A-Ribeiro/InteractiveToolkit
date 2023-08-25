#pragma once

#include "collision_common.h"

namespace CollisionCore
{

    /// \brief Line Segment representation
    ///
    /// Can be used to compute intersection with other shapes (Triangle, AABB, Sphere)
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    class LineSegment<T, _CHKE_>
    {
    public:
        using self_type = LineSegment<T>;

        using float_type = typename MathCore::MathTypeInfo<T>::_type;
        using vec2_type = typename MathCore::MathTypeInfo<T>::_vec2;
        using vec3_type = T; // typename MathCore::MathTypeInfo<T>::_vec3;
        using vec4_type = typename MathCore::MathTypeInfo<T>::_vec4;
        using quat_type = typename MathCore::MathTypeInfo<T>::_quat;
        using mat2_type = typename MathCore::MathTypeInfo<T>::_mat2;
        using mat3_type = typename MathCore::MathTypeInfo<T>::_mat3;
        using mat4_type = typename MathCore::MathTypeInfo<T>::_mat4;

        vec3_type a, b;

        /// \brief Construct a line segment with point a and b in the coordinate origin. vec3(0)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline LineSegment();

        /// \brief Construct a line segment with point a and b
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_a, ptn_b;
        ///
        /// LineSegment lineSegment = LineSegment( ptn_a, ptn_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The point a from line segment
        /// \param b The point b from line segment
        ///
        inline LineSegment(const vec3_type &a, const vec3_type &b);

        /// \brief Compute the point in the line that is closer to another specified point
        ///
        /// Given segment ab and point c, computes closest point d on ab.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_a, ptn_b;
        /// vec3 ptn_input;
        ///
        /// vec3 closest_point = LineSegment::closestPointToSegment( ptn_to_input, ptn_a, ptn_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param a The point a from line segment
        /// \param b The point b from line segment
        /// \return The closest point in the line related to p
        ///
        static inline vec3_type closestPointToSegment(const vec3_type &p, const vec3_type &a, const vec3_type &b);

        /// \brief Compute the point in the line that is closer to another specified point
        ///
        /// Given segment ab and point c, computes closest point d on ab.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// vec3 ptn_input;
        ///
        /// vec3 closest_point = LineSegment::closestPointToSegment( ptn_to_input, lineSegment );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param ls The line segment
        /// \return The closest point in the line related to p
        ///
        static inline vec3_type closestPointToSegment(const vec3_type &p, const LineSegment<T> &ls);

        //
        // Cloned methods from other collision classes
        //
        /// \brief Test if a line segment intersects the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// //line segment a and b
        /// vec3 a, b;
        /// AABB aabb;
        ///
        /// if ( LineSegment::aabbIntersectsSegment( aabb, a, b ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param p0 line segment point a
        /// \param p1 line segment point b
        /// \return true, if the aabb overlaps the line segment
        ///
        static inline bool aabbIntersectsSegment(const AABB<T> &aabb, const vec3_type &p0, const vec3_type &p1);

        /// \brief Test if a line segment intersects the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// AABB aabb;
        ///
        /// if ( LineSegment::aabbIntersectsSegment( aabb, lineSegment ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param ls line segment
        /// \return true, if the aabb overlaps the line segment
        ///
        static inline bool aabbIntersectsSegment(const AABB<T> &aabb, const LineSegment<T> &ls);

        /// \brief Test if a line segment intersects the Plane
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// //line segment a and b
        /// vec3 a, b;
        /// Plane plane;
        ///
        /// if ( LineSegment::planeIntersectsSegment( plane, a, b ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param plane The Plane
        /// \param a line segment point a
        /// \param b line segment point b
        /// \return true, if the plane intersects the line segment
        ///
        static inline bool planeIntersectsSegment(const Plane<T> &plane, const vec3_type &a, const vec3_type &b);

        /// \brief Test if a line segment intersects the Plane
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// Plane plane;
        ///
        /// if ( LineSegment::planeIntersectsSegment( plane, lineSegment ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param plane The Plane
        /// \param ls line segment
        /// \return true, if the plane intersects the line segment
        ///
        static inline bool planeIntersectsSegment(const Plane<T> &plane, const LineSegment<T> &ls);

        /// \brief Test if a line segment intersects the Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// //line segment a and b
        /// vec3 a, b;
        /// //sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// if ( LineSegment::sphereIntersectsSegment( sphere_center, sphere_radius, a, b ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center The Sphere center
        /// \param radius The Sphere radius
        /// \param a line segment point a
        /// \param b line segment point b
        /// \return true, if the sphere intersects the line segment
        ///
        static inline bool sphereIntersectsSegment(const vec3_type &center, const float_type &radius, const vec3_type &a, const vec3_type &b);

        /// \brief Test if a line segment intersects the Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// //sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// if ( LineSegment::sphereIntersectsSegment( sphere_center, sphere_radius, lineSegment ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center The Sphere center
        /// \param radius The Sphere radius
        /// \param ls line segment
        /// \return true, if the sphere intersects the line segment
        ///
        static inline bool sphereIntersectsSegment(const vec3_type &center, const float_type &radius, const LineSegment<T> &ls);

        /// \brief Test if a line segment intersects the Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// //line segment a and b
        /// vec3 a, b;
        /// Sphere sphere;
        ///
        /// if ( LineSegment::sphereIntersectsSegment( sphere, a, b ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param sphere The Sphere
        /// \param a line segment point a
        /// \param b line segment point b
        /// \return true, if the sphere intersects the line segment
        ///
        static inline bool sphereIntersectsSegment(const Sphere<T> &sphere, const vec3_type &a, const vec3_type &b);

        /// \brief Test if a line segment intersects the Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// Sphere sphere;
        ///
        /// if ( LineSegment::sphereIntersectsSegment( sphere, lineSegment ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param sphere The Sphere
        /// \param ls line segment
        /// \return true, if the sphere intersects the line segment
        ///
        static inline bool sphereIntersectsSegment(const Sphere<T> &sphere, const LineSegment<T> &ls);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // triangle
        /// vec3 a, b, c;
        /// // line segment
        /// vec3 p, q;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( LineSegment::triangleIntersectsSegment( a, b, c, p, q, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Triangle vertex a
        /// \param b Triangle vertex b
        /// \param c Triangle vertex c
        /// \param p line segment point a
        /// \param q line segment point b
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static inline bool triangleIntersectsSegment(
            const vec3_type &a, const vec3_type &b, const vec3_type &c,
            const vec3_type &p, const vec3_type &q,
            float_type *u, float_type *v, float_type *w);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // triangle
        /// vec3 a, b, c;
        /// LineSegment lineSegment;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( LineSegment::triangleIntersectsSegment( a, b, c, lineSegment, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Triangle vertex a
        /// \param b Triangle vertex b
        /// \param c Triangle vertex c
        /// \param ls line segment
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static inline bool triangleIntersectsSegment(
            const vec3_type &a, const vec3_type &b, const vec3_type &c,
            const LineSegment<T> &ls,
            float_type *u, float_type *v, float_type *w);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        /// LineSegment lineSegment;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( LineSegment::triangleIntersectsSegment( triangle, lineSegment, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param t triangle
        /// \param ls line segment
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static inline bool triangleIntersectsSegment(
            const Triangle<T> &t,
            const LineSegment<T> &ls,
            float_type *u, float_type *v, float_type *w);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        /// // line segment
        /// vec3 p, q;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( LineSegment::triangleIntersectsSegment( triangle, p, q, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param t triangle
        /// \param p line segment point a
        /// \param q line segment point b
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static inline bool triangleIntersectsSegment(
            const Triangle<T> &t,
            const vec3_type &p, const vec3_type &q,
            float_type *u, float_type *v, float_type *w);

        /// \brief Test if a line segment intersects the OBB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// //line segment a and b
        /// vec3 a, b;
        /// OBB obb;
        ///
        /// if ( LineSegment::obbIntersectsSegment( obb, a, b ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param obb The OBB
        /// \param p0 line segment point a
        /// \param p1 line segment point b
        /// \return true, if the obb overlaps the line segment
        ///
        static inline bool obbIntersectsSegment(const OBB<T> &obb, const vec3_type &p0, const vec3_type &p1);

        /// \brief Test if a line segment intersects the OBB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        /// OBB obb;
        ///
        /// if ( LineSegment::obbIntersectsSegment( obb, lineSegment ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param obb The OBB
        /// \param ls line segment
        /// \return true, if the obb overlaps the line segment
        ///
        static inline bool obbIntersectsSegment(const OBB<T> &obb, const LineSegment<T> &ls);
    };

}
