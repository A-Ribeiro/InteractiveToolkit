#pragma once

#include "collision_common.h"

namespace CollisionCore
{

    /// \brief Plane representation
    ///
    /// It stores the plane normal and the distance to origin.
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    class Plane<T, _CHKE_>
    {
    public:
        using self_type = Plane<T>;

        using float_type = typename MathCore::MathTypeInfo<T>::_type;
        using vec2_type = typename MathCore::MathTypeInfo<T>::_vec2;
        using vec3_type = T; // typename MathCore::MathTypeInfo<T>::_vec3;
        using vec4_type = typename MathCore::MathTypeInfo<T>::_vec4;
        using quat_type = typename MathCore::MathTypeInfo<T>::_quat;
        using mat2_type = typename MathCore::MathTypeInfo<T>::_mat2;
        using mat3_type = typename MathCore::MathTypeInfo<T>::_mat3;
        using mat4_type = typename MathCore::MathTypeInfo<T>::_mat4;

        vec3_type normal;
        float_type distance;

        /// \brief Construct a Plane with normal = vec3(0,1,0) and distance = 0
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Plane plane;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline Plane();

        /// \brief Construct a Plane
        ///
        /// With this constructor it is possible to define a plane using a 3d point and a normal.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 point_in_plane;
        /// vec3 normal;
        ///
        /// Plane plane = Plane( point_in_plane, normal );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param point a point in the plane
        /// \param normal the plane normal
        ///
        inline Plane(const vec3_type &point, const vec3_type &normal);

        /// \brief Construct a Plane
        ///
        /// With this constructor it is possible to define a plane using three other points (a triangle).
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // triangle a, b, c
        /// vec3 a, b, c;
        ///
        /// Plane plane = Plane( a, b, c );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a triangle vertex a
        /// \param b triangle vertex b
        /// \param c triangle vertex c
        ///
        inline Plane(const vec3_type &a, const vec3_type &b, const vec3_type &c);

        /// \brief Make the plane normal to have the magnitude 1
        ///
        /// Normalize the normal from the plane equation.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Plane plane;
        ///
        /// plane.normalize();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline void normalize();

        //
        // static  inline constructors
        //
        /// \brief Create a Plane from a triangle
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// // triangle a, b, c
        /// vec3 a, b, c;
        ///
        /// Plane plane = Plane::fromTriangle( a, b, c );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a triangle vertex a
        /// \param b triangle vertex b
        /// \param c triangle vertex c
        /// \return The plane from a triangle
        ///
        static inline Plane<T> fromTriangle(const vec3_type &a, const vec3_type &b, const vec3_type &c);

        /// \brief Create a Plane from a triangle
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
        /// Plane plane = Plane::fromTriangle( triangle );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param t Triangle
        /// \return The plane from a triangle
        ///
        static inline Plane<T> fromTriangle(const Triangle<T> &t);

        //
        // Point operations
        //

        /// \brief Compute the point in the plane that is closer to another specified point
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 ptn_to_input;
        /// Plane plane;
        ///
        /// vec3 closest_point = Plane::closestPointToPlane( ptn_to_input, plane );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point to compute closest point
        /// \param plane The Plane
        /// \return The closest point in the plane related to p
        ///
        static inline vec3_type closestPointToPlane(const vec3_type &p, const Plane<T> &plane);

        /// \brief Compute the distance of a point related to the plane
        ///
        /// This distance can be seen as the projection of that point in the plane's normal.
        ///
        /// The result has a sign, indicating the side the point is related to the plane.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// vec3 point;
        /// Plane plane;
        ///
        /// float signed_distance = Plane::pointDistanceToPlane( point, plane );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The input point
        /// \param plane The Plane
        /// \return Signed distance of the point related to the plane
        ///
        static inline float_type pointDistanceToPlane(const vec3_type &p, const Plane<T> &plane);

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
        /// if ( Plane::raycastPlane(ray, aabb, &tmin, &normal) ) {
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
        /// if ( Plane::segmentIntersectsPlane( a, b, plane ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param a line segment point a
        /// \param b line segment point b
        /// \param plane The Plane
        /// \return true, if the plane intersects the line segment
        ///
        static inline bool segmentIntersectsPlane(const vec3_type &a, const vec3_type &b, const Plane<T> &plane);

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
        /// if ( Plane::segmentIntersectsPlane( lineSegment, plane ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param ls line segment
        /// \param plane The Plane
        /// \return true, if the plane intersects the line segment
        ///
        static inline bool segmentIntersectsPlane(const LineSegment<T> &ls, const Plane<T> &plane);

        /// \brief Compute a plane to plane intersection with two (2) planes
        ///
        /// Given planes p1 and p2, compute line L = p+t*d of their intersection.
        ///
        /// The plane to plane intersetion, when occurs, can lead to a line result.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Plane plane_a, plane_b;
        ///
        /// vec3 line_position, line_direction;
        /// if ( Plane::intersectPlaneToPlane( plane_a, plane_b, &line_position, &line_direction ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p1 The first plane
        /// \param p2 The second plane
        /// \param outP **return** line position
        /// \param outD **return** line direction
        /// \return true, if the plane to plane intersection has a result
        ///
        static inline bool intersectPlaneToPlane(const Plane<T> &p1, const Plane<T> &p2, vec3_type *outP, vec3_type *outD);

        /// \brief Compute a plane to plane intersection with three (3) planes
        ///
        /// Compute the point p at which the three planes p1, p2 and p3 intersect (if at all)
        ///
        /// The plane to plane (three planes) intersetion, when occurs, can lead to a point.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Plane plane_a, plane_b, plane_c;
        ///
        /// vec3 position;
        /// if ( Plane::intersectPlanes( plane_a, plane_b, plane_c, &position ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p1 The first plane
        /// \param p2 The second plane
        /// \param p3 The third plane
        /// \param outP **return** intersection position
        /// \return true, if the plane to plane to plane intersection has a result
        ///
        static inline bool intersectPlanes(const Plane<T> &p1, const Plane<T> &p2, const Plane<T> &p3, vec3_type *outP);

        //
        // Cloned methods from other collision classes
        //

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
        /// if ( Plane::aabbIntersectsPlane( aabb, plane ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param plane The Plane
        /// \return true, if the aabb intersects the plane
        ///
        static inline bool aabbIntersectsPlane(const AABB<T> &aabb, const Plane<T> &plane);

        /// \brief Test if a plane intersects the OBB
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        /// using namespace aRibeiro::collision;
        ///
        /// Plane plane;
        /// OBB obb;
        ///
        /// if ( Plane::obbIntersectsPlane( obb, plane ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param obb The OBB
        /// \param plane The Plane
        /// \return true, if the obb intersects the plane
        ///
        static inline bool obbIntersectsPlane(const OBB<T> &obb, const Plane<T> &plane);
    };
}
