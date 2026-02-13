#pragma once

#include "collision_common.h"

#include "Plane.h"

namespace CollisionCore
{

    struct FrustumVertex
    {
        static constexpr int Near_Right_Top = 0;
        static constexpr int Near_Right_Bottom = 1;
        static constexpr int Near_Left_Bottom = 2;
        static constexpr int Near_Left_Top = 3;

        static constexpr int Far_Right_Top = 4;
        static constexpr int Far_Right_Bottom = 5;
        static constexpr int Far_Left_Bottom = 6;
        static constexpr int Far_Left_Top = 7;
    };

    struct FrustumPlane
    {
        static constexpr int Right = 0;
        static constexpr int Left = 1;
        static constexpr int Bottom = 2;
        static constexpr int Top = 3;
        static constexpr int Near = 4;
        static constexpr int Far = 5;
        static constexpr int Count = 6;
    };

    /// \brief Camera frustum representation
    ///
    /// It can be constructed from the camera projection and modelview matrix.
    ///
    /// Could be used to implement the frustum culling of other shapes (point, sphere, aabb)
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    class Frustum<T, _CHKE_>
    {
    public:
        using self_type = Frustum<T>;

        using float_type = typename MathCore::MathTypeInfo<T>::_type;
        using vec2_type = typename MathCore::MathTypeInfo<T>::_vec2;
        using vec3_type = T; // typename MathCore::MathTypeInfo<T>::_vec3;
        using vec4_type = typename MathCore::MathTypeInfo<T>::_vec4;
        using quat_type = typename MathCore::MathTypeInfo<T>::_quat;
        using mat2_type = typename MathCore::MathTypeInfo<T>::_mat2;
        using mat3_type = typename MathCore::MathTypeInfo<T>::_mat3;
        using mat4_type = typename MathCore::MathTypeInfo<T>::_mat4;

    private:
        inline void computePlanes(const mat4_type &clipMatrix, bool depth_zero_one);

        float_type minProjections[6];
        float_type maxProjections[6];

    public:
        //Plane<T> rightPlane, leftPlane, bottomPlane, topPlane, nearPlane, farPlane;
        Plane<T> planes[6];
        vec3_type vertices[8];
        AABB<T> aabb;
        OBB<T> obb;

        /// \brief Access one of the 6 planes that compose this frustum
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Frustum frustum;
        ///
        /// Plane right = frustum[0];
        /// Plane left = frustum[1];
        /// Plane bottom = frustum[2];
        /// Plane top = frustum[3];
        /// Plane near = frustum[4];
        /// Plane far = frustum[5];
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline Plane<T> &operator[](int idx);

        /// \brief Access one of the 6 planes that compose this frustum
        ///
        /// Example:
        ///
        /// \code
        ///
        /// void process_frustum( const Frustum &frustum ) {
        ///     Plane right = frustum[0];
        ///     Plane left = frustum[1];
        ///     Plane bottom = frustum[2];
        ///     Plane top = frustum[3];
        ///     Plane near = frustum[4];
        ///     Plane far = frustum[5];
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline const Plane<T> &operator[](int idx) const;

        /// default constructor
        inline Frustum();

        /// \brief Construct a frustum from the camera projection matrix
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 camera_projection;
        ///
        /// Frustum frustum = Frustum( camera_projection );
        ///
        /// Frustum frustum_a = camera_projection;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline Frustum(const mat4_type &projection, bool depth_zero_one = false);

        /// \brief Construct a frustum from the camera projection matrix and the transformation_matrix
        ///
        /// Example:
        ///
        /// \code
        ///
        /// mat4 camera_projection;
        /// mat4 camera_inverse_transformation;
        ///
        /// Frustum frustum = Frustum( camera_projection, camera_inverse_transformation );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        inline Frustum(const mat4_type &projection, const mat4_type &camera, bool depth_zero_one = false);

        /// \brief Test if a point is inside the frustum
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Frustum frustum;
        /// vec3 point;
        ///
        /// if ( Frustum::pointInsideFrustum( point, frustum ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param p The 3D point
        /// \param f The frustum
        /// \return true, if the point is inside the frustum
        ///
        static inline bool pointInsideFrustum(const vec3_type &p, const Frustum<T> &f);

        /// \brief Test if a frustum overlaps the sphere
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Frustum frustum;
        /// // sphere
        /// vec3 sphere_center;
        /// float sphere_radius;
        ///
        /// if ( Frustum::sphereOverlapsFrustum( sphere_center, sphere_radius, frustum ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param center The sphere center
        /// \param radius The sphere radius
        /// \param f The frustum
        /// \return true, if the sphere overlaps the frustum
        ///
        static inline bool sphereOverlapsFrustum(const vec3_type &center, const float_type &radius, const Frustum<T> &f);

        /// \brief Test if a frustum overlaps the sphere
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Frustum frustum;
        /// Sphere sphere;
        ///
        /// if ( Frustum::sphereOverlapsFrustum( sphere, frustum ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param s The sphere
        /// \param f The frustum
        /// \return true, if the sphere overlaps the frustum
        ///
        static inline bool sphereOverlapsFrustum(const Sphere<T> &s, const Frustum<T> &f);

        /// \brief Test if a frustum overlaps the aabb
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Frustum frustum;
        /// AABB aabb;
        ///
        /// if ( Frustum::aabbOverlapsFrustum( aabb, frustum ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param aabb The AABB
        /// \param f The frustum
        /// \return true, if the aabb overlaps the frustum
        ///
        static inline bool aabbOverlapsFrustum(const AABB<T> &aabb, const Frustum<T> &f);

        /// \brief Test if a frustum overlaps the oobb
        ///
        /// Example:
        ///
        /// \code
        ///
        /// Frustum frustum;
        /// OOBB oobb;
        ///
        /// if ( Frustum::oobbOverlapsFrustum( oobb, frustum ) ) {
        ///     ...
        /// }
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param oobb The OOBB
        /// \param f The frustum
        /// \return true, if the aabb overlaps the frustum
        ///
        static inline bool obbOverlapsFrustum(const OBB<T> &obb, const Frustum<T> &f);
    };
}