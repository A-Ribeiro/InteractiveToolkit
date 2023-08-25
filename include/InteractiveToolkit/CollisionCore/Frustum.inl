#include "Frustum.h"

namespace CollisionCore
{

    template <typename T>
    inline void Frustum<T, _CHKE_>::computePlanes(const mat4_type &matrix, bool depth_zero_one)
    {
        using namespace MathCore;
        // using mat4_type = typename Frustum<T>::mat4_type;
        // using vec4_type = typename Frustum<T>::vec4_type;
        // using vec3_type = typename Frustum<T>::vec3_type;
        // using float_type = typename Frustum<T>::float_type;
        // using quat_type = typename Frustum<T>::quat_type;

        mat4_type clipMatrix = OP<mat4_type>::transpose(matrix);
        /*
                    0 _11,1 _21,2 _31,3 _41,
                    4 _12,5 _22,6 _32,7 _42,
                    8 _13,9 _23,10_33,11_43,
                    12_14,13_24,14_34,15_44;
        */

        // alias for planes
        Plane<T> &rightPlane = planes[FrustumPlane::Right],
                 &leftPlane = planes[FrustumPlane::Left],
                 &bottomPlane = planes[FrustumPlane::Bottom],
                 &topPlane = planes[FrustumPlane::Top],
                 &nearPlane = planes[FrustumPlane::Near],
                 &farPlane = planes[FrustumPlane::Far];

        vec4_type aux = clipMatrix[3] - clipMatrix[0];
        rightPlane.normal = CVT<vec4_type>::toVec3(aux);
        rightPlane.distance = -aux.w;
        rightPlane.normalize();

        aux = clipMatrix[3] + clipMatrix[0];
        leftPlane.normal = CVT<vec4_type>::toVec3(aux);
        leftPlane.distance = -aux.w;
        leftPlane.normalize();

        aux = clipMatrix[3] + clipMatrix[1];
        bottomPlane.normal = CVT<vec4_type>::toVec3(aux);
        bottomPlane.distance = -aux.w;
        bottomPlane.normalize();

        aux = clipMatrix[3] - clipMatrix[1];
        topPlane.normal = CVT<vec4_type>::toVec3(aux);
        topPlane.distance = -aux.w;
        topPlane.normalize();

        if (depth_zero_one)
            aux = clipMatrix[2];
        else
            aux = clipMatrix[3] + clipMatrix[2];
        nearPlane.normal = CVT<vec4_type>::toVec3(aux);
        nearPlane.distance = -aux.w;
        nearPlane.normalize();

        aux = clipMatrix[3] - clipMatrix[2];
        farPlane.normal = CVT<vec4_type>::toVec3(aux);
        farPlane.distance = -aux.w;
        farPlane.normalize();

        //
        // Compute vertices
        //

        Plane<vec3_type>::intersectPlanes(rightPlane, topPlane, nearPlane,
                                          &vertices[FrustumVertex::Near_Right_Top]);
        Plane<vec3_type>::intersectPlanes(rightPlane, bottomPlane, nearPlane,
                                          &vertices[FrustumVertex::Near_Right_Bottom]);
        Plane<vec3_type>::intersectPlanes(leftPlane, bottomPlane, nearPlane,
                                          &vertices[FrustumVertex::Near_Left_Bottom]);
        Plane<vec3_type>::intersectPlanes(leftPlane, topPlane, nearPlane,
                                          &vertices[FrustumVertex::Near_Left_Top]);

        Plane<vec3_type>::intersectPlanes(rightPlane, topPlane, farPlane,
                                          &vertices[FrustumVertex::Far_Right_Top]);
        Plane<vec3_type>::intersectPlanes(rightPlane, bottomPlane, farPlane,
                                          &vertices[FrustumVertex::Far_Right_Bottom]);
        Plane<vec3_type>::intersectPlanes(leftPlane, bottomPlane, farPlane,
                                          &vertices[FrustumVertex::Far_Left_Bottom]);
        Plane<vec3_type>::intersectPlanes(leftPlane, topPlane, farPlane,
                                          &vertices[FrustumVertex::Far_Left_Top]);

        aabb = AABB<vec3_type>(vertices[0], vertices[1]);
        for (int i = 2; i < 8; i++)
        {
            aabb.min_box = OP<vec3_type>::minimum(aabb.min_box, vertices[i]);
            aabb.max_box = OP<vec3_type>::maximum(aabb.max_box, vertices[i]);
        }

        // aabb = AABB::joinAABB(aabb, AABB(vertices[i],vertices[i+1]) );

        // pre-calculate the vertex projections over the frustum plane
        for (int i = 0; i < 6; i++)
            Util::projectOnAxis(vertices, 8, (*this)[i].normal, &minProjections[i], &maxProjections[i]);

        //
        // OBB calculation...
        //

        // vec3 center = nearPlane.normal * ( (nearPlane.distance - farPlane.distance) * 0.5f );
        vec3_type right = OP<vec3_type>::normalize(vertices[FrustumVertex::Far_Right_Bottom] - vertices[FrustumVertex::Far_Left_Bottom]);
        // vec3 up = normalize( vertices[Far_Right_Top] - vertices[Far_Right_Bottom] );
        vec3_type front = nearPlane.normal;

        // const vec4_type _0001 = vec4_type(0, 0, 0, 1);
        // mat4_type rotationBase = mat4_type(CVT<vec3_type>::toVec4(right), CVT<vec3_type>::toVec4(OP<vec3_type>::cross(front, right)), CVT<vec3_type>::toVec4(front), _0001);
        // quat_type rotationBase_quat = GEN<quatf>::fromMat4(rotationBase);

        mat3_type rotationBase = mat3_type(right, OP<vec3_type>::cross(front, right), front);
        quat_type rotationBase_quat = GEN<quatf>::fromMat3(rotationBase);

        float_type dimension_min[3];
        float_type dimension_max[3];

        float_type center_local[3];
        vec3_type dimension;

        vec3_type center; // = inv( rotationBase_quat ) * center_local;

        for (int i = 0; i < 3; i++)
        {
            // Util::projectOnAxis(vertices, 8, CVT<vec4_type>::toVec3(rotationBase[i]), &dimension_min[i], &dimension_max[i]);
            Util::projectOnAxis(vertices, 8, rotationBase[i], &dimension_min[i], &dimension_max[i]);
            dimension[i] = dimension_max[i] - dimension_min[i];
            center_local[i] = (dimension_min[i] + dimension_max[i]) * (float_type)0.5;
            // center += CVT<vec4_type>::toVec3(rotationBase[i]) * center_local[i];
            center += rotationBase[i] * center_local[i];
        }

        obb = OBB<T>(center, dimension, rotationBase_quat);
    }

