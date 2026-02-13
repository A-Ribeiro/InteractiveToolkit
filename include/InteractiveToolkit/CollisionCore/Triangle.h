#pragma once

#include "collision_common.h"

namespace CollisionCore
{

    /// \brief Triangle representation
    ///
    /// THe methods of this class can be used with the vertex parameters directly.
    ///
    /// It is possible to use it with a vertex list.
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    class Triangle<T, _CHKE_>
    {

    public:
        using self_type = Triangle<T>;

        using float_type = typename MathCore::MathTypeInfo<T>::_type;
        using vec2_type = typename MathCore::MathTypeInfo<T>::_vec2;
        using vec3_type = T; // typename MathCore::MathTypeInfo<T>::_vec3;
        using vec4_type = typename MathCore::MathTypeInfo<T>::_vec4;
        using quat_type = typename MathCore::MathTypeInfo<T>::_quat;
        using mat2_type = typename MathCore::MathTypeInfo<T>::_mat2;
        using mat3_type = typename MathCore::MathTypeInfo<T>::_mat3;
        using mat4_type = typename MathCore::MathTypeInfo<T>::_mat4;

        vec3_type a, b, c;

        /// \brief Construct a Triangle with a = b = c = vec3(0)
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Triangle triangle;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline Triangle();

        /// \brief Construct a Triangle
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 a, b, c;
        ///
        /// Triangle triangle( a, b, c );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a The triangle a
        /// \param b The triangle b
        /// \param c The triangle c
        ///
        inline Triangle(const vec3_type &a, const vec3_type &b, const vec3_type &c);

        /// \brief Raycast test against a Triangle
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against a Triangle
        ///
        /// When intersecting it returns the intersection distance tmin and normal of intersection.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Ray ray;
        /// //triangle
        /// vec3 a, b, c;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Triangle::raycastTriangle(ray, a, b, c, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ray The Ray
        /// \param a The triangle a
        /// \param b The triangle b
        /// \param c The triangle c
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the sphere
        ///
        static inline bool raycastTriangle(const Ray<T> &ray, const vec3_type &a, const vec3_type &b, const vec3_type &c, float_type *outTmin, vec3_type *outNormal);

        /// \brief Raycast test against a Triangle
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against a Triangle
        ///
        /// When intersecting it returns the intersection distance tmin and normal of intersection.
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Ray ray;
        /// Triangle triangle;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Triangle::raycastTriangle(ray, triangle, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ray The Ray
        /// \param t The Triangle
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the sphere
        ///
        static inline bool raycastTriangle(const Ray<T> &ray, const Triangle<T> &t, float_type *outTmin, vec3_type *outNormal);

        // returns the closest point inside the triangle relative to the parameter p

        /// \brief Compute the point in the Triangle surface that is closer to another specified point
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 ptn_to_input;
        /// // triangle
        /// vec3 a, b, c;
        ///
        /// vec3 closest_point = Triangle::closestPointToTriangle( ptn_to_input, a, b, c );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param a The triangle vertex a
        /// \param b The triangle vertex b
        /// \param c The triangle vertex c
        /// \return The closest point in the Triangle related to p
        ///
        static inline vec3_type closestPointToTriangle(const vec3_type &p, const vec3_type &a, const vec3_type &b, const vec3_type &c);

        /// \brief Compute the point in the Triangle surface that is closer to another specified point
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 ptn_to_input;
        /// Triangle triangle;
        ///
        /// vec3 closest_point = Triangle::closestPointToTriangle( ptn_to_input, triangle );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param t The Triangle
        /// \return The closest point in the Triangle related to p
        ///
        static inline vec3_type closestPointToTriangle(const vec3_type &p, const Triangle<T> &t);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Given line pq and ccw triangle abc, return whether line pierces triangle. <br />
        /// If so, also return the barycentric coordinates (u,v,w) of the intersection point
        ///
        /// Example:
        ///
        /// \code
        ///
        /// // line segment
        /// vec3 p, q;
        /// // triangle
        /// vec3 a, b, c;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( Triangle::segmentIntersectsTriangle( p, q, a, b, c, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p line segment point a
        /// \param q line segment point b
        /// \param a triangle vertex a
        /// \param b triangle vertex b
        /// \param c triangle vertex c
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static inline bool segmentIntersectsTriangle(const vec3_type &p, const vec3_type &q,
                                                     const vec3_type &a, const vec3_type &b, const vec3_type &c,
                                                     float_type *u, float_type *v, float_type *w);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Given line pq and ccw triangle abc, return whether line pierces triangle. <br />
        /// If so, also return the barycentric coordinates (u,v,w) of the intersection point
        ///
        /// Example:
        ///
        /// \code
        ///
        /// LineSegment lineSegment;
        /// // triangle
        /// vec3 a, b, c;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( Triangle::segmentIntersectsTriangle( lineSegment, a, b, c, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls line segment
        /// \param a triangle vertex a
        /// \param b triangle vertex b
        /// \param c triangle vertex c
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static inline bool segmentIntersectsTriangle(const LineSegment<T> &ls,
                                                     const vec3_type &a, const vec3_type &b, const vec3_type &c,
                                                     float_type *u, float_type *v, float_type *w);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Given line pq and ccw triangle abc, return whether line pierces triangle. <br />
        /// If so, also return the barycentric coordinates (u,v,w) of the intersection point
        ///
        /// Example:
        ///
        /// \code
        ///
        /// // line segment
        /// vec3 p, q;
        /// Triangle triangle;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( Triangle::segmentIntersectsTriangle( p, q, triangle, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p line segment point a
        /// \param q line segment point b
        /// \param t Triangle
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static inline bool segmentIntersectsTriangle(const vec3_type &p, const vec3_type &q,
                                                     const Triangle<T> &t,
                                                     float_type *u, float_type *v, float_type *w);

        /// \brief Test if a line segment intersects the triangle
        ///
        /// Given line pq and ccw triangle abc, return whether line pierces triangle. <br />
        /// If so, also return the barycentric coordinates (u,v,w) of the intersection point
        ///
        /// Example:
        ///
        /// \code
        ///
        /// LineSegment lineSegment;
        /// Triangle triangle;
        ///
        /// // triangle baricentric coords u,v,w
        /// float u, v, w;
        /// if ( Triangle::segmentIntersectsTriangle( lineSegment, triangle, &u, &v, &w ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls line segment
        /// \param t Triangle
        /// \param u **return** u barycentric coordinate
        /// \param v **return** v barycentric coordinate
        /// \param w **return** w barycentric coordinate
        /// \return true, if the triangle intersects the line segment
        ///
        static inline bool segmentIntersectsTriangle(const LineSegment<T> &ls,
                                                     const Triangle<T> &t,
                                                     float_type *u, float_type *v, float_type *w);

        /// \brief Test if a sphere intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        ///
        /// // triangle
        /// vec3 a, b, c;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// vec3 penetration;
        /// if ( Triangle::sphereIntersectsTriangle( sphere_center, sphere_radius, a, b, c, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param a The triangle vertex a
        /// \param b The triangle vertex b
        /// \param c The triangle vertex c
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static inline bool sphereIntersectsTriangle(const vec3_type &center, const float_type &radius, const vec3_type &a, const vec3_type &b, const vec3_type &c, vec3_type *penetration);

        /// \brief Test if a sphere intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        ///
        /// // triangle
        /// vec3 a, b, c;
        /// Sphere sphere;
        ///
        /// vec3 penetration;
        /// if ( Triangle::sphereIntersectsTriangle( sphere, a, b, c, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param sphere The Sphere
        /// \param a The triangle vertex a
        /// \param b The triangle vertex b
        /// \param c The triangle vertex c
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static inline bool sphereIntersectsTriangle(const Sphere<T> &sphere, const vec3_type &a, const vec3_type &b, const vec3_type &c, vec3_type *penetration);

        /// \brief Test if a sphere intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Triangle triangle;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// vec3 penetration;
        /// if ( Triangle::sphereIntersectsTriangle( sphere_center, sphere_radius, triangle, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param t The Triangle
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static inline bool sphereIntersectsTriangle(const vec3_type &center, const float_type &radius, const Triangle<T> &t, vec3_type *penetration);

        /// \brief Test if a sphere intersects the triangle
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Triangle triangle;
        /// Sphere sphere;
        ///
        /// vec3 penetration;
        /// if ( Triangle::sphereIntersectsTriangle( sphere, triangle, &penetration ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param sphere The Sphere
        /// \param t The Triangle
        /// \param penetration **return** the amount one shape is inside the other
        /// \return true, if the triangle intersects the sphere
        ///
        static inline bool sphereIntersectsTriangle(const Sphere<T> &sphere, const Triangle<T> &t, vec3_type *penetration);

        //
        // Cloned methods from other collision classes
        //
        /// \brief Test if a triangle intersects the AABB
        ///
        /// Example:
        ///
        /// \code
        ///
        /// // triangle vertex a, b, c
        /// vec3 a, b, c;
        /// AABB aabb;
        ///
        /// if ( Triangle::aabbIntersectsTriangle( aabb, a, b, c ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param v0 The triangle vertex a
        /// \param v1 The triangle vertex b
        /// \param v2 The triangle vertex c
        /// \return true, if the aabb intersects the triangle
        ///
        static inline bool aabbIntersectsTriangle(const AABB<T> &aabb, const vec3_type &v0, const vec3_type &v1, const vec3_type &v2);

        /// \brief Test if a triangle intersects the AABB
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Triangle triangle;
        /// AABB aabb;
        ///
        /// if ( Triangle::aabbIntersectsTriangle( aabb, triangle ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param triangle The Triangle
        /// \return true, if the aabb intersects the triangle
        ///
        static inline bool aabbIntersectsTriangle(const AABB<T> &aabb, const Triangle<T> &triangle);

        /// \brief Test if a triangle intersects the OBB
        ///
        /// Example:
        ///
        /// \code
        ///
        /// // triangle vertex a, b, c
        /// vec3 a, b, c;
        /// OBB obb;
        ///
        /// if ( Triangle::obbIntersectsTriangle( obb, a, b, c ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param obb The OBB
        /// \param v0 The triangle vertex a
        /// \param v1 The triangle vertex b
        /// \param v2 The triangle vertex c
        /// \return true, if the obb intersects the triangle
        ///
        static inline bool obbIntersectsTriangle(const OBB<T> &obb, const vec3_type &v0, const vec3_type &v1, const vec3_type &v2);

        /// \brief Test if a triangle intersects the OBB
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Triangle triangle;
        /// OBB obb;
        ///
        /// if ( Triangle::obbIntersectsTriangle( obb, triangle ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param obb The OBB
        /// \param triangle The Triangle
        /// \return true, if the obb intersects the triangle
        ///
        static inline bool obbIntersectsTriangle(const OBB<T> &obb, const Triangle<T> &t);
    };

}