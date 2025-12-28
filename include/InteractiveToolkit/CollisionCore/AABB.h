#pragma once

#include "collision_common.h"

namespace CollisionCore
{

    /// \brief Axis Aligned Bounding Box (AABB)
    ///
    /// Stores 3D points to represent an Axis Aligned Bounding Box. <br/>
    /// It can be used to make collision tests
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    class AABB<T, _CHKE_>
    {
    public:
        using self_type = AABB<T>;

        using float_type = typename MathCore::MathTypeInfo<T>::_type;
        using vec2_type = typename MathCore::MathTypeInfo<T>::_vec2;
        using vec3_type = T; // typename MathCore::MathTypeInfo<T>::_vec3;
        using vec4_type = typename MathCore::MathTypeInfo<T>::_vec4;
        using quat_type = typename MathCore::MathTypeInfo<T>::_quat;
        using mat2_type = typename MathCore::MathTypeInfo<T>::_mat2;
        using mat3_type = typename MathCore::MathTypeInfo<T>::_mat3;
        using mat4_type = typename MathCore::MathTypeInfo<T>::_mat4;

        vec3_type min_box; ///< Store the minimum values of the AABB Box
        vec3_type max_box; ///< Store the maximum values of the AABB Box
        //--------------------------------------------------------------------------
        /// \brief Construct a ZERO AABB class
        ///
        /// The ZERO AABB class have both points in the origin (0,0,0)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// AABB aabb = AABB();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline AABB();

        inline void makeEmpty();
        inline bool isEmpty() const;
        //--------------------------------------------------------------------------
        /// \brief Constructs a new bidimensional AABB class
        ///
        /// Requires to pass 2 points to construct of the AABB class
        ///
        /// Ex:
        /// <pre>
        ///  CornerA *--* CornerB
        ///          |  |
        ///  CornerC *--* CornerD
        /// </pre>
        ///
        /// You can use any combination: {CornerA,CornerD}, {CornerC,CornerB}, etc...
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec2 corner_a, corner_d;
        ///
        /// AABB aabb = AABB(corner_d, corner_a);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a An AABB corner
        /// \param b Another AABB corner
        /// \warning The z value of both min_box and max_box are -1 and 1 respectively
        ///
        inline AABB(const vec2_type &a, const vec2_type &b);
        //--------------------------------------------------------------------------
        /// \brief Constructs a new bidimensional AABB class
        ///
        /// Requires to pass 2 points to construct of the AABB class
        ///
        /// Ex:
        /// <pre>
        ///  CornerA *--* CornerB
        ///          |  |
        ///  CornerC *--* CornerD
        /// </pre>
        ///
        /// You can use any combination: {CornerA,CornerD}, {CornerC,CornerB}, etc...
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 corner_c, corner_b;
        ///
        /// AABB aabb = AABB(corner_b, corner_c);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a An AABB corner
        /// \param b Another AABB corner
        ///
        inline AABB(const vec3_type &a, const vec3_type &b);
        //--------------------------------------------------------------------------
        // static  inline methods
        //--------------------------------------------------------------------------

        /// \brief Test if a point is inside an AABB
        /// \author Alessandro Ribeiro
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// AABB aabb;
        /// vec2 point;
        ///
        /// if ( AABB::pointInsideAABB( point, aabb ) ){
        ///     ...
        /// }
        /// \endcode
        ///
        /// \param ptn 2D point to test against the AABB
        /// \param b The AABB
        /// \return true if the point is inside the AABB, otherwise false
        /// \warning The z is not used in the test
        ///
        static inline bool pointInsideAABB(const vec2_type &ptn, const AABB<T> &b);
        /// \brief Test if a point is inside an AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// AABB aabb;
        /// vec3 point;
        ///
        /// if ( AABB::pointInsideAABB( point, aabb ) ){
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ptn 3D point to test against the AABB
        /// \param b The AABB
        /// \return true if the point is inside the AABB, otherwise false
        ///
        static inline bool pointInsideAABB(const vec3_type &ptn, const AABB<T> &b);

        /// \brief Test if there is some overlaped area between two AABBs
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// AABB aabb_a;
        /// AABB aabb_b;
        ///
        /// if ( AABB::aabbOverlapsAABB( aabb_a, aabb_b ) ){
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first AABB to test
        /// \param b The second AABB to test against
        /// \return true if there are some overlaped area between the two AABBs, otherwise false
        ///
        static inline bool aabbOverlapsAABB(const AABB<T> &a, const AABB<T> &b);

        /// \brief Create an AABB that is the union result between the two AABBs
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// AABB aabb_a;
        /// AABB aabb_b;
        ///
        /// AABB aabb = AABB::joinAABB( aabb_a, aabb_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first AABB to consider in the union
        /// \param b The second AABB to consider in the union
        /// \return AABB of the union of the parameters
        ///
        static inline AABB<T> joinAABB(const AABB<T> &a, const AABB<T> &b);

        /// \brief Create an AABB that that contains the triangle vertex a, b and c
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // triangle vertex
        /// vec3 a, b, c;
        ///
        /// AABB aabb = AABB::fromTriangle( a, b, c );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Triangle vertex a
        /// \param b Triangle vertex b
        /// \param c Triangle vertex c
        /// \return AABB containing the triangle
        ///
        static inline AABB<T> fromTriangle(const vec3_type &a, const vec3_type &b, const vec3_type &c);

        /// \brief Create an AABB that that contains the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        ///
        /// AABB aabb = AABB::fromTriangle( triangle );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param triangle Triangle
        /// \return AABB containing the triangle
        ///
        static inline AABB<T> fromTriangle(const Triangle<T> &triangle);

        /// \brief Create an AABB that that contains the sphere with pos as position and radius
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // sphere parameters
        /// vec3 sphere_pos;
        /// float sphere_radius;
        ///
        /// AABB aabb = AABB::fromSphere( sphere_pos, sphere_radius );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param pos sphere center position
        /// \param radius sphere radius
        /// \return AABB containing the sphere
        ///
        static inline AABB<T> fromSphere(const vec3_type &pos, const float_type &radius);

        /// \brief Create an AABB that that contains the sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Sphere sphere;
        ///
        /// AABB aabb = AABB::fromSphere( sphere );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param sphere Sphere
        /// \return AABB containing the sphere
        ///
        static inline AABB<T> fromSphere(const Sphere<T> &sphere);

        /// \brief Create an AABB that that contains the line segment
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // segment points
        /// vec3 segment_a, segment_b;
        ///
        /// AABB aabb = AABB::fromLineSegment( segment_a, segment_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a Point 'a' from the line segment
        /// \param b Point 'b' from the line segment
        /// \return AABB containing the line segment
        ///
        static inline AABB<T> fromLineSegment(const vec3_type &a, const vec3_type &b);

        /// \brief Create an AABB that that contains the line segment
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment lineSegment;
        ///
        /// AABB aabb = AABB::fromLineSegment( lineSegment );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls the line segment
        /// \return AABB containing the line segment
        ///
        static inline AABB<T> fromLineSegment(const LineSegment<T> &ls);

        /// \brief Create an AABB that that contains the frustum
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Frustum frustum;
        ///
        /// AABB aabb = AABB::fromFrustum( frustum );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param frustum the frustum
        /// \return AABB containing the line segment
        ///
        static inline AABB<T> fromFrustum(const Frustum<T> &frustum);

        /// \brief Create an AABB that that contains the OBB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// OBB obb;
        ///
        /// AABB aabb = AABB::fromOBB( obb );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param obb the obb
        /// \return AABB containing the OBB
        ///
        static inline AABB<T> fromOBB(const OBB<T> &obb);

        /// \brief Raycast test against an AABB
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against AABB a.
        ///
        /// When intersecting it returns the intersection distance tmin and normal of intersection.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Ray ray;
        /// AABB aabb;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( AABB::raycastAABB(ray, aabb, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r The Ray
        /// \param a The AABB
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the aabb
        ///
        static inline bool raycastAABB(const Ray<T> &r, const AABB<T> &a, float_type *outTmin, vec3_type *outNormal);

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
        /// if ( AABB::segmentIntersectsAABB( a, b, aabb ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p0 line segment point a
        /// \param p1 line segment point b
        /// \param aabb The AABB
        /// \return true, if the aabb overlaps the line segment
        ///
        static inline bool segmentIntersectsAABB(const vec3_type &p0, const vec3_type &p1, const AABB<T> &aabb);

        /// \brief Test if a line segment intersects the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// LineSegment ls;
        /// AABB aabb;
        ///
        /// if ( AABB::segmentIntersectsAABB( ls, aabb ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls line segment
        /// \param aabb The AABB
        /// \return true, if the aabb overlaps the line segment
        ///
        static inline bool segmentIntersectsAABB(const LineSegment<T> &ls, const AABB<T> &aabb);

        /// \brief Compute the point in the AABB surface that is closer to another specified point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_to_input;
        /// AABB aabb;
        ///
        /// vec3 closest_point = AABB::closestPointToAABB( ptn_to_input, aabb );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param aabb The AABB
        /// \return The closest point in the AABB related to p
        ///
        static inline vec3_type closestPointToAABB(const vec3_type &p, const AABB<T> &aabb);

        /// \brief Test if a sphere overlaps the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 sphere_center;
        /// float sphere_radius;
        /// AABB aabb;
        ///
        /// vec3 penetration;
        /// if ( AABB::sphereOverlapsAABB( sphere_center, sphere_radius, aabb, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param aabb The AABB
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the aabb overlaps the sphere
        ///
        static inline bool sphereOverlapsAABB(const vec3_type &center, const float_type &radius, const AABB<T> &aabb, vec3_type *penetration);

        /// \brief Test if a sphere overlaps the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Sphere sphere;
        /// AABB aabb;
        ///
        /// vec3 penetration;
        /// if ( AABB::sphereOverlapsAABB( sphere, aabb, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param sphere The Sphere
        /// \param aabb The AABB
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the aabb overlaps the sphere
        ///
        static inline bool sphereOverlapsAABB(const Sphere<T> &sphere, const AABB<T> &aabb, vec3_type *penetration);

        /// \brief Test if a plane intersects the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Plane plane;
        /// AABB aabb;
        ///
        /// if ( AABB::planeIntersectsAABB( plane, aabb ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param plane The Plane
        /// \param aabb The AABB
        /// \return true, if the aabb intersects the plane
        ///
        static inline bool planeIntersectsAABB(const Plane<T> &plane, const AABB<T> &aabb);

        /// \brief Test if a triangle intersects the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // triangle vertex a, b, c
        /// vec3 a, b, c;
        /// AABB aabb;
        ///
        /// if ( AABB::triangleIntersectsAABB( a, b, c, aabb ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v0 The triangle vertex a
        /// \param v1 The triangle vertex b
        /// \param v2 The triangle vertex c
        /// \param aabb The AABB
        /// \return true, if the aabb intersects the triangle
        ///
        static inline bool triangleIntersectsAABB(const vec3_type &v0, const vec3_type &v1, const vec3_type &v2, const AABB<T> &aabb);

        /// \brief Test if a triangle overlaps the AABB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        /// AABB aabb;
        ///
        /// if ( AABB::triangleIntersectsAABB( triangle, aabb ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param t The triangle
        /// \param aabb The AABB
        /// \return true, if the aabb overlaps the triangle
        ///
        static inline bool triangleIntersectsAABB(const Triangle<T> &t, const AABB<T> &aabb);

        //
        // Cloned methods from other collision classes
        //

        /// \brief Test if a frustum overlaps the aabb
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Frustum frustum;
        /// AABB aabb;
        ///
        /// if ( AABB::frustumOverlapsAABB( frustum, aabb ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param f The frustum
        /// \param aabb The AABB
        /// \return true, if the aabb overlaps the frustum
        ///
        static inline bool frustumOverlapsAABB(const Frustum<T> &f, const AABB<T> &aabb);

        /// \brief Test if an OBB overlaps the aabb
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// OBB obb;
        /// AABB aabb;
        ///
        /// if ( AABB::obbOverlapsAAB( obb, aabb ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param obb The OBB
        /// \param aabb The AABB
        /// \return true, if the aabb overlaps the frustum
        ///
        static inline bool obbOverlapsAABB(const OBB<T> &obb, const AABB<T> &aabb);

        // comparison operators
        inline bool operator==(const self_type &other) const;
        inline bool operator!=(const self_type &other) const;
    };

}
