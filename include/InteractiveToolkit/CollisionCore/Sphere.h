#pragma once

#include "collision_common.h"

namespace CollisionCore
{

    /// \brief Sphere representation
    ///
    /// It stores the center and radius
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    class Sphere<T, _CHKE_>
    {
    public:
        using self_type = Sphere<T>;

        using float_type = typename MathCore::MathTypeInfo<T>::_type;
        using vec2_type = typename MathCore::MathTypeInfo<T>::_vec2;
        using vec3_type = T; // typename MathCore::MathTypeInfo<T>::_vec3;
        using vec4_type = typename MathCore::MathTypeInfo<T>::_vec4;
        using quat_type = typename MathCore::MathTypeInfo<T>::_quat;
        using mat2_type = typename MathCore::MathTypeInfo<T>::_mat2;
        using mat3_type = typename MathCore::MathTypeInfo<T>::_mat3;
        using mat4_type = typename MathCore::MathTypeInfo<T>::_mat4;

        vec3_type center;
        float_type radius;

        /// \brief Construct a Sphere with center = vec3(0,1,0) and radius = 1.0f
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Sphere sphere;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline Sphere();

        /// \brief Construct a Sphere
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
        ///
        /// Sphere sphere = Sphere( sphere_center, sphere_radius );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center the center of the sphere
        /// \param radius the radius of the sphere
        ///
        inline Sphere(const vec3_type &center, float_type radius);

        /// \brief Compute the point in the Sphere surface that is closer to another specified point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_to_input;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// vec3 closest_point = Sphere::closestPointToSphere( ptn_to_input, sphere_center, sphere_radius );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \return The closest point in the Sphere related to p
        ///
        static inline vec3_type closestPointToSphere(const vec3_type &p, const vec3_type &center, const float_type &radius);

        /// \brief Compute the point in the Sphere surface that is closer to another specified point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_to_input;
        /// Sphere sphere;
        ///
        /// vec3 closest_point = Sphere::closestPointToSphere( ptn_to_input, sphere );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param sphere The Sphere
        /// \return The closest point in the Sphere related to p
        ///
        static inline vec3_type closestPointToSphere(const vec3_type &p, const Sphere<T> &sphere);

        /// \brief Test if there is some overlaped area between two Spheres
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Sphere sphere_a;
        /// Sphere sphere_b;
        ///
        /// if ( Sphere::sphereOverlapsSphere( sphere_a, sphere_b ) ){
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first Sphere to test
        /// \param b The second Sphere to test against
        /// \return true if there are some overlaped area between the two Spheres, otherwise false
        ///
        static inline bool sphereOverlapsSphere(const Sphere<T> &a, const Sphere<T> &b);

        /// \brief Create an Sphere that is the union result between the two Spheres
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Sphere sphere_a;
        /// Sphere sphere_b;
        ///
        /// Sphere sphere = Sphere::joinSpheres( sphere_a, sphere_b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The first Sphere to consider in the union
        /// \param b The second Sphere to consider in the union
        /// \return Sphere of the union of the parameters
        ///
        static inline Sphere<T> joinSpheres(const Sphere<T> &a, const Sphere<T> &b);

        /// \brief Raycast test against a Sphere
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against a Sphere
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
        /// //sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Sphere::raycastSphere(ray, sphere_center, sphere_radius, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r The Ray
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the sphere
        ///
        static inline bool raycastSphere(const Ray<T> &r, const vec3_type &center, const float_type &radius, float_type *outTmin, vec3_type *outNormal);

        /// \brief Raycast test against a Sphere
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against a Sphere
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
        /// Sphere sphere;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Sphere::raycastSphere(ray, sphere, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r The Ray
        /// \param sphere The Sphere
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the sphere
        ///
        static inline bool raycastSphere(const Ray<T> &r, const Sphere<T> &sphere, float_type *outTmin, vec3_type *outNormal);

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
        /// if ( Sphere::segmentIntersectsSphere( a, b, sphere_center, sphere_radius ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a line segment point a
        /// \param b line segment point b
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \return true, if the sphere intersects the line segment
        ///
        static inline bool segmentIntersectsSphere(const vec3_type &a, const vec3_type &b, const vec3_type &center, const float_type &radius);

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
        /// if ( Sphere::segmentIntersectsSphere( lineSegment, sphere_center, sphere_radius ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls line segment
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \return true, if the sphere intersects the line segment
        ///
        static inline bool segmentIntersectsSphere(const LineSegment<T> &ls, const vec3_type &center, const float_type &radius);

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
        /// if ( Sphere::segmentIntersectsSphere( a, b, sphere ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a line segment point a
        /// \param b line segment point b
        /// \param sphere The Sphere
        /// \return true, if the sphere intersects the line segment
        ///
        static inline bool segmentIntersectsSphere(const vec3_type &a, const vec3_type &b, const Sphere<T> &sphere);

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
        /// if ( Sphere::segmentIntersectsSphere( lineSegment, sphere ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls line segment
        /// \param sphere The Sphere
        /// \return true, if the sphere intersects the line segment
        ///
        static inline bool segmentIntersectsSphere(const LineSegment<T> &ls, const Sphere<T> &sphere);

        /// \brief Test if a point is inside a Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 point;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// if ( Sphere::pointInsideSphere( point, sphere_center, sphere_radius ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p line point to test
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \return true, if the point is inside the sphere
        ///
        static inline bool pointInsideSphere(const vec3_type &p, const vec3_type &center, const float_type &radius);

        /// \brief Test if a point is inside a Sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 point;
        /// Sphere sphere;
        ///
        /// if ( Sphere::pointInsideSphere( point, sphere ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p line point to test
        /// \param sphere The Sphere
        /// \return true, if the point is inside the sphere
        ///
        static inline bool pointInsideSphere(const vec3_type &p, const Sphere<T> &sphere);

        //
        // Cloned methods from other collision classes
        //

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
        /// if ( Sphere::aabbOverlapsSphere( aabb, sphere_center, sphere_radius, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the aabb overlaps the sphere
        ///
        static inline bool aabbOverlapsSphere(const AABB<T> &aabb, const vec3_type &center, const float_type &radius, vec3_type *penetration);

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
        /// if ( Sphere::aabbOverlapsSphere( aabb, sphere, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param sphere The Sphere
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the aabb overlaps the sphere
        ///
        static inline bool aabbOverlapsSphere(const AABB<T> &aabb, const Sphere<T> &sphere, vec3_type *penetration);

        /// \brief Test if a frustum overlaps the sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Frustum frustum;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// if ( Sphere::frustumOverlapsSphere( frustum, sphere_center, sphere_radius ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param f The frustum
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \return true, if the sphere overlaps the frustum
        ///
        static inline bool frustumOverlapsSphere(const Frustum<T> &f, const vec3_type &center, const float_type &radius);

        /// \brief Test if a frustum overlaps the sphere
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Frustum frustum;
        /// Sphere sphere;
        ///
        /// if ( Sphere::frustumOverlapsSphere( frustum, sphere ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param f The frustum
        /// \param s The Sphere
        /// \return true, if the sphere overlaps the frustum
        ///
        static inline bool frustumOverlapsSphere(const Frustum<T> &f, const Sphere<T> &s);

        /// \brief Test if a sphere intersects the triangle
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
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// vec3 penetration;
        /// if ( Sphere::triangleIntersectsSphere( a, b, c, sphere_center, sphere_radius, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The triangle vertex a
        /// \param b The triangle vertex b
        /// \param c The triangle vertex c
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param[out] penetration returns the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static inline bool triangleIntersectsSphere(const vec3_type &a, const vec3_type &b, const vec3_type &c, const vec3_type &center, const float_type &radius, vec3_type *penetration);

        /// \brief Test if a sphere intersects the triangle
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
        /// Sphere sphere;
        ///
        /// vec3 penetration;
        /// if ( Sphere::triangleIntersectsSphere( a, b, c, sphere, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The triangle vertex a
        /// \param b The triangle vertex b
        /// \param c The triangle vertex c
        /// \param sphere The Sphere
        /// \param[out] penetration returns the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static inline bool triangleIntersectsSphere(const vec3_type &a, const vec3_type &b, const vec3_type &c, const Sphere<T> &sphere, vec3_type *penetration);

        /// \brief Test if a sphere intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// vec3 penetration;
        /// if ( Sphere::triangleIntersectsSphere( triangle, sphere_center, sphere_radius, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param triangle The Triangle
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param[out] penetration returns the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static inline bool triangleIntersectsSphere(const Triangle<T> &triangle, const vec3_type &center, const float_type &radius, vec3_type *penetration);

        /// \brief Test if a sphere intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Triangle triangle;
        /// Sphere sphere;
        ///
        /// vec3 penetration;
        /// if ( Sphere::triangleIntersectsSphere( triangle, sphere, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param triangle The Triangle
        /// \param sphere The Sphere
        /// \param[out] penetration returns the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static inline bool triangleIntersectsSphere(const Triangle<T> &triangle, const Sphere<T> &sphere, vec3_type *penetration);

        /// \brief Test if a sphere overlaps the OBB
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
        /// OBB obb;
        ///
        /// vec3 penetration;
        /// if ( Sphere::obbOverlapsSphere( obb, sphere_center, sphere_radius, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param obb The OBB
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the obb overlaps the sphere
        ///
        static inline bool obbOverlapsSphere(const OBB<T> &obb, const vec3_type &center, const float_type &radius, vec3_type *penetration);

        /// \brief Test if a sphere overlaps the OBB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Sphere sphere;
        /// OBB obb;
        ///
        /// vec3 penetration;
        /// if ( Sphere::obbOverlapsSphere( obb, sphere, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param obb The OBB
        /// \param sphere The Sphere
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the obb overlaps the sphere
        ///
        static inline bool obbOverlapsSphere(const OBB<T> &obb, const Sphere<T> &sphere, vec3_type *penetration);

        static inline bool planeIntersectsSphere(const Plane<T> &plane, const vec3_type &center, const float_type &radius, vec3_type *penetration);

        static inline bool planeIntersectsSphere(const Plane<T> &plane, const Sphere<T> &sphere, vec3_type *penetration);

        /// \brief Compute the minimum bounding sphere from four points (Tetrahedron)
        ///
        /// Details are in:
        ///
        /// https://gamedev.stackexchange.com/questions/162731/welzl-algorithm-to-find-the-smallest-bounding-sphere
        ///
        /// https://mathworld.wolfram.com/Circumsphere.html
        ///
        /// \author Alessandro Ribeiro
        /// \param a Tetrahedron point a
        /// \param b Tetrahedron point b
        /// \param c Tetrahedron point c
        /// \param d Tetrahedron point d
        /// \return the minimum sphere
        ///
        static inline Sphere<T> from4Points(const vec3_type &a, const vec3_type &b, const vec3_type &c, const vec3_type &d);

        /// \brief Compute the bounding sphere from a frustum
        ///
        /// **Notice:** This method might not return the minimum sphere.
        ///
        /// \author Alessandro Ribeiro
        /// \param frustum The frustum
        /// \return the bouding sphere
        ///
        static inline Sphere<T> fromFrustum(const Frustum<T> &frustum);

        /// \brief Compute the bounding sphere from an AABB
        ///
        /// **Notice:** This method might not return the minimum sphere.
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param discard_z If true, will compute just the x and y coords
        /// \return the bouding sphere
        ///
        static inline Sphere<T> fromAABB(const AABB<T> &aabb, bool discard_z = false);

        /// \brief Compute the bounding sphere from a line segment
        ///
        /// **Notice:** This method might not return the minimum sphere.
        ///
        /// \author Alessandro Ribeiro
        /// \param ls The line segment
        /// \return the bouding sphere
        ///
        static inline Sphere<T> fromLineSegment(const LineSegment<T> &ls);

        /// \brief Compute the bounding sphere from a triangle (Not tested...)
        ///
        /// **Notice:** This method might not return the minimum sphere.
        ///
        /// \author Alessandro Ribeiro
        /// \param triangle The Triangle
        /// \return the bouding sphere
        ///
        static inline Sphere<T> fromTriangle(const Triangle<T> &triangle);

        /// \brief Compute the bounding sphere from an OBB
        ///
        /// **Notice:** This method might not return the minimum sphere.
        ///
        /// \author Alessandro Ribeiro
        /// \param obb The OBB
        /// \param discard_z If true, will compute just the x and y coords
        /// \return the bouding sphere
        ///
        static inline Sphere<T> fromOBB(const OBB<T> &obb, bool discard_z = false);
    };

}