    template <typename T>
    inline Plane<T> &Frustum<T, _CHKE_>::operator[](int idx)
    {
        return planes[idx];
    }

    template <typename T>
    inline const Plane<T> &Frustum<T, _CHKE_>::operator[](int idx) const
    {
        return planes[idx];
    }

    template <typename T>
    inline Frustum<T, _CHKE_>::Frustum()
    {
    }

    template <typename T>
    inline Frustum<T, _CHKE_>::Frustum(const mat4_type &projection, bool depth_zero_one)
    {
        computePlanes(projection, depth_zero_one);
    }

    template <typename T>
    inline Frustum<T, _CHKE_>::Frustum(const mat4_type &projection, const mat4_type &camera, bool depth_zero_one)
    {
        computePlanes(projection * camera, depth_zero_one);
    }

    template <typename T>
    inline bool Frustum<T, _CHKE_>::pointInsideFrustum(const vec3_type &p, const Frustum<T> &frustum)
    {
        for (int i = 0; i < 6; i++)
        {
            if (Plane<T>::pointDistanceToPlane(p, frustum[i]) < 0)
                return false;
        }
        return true;
    }

    template <typename T>
    inline bool Frustum<T, _CHKE_>::sphereOverlapsFrustum(const vec3_type &center, const float_type &radius, const Frustum<T> &frustum)
    {
        for (int i = 0; i < 6; i++)
        {
            if (Plane<T>::pointDistanceToPlane(center, frustum[i]) < -radius)
                return false;
        }
        return true;
    }

    template <typename T>
    inline bool Frustum<T, _CHKE_>::sphereOverlapsFrustum(const Sphere<T> &s, const Frustum<T> &frustum)
    {
        return Frustum<T>::sphereOverlapsFrustum(s.center, s.radius, frustum);
    }

