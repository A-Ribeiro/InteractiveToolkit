#pragma once

#include "collision_common.h"

namespace CollisionCore
{

    /// \brief Ray representation
    ///
    /// Used in the raycast methods.
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    class Ray<T, _CHKE_>
    {
    public:
        using self_type = OBB<T>;

        using float_type = typename MathCore::MathTypeInfo<T>::_type;
        using vec2_type = typename MathCore::MathTypeInfo<T>::_vec2;
        using vec3_type = T; // typename MathCore::MathTypeInfo<T>::_vec3;
        using vec4_type = typename MathCore::MathTypeInfo<T>::_vec4;
        using quat_type = typename MathCore::MathTypeInfo<T>::_quat;
        using mat2_type = typename MathCore::MathTypeInfo<T>::_mat2;
        using mat3_type = typename MathCore::MathTypeInfo<T>::_mat3;
        using mat4_type = typename MathCore::MathTypeInfo<T>::_mat4;

        vec3_type origin;
        vec3_type dir;

        /// \brief Construct a Ray with origin = dir = vec3(0)
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Ray ray;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline Ray();

        /// \brief Construct a Ray
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 origin, direction;
        ///
        /// Ray ray = Ray( origin, direction );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline Ray(const vec3_type &origin, const vec3_type &dir);

        //
        // Cloned methods from other collision classes
        //

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
        /// if ( Ray::raycastAABB(ray, aabb, &tmin, &normal) ) {
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

        /// \brief Raycast test against a Plane
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against Plane
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
        /// Plane plane;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Ray::raycastPlane(ray, aabb, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r The Ray
        /// \param plane The Plane
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the plane
        ///
        static inline bool raycastPlane(const Ray<T> &r, const Plane<T> &plane, float_type *outTmin, vec3_type *outNormal);

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
        /// if ( Ray::raycastSphere(ray, sphere_center, sphere_radius, &tmin, &normal) ) {
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
        /// if ( Ray::raycastSphere(ray, sphere, &tmin, &normal) ) {
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

        /// \brief Raycast test against a Triangle
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against a Triangle
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
        /// //triangle
        /// vec3 a, b, c;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Ray::raycastTriangle(ray, a, b, c, &tmin, &normal) ) {
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
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Ray ray;
        /// Triangle triangle;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Ray::raycastTriangle(ray, triangle, &tmin, &normal) ) {
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

        /// \brief Raycast test against an OBB
        ///
        /// Intersect ray R(t) = p + t*d, |d| = 1, against OBB a.
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
        /// OBB obb;
        ///
        /// float tmin;
        /// vec3 normal;
        /// if ( Ray::raycastOBB(ray, obb, &tmin, &normal) ) {
        ///     vec3 collision_ptn = ray.origin + ray.dir * tmin;
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r The Ray
        /// \param a The OBB
        /// \param outTmin **return** the distance from ray origin
        /// \param outNormal **return** the surface normal
        /// \return true, if the ray intersects the obb
        ///
        static inline bool raycastOBB(const Ray<T> &r, const OBB<T> &a, float_type *outTmin, vec3_type *outNormal);
    };

}