    template <typename T>
    inline bool Frustum<T, _CHKE_>::aabbOverlapsFrustum(const AABB<T> &aabb, const Frustum<T> &frustum)
    {
        using namespace MathCore;
        if (!AABB<T>::aabbOverlapsAABB(aabb, frustum.aabb))
            return false;

        /*
        // Test the box normals (x-, y- and z-axes)
        vec3 boxNormals[] = {
            vec3(1,0,0),
            vec3(0,1,0),
            vec3(0,0,1)
        };

        float_type frustumMin,frustumMax;
        for (int i = 0; i < 3; i++)
        {
            projectOnAxis(frustum.vertices, 8, boxNormals[i], &frustumMin, &frustumMax);
            if (frustumMax < aabb.min_box[i] - EPSILON || frustumMin > aabb.max_box[i] + EPSILON)
                return false; // No intersection possible.
        }
        */

        vec3_type box_Vertices[] = {
            vec3_type(aabb.min_box.x, aabb.min_box.y, aabb.min_box.z), // 000
            vec3_type(aabb.min_box.x, aabb.min_box.y, aabb.max_box.z), // 001
            vec3_type(aabb.min_box.x, aabb.max_box.y, aabb.min_box.z), // 010
            vec3_type(aabb.min_box.x, aabb.max_box.y, aabb.max_box.z), // 011
            vec3_type(aabb.max_box.x, aabb.min_box.y, aabb.min_box.z), // 100
            vec3_type(aabb.max_box.x, aabb.min_box.y, aabb.max_box.z), // 101
            vec3_type(aabb.max_box.x, aabb.max_box.y, aabb.min_box.z), // 110
            vec3_type(aabb.max_box.x, aabb.max_box.y, aabb.max_box.z)  // 111
        };

        float_type boxMin, boxMax;

        for (int i = 0; i < 6; i++)
        {
            Util::projectOnAxis(box_Vertices, 8, frustum[i].normal, &boxMin, &boxMax);
            // projectOnAxis(frustum.vertices, 8, frustum[i].normal , &frustum.minProjections[i], &frustum.maxProjections[i]);
            if (boxMax < frustum.minProjections[i] - EPSILON<float_type>::high_precision || boxMin > frustum.maxProjections[i] + EPSILON<float_type>::high_precision)
                return false; // No intersection possible.
        }

        return true;

        /*
        vec3 center = (aabb.min_box + aabb.max_box) * 0.5;
        vec3 halfbounds = (aabb.max_box - aabb.min_box) * 0.5;

        for (int i = 0; i < 6; i++) {

            if (Plane::pointDistanceToPlane(center + halfbounds * vec3(1, 1, 1), frustum[i]) >= 0.0f) continue; // 000
            if (Plane::pointDistanceToPlane(center + halfbounds * vec3(1, 1, -1), frustum[i]) >= 0.0f) continue; // 001
            if (Plane::pointDistanceToPlane(center + halfbounds * vec3(1, -1, 1), frustum[i]) >= 0.0f) continue; // 010
            if (Plane::pointDistanceToPlane(center + halfbounds * vec3(1, -1, -1), frustum[i]) >= 0.0f) continue; // 011
            if (Plane::pointDistanceToPlane(center + halfbounds * vec3(-1, 1, 1), frustum[i]) >= 0.0f) continue; // 100
            if (Plane::pointDistanceToPlane(center + halfbounds * vec3(-1, 1, -1), frustum[i]) >= 0.0f) continue; // 101
            if (Plane::pointDistanceToPlane(center + halfbounds * vec3(-1, -1, 1), frustum[i]) >= 0.0f) continue; // 110
            if (Plane::pointDistanceToPlane(center + halfbounds * vec3(-1, -1, -1), frustum[i]) >= 0.0f) continue; // 111

            return false;
        }
        return true;
        */
    }

    template <typename T>
    inline bool Frustum<T, _CHKE_>::obbOverlapsFrustum(const OBB<T> &obb, const Frustum<T> &frustum)
    {
        using namespace MathCore;
        float_type frustumMin, frustumMax;
        for (int i = 0; i < 3; i++)
        {
            Util::projectOnAxis(frustum.vertices, 8, obb.right_up_depth_vec[i], &frustumMin, &frustumMax);
            if (frustumMax < obb.center_right_up_depth_proj_min[i] || frustumMin > obb.center_right_up_depth_proj_max[i])
                return false; // No intersection possible.
        }

        float_type boxMin, boxMax;
        for (int i = 0; i < 6; i++)
        {
            Util::projectOnAxis(obb.box_vertices, 8, frustum[i].normal, &boxMin, &boxMax);
            if (boxMax < frustum.minProjections[i] || boxMin > frustum.maxProjections[i])
                return false; // No intersection possible.
        }

        return true;
    }

}
