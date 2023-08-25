#include "OBB.h"

namespace CollisionCore
{

    template <typename T> inline
    void OBB<T, _CHKE_>::computeOptimizationData()
    {
        using namespace MathCore;
        // using vec3_type = typename OBB<T>::vec3_type;
        // using float_type = typename OBB<T>::float_type;
        // using quat_type = typename OBB<T>::quat_type;

        right_up_depth_vec[0] = orientation * vec3_type(1, 0, 0);
        right_up_depth_vec[1] = orientation * vec3_type(0, 1, 0);
        right_up_depth_vec[2] = orientation * vec3_type(0, 0, 1);

        for (int i = 0; i < 3; i++)
        {
            float_type center_projected = OP<vec3_type>::dot(center, right_up_depth_vec[i]);
            center_right_up_depth_proj_min[i] = center_projected - dimension_2[i];
            center_right_up_depth_proj_max[i] = center_projected + dimension_2[i];
        }

        vec3_type right = right_up_depth_vec[0] * dimension_2.x;
        vec3_type top = right_up_depth_vec[1] * dimension_2.y;
        vec3_type depth = right_up_depth_vec[2] * dimension_2.z;

        box_vertices[0] = center - right - top - depth; // 000
        box_vertices[1] = center - right - top + depth; // 001
        box_vertices[2] = center - right + top - depth; // 010
        box_vertices[3] = center - right + top + depth; // 011
        box_vertices[4] = center + right - top - depth; // 100
        box_vertices[5] = center + right - top + depth; // 101
        box_vertices[6] = center + right + top - depth; // 110
        box_vertices[7] = center + right + top + depth; // 111

        /*

         box_vertices[0] = center + orientation * vec3_type(-dimension_2.x,-dimension_2.y,-dimension_2.z);// 000
         box_vertices[1] = center + orientation * vec3_type(-dimension_2.x,-dimension_2.y, dimension_2.z);// 001
         box_vertices[2] = center + orientation * vec3_type(-dimension_2.x, dimension_2.y,-dimension_2.z);// 010
         box_vertices[3] = center + orientation * vec3_type(-dimension_2.x, dimension_2.y, dimension_2.z);// 011
         box_vertices[4] = center + orientation * vec3_type( dimension_2.x,-dimension_2.y,-dimension_2.z);// 100
         box_vertices[5] = center + orientation * vec3_type( dimension_2.x,-dimension_2.y, dimension_2.z);// 101
         box_vertices[6] = center + orientation * vec3_type( dimension_2.x, dimension_2.y,-dimension_2.z);// 110
         box_vertices[7] = center + orientation * vec3_type( dimension_2.x, dimension_2.y, dimension_2.z);// 111
 */
    }

    template <typename T> inline
    OBB<T, _CHKE_>::OBB()
    {
        computeOptimizationData();
    }

    template <typename T> inline
    OBB<T, _CHKE_>::OBB(const vec3_type &_center, const vec3_type &_dimension, const quat_type &_orientation)
    {
        setOBB(_center, _dimension, _orientation);
    }

    template <typename T> inline
    OBB<T, _CHKE_>::OBB(const mat4_type &_transform)
    {
        setOBB(_transform);
    }

    template <typename T> inline
    void OBB<T, _CHKE_>::setOBB(const vec3_type &_center, const vec3_type &_dimension, const quat_type &_orientation)
    {
        // using float_type = typename OBB<T>::float_type;

        center = _center;
        dimension_2 = _dimension * (float_type)0.5;
        orientation = _orientation;

        computeOptimizationData();
    }

    template <typename T> inline
    void OBB<T, _CHKE_>::setOBB(const mat4_type &_transform)
    {
        using namespace MathCore;
        // using vec4_type = typename OBB<T>::vec4_type;
        // using vec3_type = typename OBB<T>::vec3_type;
        // using float_type = typename OBB<T>::float_type;
        // using quat_type = typename OBB<T>::quat_type;

        center = CVT<vec4_type>::toVec3(_transform[3]);
        dimension_2 = vec3_type(
                          OP<vec4_type>::length(_transform[0]),
                          OP<vec4_type>::length(_transform[1]),
                          OP<vec4_type>::length(_transform[2])) *
                      (float_type)0.5;

        orientation = GEN<quat_type>::fromMat4(_transform);

        computeOptimizationData();
    }

    //--------------------------------------------------------------------------
    // Static methods
    //--------------------------------------------------------------------------

    template <typename T> inline
    bool OBB<T, _CHKE_>::pointInsideOBB(const vec2_type &ptn, const OBB<T> &b)
    {
        using namespace MathCore;
        for (int i = 0; i < 2; i++)
        {
            float_type ptn_projected = OP<vec2_type>::dot(ptn, vec2_type(b.right_up_depth_vec[i].x, b.right_up_depth_vec[i].y));
            if ((ptn_projected < b.center_right_up_depth_proj_min[i]) || (ptn_projected > b.center_right_up_depth_proj_max[i]))
                return false;
        }
        return true;
    }

    template <typename T> inline
    bool OBB<T, _CHKE_>::pointInsideOBB(const vec3_type &ptn, const OBB<T> &b)
    {
        using namespace MathCore;
        for (int i = 0; i < 3; i++)
        {
            float_type ptn_projected = OP<vec3_type>::dot(ptn, b.right_up_depth_vec[i]);
            if ((ptn_projected < b.center_right_up_depth_proj_min[i]) || (ptn_projected > b.center_right_up_depth_proj_max[i]))
                return false;
        }
        return true;
    }

    template <typename T> inline
    bool OBB<T, _CHKE_>::obbOverlapsOBB(const OBB<T> &a, const OBB<T> &b)
    {
        using namespace MathCore;

        float_type min, max;

        // test vertices from 'b' into 'a' orientation
        for (int i = 0; i < 3; i++)
        {
            Util::projectOnAxis(b.box_vertices, 8, a.right_up_depth_vec[i], &min, &max);
            if ((max < a.center_right_up_depth_proj_min[i]) || (min > a.center_right_up_depth_proj_max[i]))
                return false; // No intersection possible.
        }

        // test vertices from 'a' into 'b' orientation
        for (int i = 0; i < 3; i++)
        {
            Util::projectOnAxis(a.box_vertices, 8, b.right_up_depth_vec[i], &min, &max);
            if ((max < b.center_right_up_depth_proj_min[i]) || (min > b.center_right_up_depth_proj_max[i]))
                return false; // No intersection possible.
        }

        return true;
    }

    template <typename T> inline
    OBB<T> OBB<T, _CHKE_>::fromAABB(const AABB<T> &a)
    {
        vec3_type center = (a.min_box + a.max_box) * (float_type)0.5;
        vec3_type dimension = (a.max_box - a.min_box);
        return OBB<T>(center, dimension, quat_type());
    }

    template <typename T> inline
    OBB<T> OBB<T, _CHKE_>::fromTriangle(const vec3_type &a, const vec3_type &b, const vec3_type &c)
    {
        using namespace MathCore;

        vec3_type ab = b - a;
        vec3_type ab_unit = OP<vec3_type>::normalize(ab);

        vec3_type normal = OP<vec3_type>::cross(ab, c - a);
        vec3_type normal_unit = OP<vec3_type>::normalize(normal);

        mat3_type rotationBase = mat3_type(ab_unit, OP<vec3_type>::cross(normal_unit, ab_unit), normal_unit);
        quat_type rotationBase_quat = GEN<quat_type>::fromMat3(rotationBase);

        float_type dimension_min[3];
        float_type dimension_max[3];

        vec3_type vertices[3] = {a, b, c};

        float_type center_local[3];
        vec3_type dimension;

        vec3_type center; // = inv( rotationBase_quat ) * center_local;

        for (int i = 0; i < 3; i++)
        {
            Util::projectOnAxis(vertices, 3, rotationBase[i], &dimension_min[i], &dimension_max[i]);
            dimension[i] = dimension_max[i] - dimension_min[i];
            center_local[i] = (dimension_min[i] + dimension_max[i]) * (float_type)0.5;
            center += rotationBase[i] * center_local[i];
        }

        // vec3 center = inv( rotationBase_quat ) * center_local;

        return OBB<T>(center, dimension, rotationBase_quat);
    }

    template <typename T> inline
    OBB<T> OBB<T, _CHKE_>::fromTriangle(const Triangle<T> &triangle)
    {
        return OBB<T>::fromTriangle(triangle.a, triangle.b, triangle.c);
    }

    template <typename T> inline
    OBB<T> OBB<T, _CHKE_>::fromSphere(const vec3_type &pos, const float_type &radius)
    {
        return OBB<T>::fromAABB(AABB<T>::fromSphere(pos, radius));
    }

    template <typename T> inline
    OBB<T> OBB<T, _CHKE_>::fromSphere(const Sphere<T> &sphere)
    {
        return OBB<T>::fromSphere(sphere.center, sphere.radius);
    }

    template <typename T> inline
    OBB<T> OBB<T, _CHKE_>::fromLineSegment(const vec3_type &a, const vec3_type &b)
    {
        using namespace MathCore;

        vec3_type center = (a + b) * (float_type)0.5;
        vec3_type dimension;
        dimension.z = OP<vec3_type>::distance(a, b);

        vec3_type ab = OP<vec3_type>::normalize(b - a);
        vec3_type up = vec3_type(0, 1, 0);

        // 0º case construction...
        if (OP<vec3_type>::angleBetween(ab, up) <= EPSILON<float_type>::high_precision)
        {
            // const quat fixedRotation = quatFromEuler(DEG2RAD((float_type)0.5), DEG2RAD((float_type)0.5), DEG2RAD((float_type)0.5));
            up = CONSTANT<quat_type>::_180_move_slerp_case() * vec3_type(0, 1, 0);
        }

        quat_type orientation = GEN<quat_type>::quatLookAtRotationLH(ab, up);

        return OBB<T>(center, dimension, orientation);
    }

    template <typename T> inline
    OBB<T> OBB<T, _CHKE_>::fromLineSegment(const LineSegment<T> &ls)
    {
        return OBB<T>::fromLineSegment(ls.a, ls.b);
    }

    template <typename T> inline
    OBB<T> OBB<T, _CHKE_>::fromFrustum(const Frustum<T> &frustum)
    {
        return frustum.obb;
    }

    template <typename T> inline
    bool OBB<T, _CHKE_>::raycastOBB(const Ray<T> &r, const OBB<T> &a, float_type *outTmin, vec3_type *outNormal)
    {
        using namespace MathCore;
        Ray<T> r_transformed = Ray<T>(r.origin - a.center,
                                      OP<quat_type>::inverse(a.orientation) * r.dir);
        AABB<T> aabb = AABB<T>(-a.dimension_2, a.dimension_2);

        float_type t;
        vec3_type normal;
        if (AABB<T>::raycastAABB(r_transformed, aabb, &t, &normal))
        {
            // vec3_type pt = r_transformed.dir * t; //r_transformed.origin +
            // pt = a.orientation * pt;// + a.center - r.origin;
            // float_type t_2 = dot(pt, r.dir);

            normal = a.orientation * normal;

            *outTmin = t;
            *outNormal = normal;

            return true;
        }

        return false;
    }

    template <typename T> inline
    bool OBB<T, _CHKE_>::segmentIntersectsOBB(const vec3_type &p0, const vec3_type &p1, const OBB<T> &obb)
    {
        using namespace MathCore;

        vec3_type p0_p1 = p1 - p0;
        float_type lgth = OP<vec3_type>::length(p0_p1);
        vec3_type p0_p1_dir = p0_p1;
        if (lgth <= EPSILON<float_type>::high_precision)
        {
            lgth = (float_type)0;
            p0_p1_dir = vec3_type(1, 0, 0);
        }
        else
        {
            p0_p1_dir = p0_p1 / lgth;
        }

        vec3_type p0_transformed = p0 - obb.center;
        vec3_type p0_p1_dir_transformed = OP<quat_type>::inverse(obb.orientation) * p0_p1_dir;
        vec3_type p1_transformed = p0_transformed + p0_p1_dir_transformed * lgth;

        AABB<T> aabb = AABB<T>(-obb.dimension_2, obb.dimension_2);

        return AABB<T>::segmentIntersectsAABB(p0_transformed, p1_transformed, aabb);
    }

    template <typename T> inline
    bool OBB<T, _CHKE_>::segmentIntersectsOBB(const LineSegment<T> &ls, const OBB<T> &obb)
    {
        return OBB<T>::segmentIntersectsOBB(ls.a, ls.b, obb);
    }

    template <typename T> inline
    typename OBB<T, _CHKE_>::vec3_type OBB<T, _CHKE_>::closestPointToOBB(const vec3_type &p, const OBB<T> &obb)
    {
        using namespace MathCore;

        vec3_type d = p - obb.center;
        // Start result at center of box; make steps from there
        vec3_type q = obb.center;
        // For each OBB axis...
        for (int i = 0; i < 3; i++)
        {
            // ...project d onto that axis to get the distance
            // along the axis of d from the box center
            float_type dist = OP<vec3_type>::dot(d, obb.right_up_depth_vec[i]);
            // If distance farther than the box extents, clamp to the box
            dist = OP<float_type>::clamp(dist, obb.center_right_up_depth_proj_min[i], obb.center_right_up_depth_proj_max[i]);

            // if (dist > b.e[i]) dist = b.e[i];
            // if (dist < -b.e[i]) dist = -b.e[i];
            //  Step that distance along the axis to get world coordinate
            q += dist * obb.right_up_depth_vec[i];
        }

        return q;
    }

    template <typename T> inline
    bool OBB<T, _CHKE_>::aabbOverlapsOBB(const AABB<T> &_a, const OBB<T> &b)
    {
        using namespace MathCore;

        // OBB a_OBB = OBB<T>::fromAABB(_a);
        // return OBB<T>::OBBOverlapsOBB(a_OBB, b);

        float_type min, max;

        /*
        vec3_type a_normals[3] = {
            vec3_type(1,0,0),
            vec3_type(0,1,0),
            vec3_type(0,0,1)
        };*/

        vec3_type b_min, b_max;
        b_min = b_max = b.box_vertices[0];
        for (int i = 1; i < 8; i++)
        {
            b_min = OP<vec3_type>::minimum(b_min, b.box_vertices[i]);
            b_max = OP<vec3_type>::maximum(b_max, b.box_vertices[i]);
        }

        // test vertices from 'b' into 'a' orientation
        for (int i = 0; i < 3; i++)
        {
            // projectOnAxis(b.box_vertices, 8, a_normals[i], &min, &max);
            // if ((max < _a.min_box[i]) || (min > _a.max_box[i]) )
            if ((b_max[i] < _a.min_box[i]) || (b_min[i] > _a.max_box[i]))
                return false; // No intersection possible.
        }

        vec3_type a_box_vertices[] = {
            vec3_type(_a.min_box.x, _a.min_box.y, _a.min_box.z), // 000
            vec3_type(_a.min_box.x, _a.min_box.y, _a.max_box.z), // 001
            vec3_type(_a.min_box.x, _a.max_box.y, _a.min_box.z), // 010
            vec3_type(_a.min_box.x, _a.max_box.y, _a.max_box.z), // 011
            vec3_type(_a.max_box.x, _a.min_box.y, _a.min_box.z), // 100
            vec3_type(_a.max_box.x, _a.min_box.y, _a.max_box.z), // 101
            vec3_type(_a.max_box.x, _a.max_box.y, _a.min_box.z), // 110
            vec3_type(_a.max_box.x, _a.max_box.y, _a.max_box.z), // 111
        };

        // test vertices from 'a' into 'b' orientation
        for (int i = 0; i < 3; i++)
        {
            Util::projectOnAxis(a_box_vertices, 8, b.right_up_depth_vec[i], &min, &max);
            if ((max < b.center_right_up_depth_proj_min[i]) || (min > b.center_right_up_depth_proj_max[i]))
                return false; // No intersection possible.
        }

        return true;
    }

    template <typename T> inline
    bool OBB<T, _CHKE_>::sphereOverlapsOBB(const vec3_type &center, const float_type &radius, const OBB<T> &obb, vec3_type *penetration)
    {
        using namespace MathCore;

        vec3_type closestPointInTriangle = closestPointToOBB(center, obb);

        vec3_type SphereToOBB = closestPointInTriangle - center;
        float_type sqrLength_SphereToOBB = OP<vec3_type>::dot(SphereToOBB, SphereToOBB);

        float_type Max_Radius_sqr = radius * radius;

        if (sqrLength_SphereToOBB > (float_type)0.00002 && sqrLength_SphereToOBB < Max_Radius_sqr)
        {

            float_type Length_SphereToTriangle = OP<float_type>::sqrt(sqrLength_SphereToOBB);
            vec3_type SphereToTriangleNorm = SphereToOBB * ((float_type)1 / Length_SphereToTriangle); // normalize(SphereToTriangle);
            // Vector3 triangleNormal = Vectormath::Aos::normalize( Vectormath::Aos::cross( p2-p1 , p3-p1 ) );

            // EPSILON - to avoid process the same triangle again...
            const float_type EPSILON = (float_type)0.002;
            *penetration = SphereToTriangleNorm * (radius - Length_SphereToTriangle + EPSILON);
            return true;
        }
        return false;
    }

    template <typename T> inline
    bool OBB<T, _CHKE_>::sphereOverlapsOBB(const Sphere<T> &sphere, const OBB<T> &obb, vec3_type *penetration)
    {
        return OBB<T>::sphereOverlapsOBB(sphere.center, sphere.radius, obb, penetration);
    }

    template <typename T> inline
    bool OBB<T, _CHKE_>::planeIntersectsOBB(const Plane<T> &plane, const OBB<T> &obb)
    {
        using namespace MathCore;

        float_type min, max;
        Util::projectOnAxis(obb.box_vertices, 8, plane.normal, &min, &max);
        if (plane.distance < min || plane.distance > max)
            return false;
        return true;
    }

    template <typename T> inline
    bool OBB<T, _CHKE_>::triangleIntersectsOBB(const vec3_type &v0, const vec3_type &v1, const vec3_type &v2, const OBB<T> &obb)
    {
        using namespace MathCore;

        const float_type EPSILON = (float_type)1e-6;

        float_type triangleMin, triangleMax;
        float_type boxMin, boxMax;

        vec3_type triangle_Vertices[] = {
            v0, v1, v2};

        for (int i = 0; i < 3; i++)
        {
            Util::projectOnAxis(triangle_Vertices, 3, obb.right_up_depth_vec[i], &triangleMin, &triangleMax);
            if ((triangleMax < obb.center_right_up_depth_proj_min[i] - EPSILON) || (triangleMin > obb.center_right_up_depth_proj_max[i] + EPSILON))
                return false; // No intersection possible.
        }

        // Test the triangle normal
        vec3_type triangle_Normal = OP<vec3_type>::normalize(OP<vec3_type>::cross(v1 - v0, v2 - v0));
        float_type triangleOffset = OP<vec3_type>::dot(triangle_Normal, v0);

        Util::projectOnAxis(obb.box_vertices, 8, triangle_Normal, &boxMin, &boxMax);

        if ((boxMax < triangleOffset - EPSILON) || (boxMin > triangleOffset + EPSILON))
            return false; // No intersection possible.

        //
        // TODO: test the nine edge cross-products or single edge case
        //
        // Test the single edge
        vec3_type triangleEdges[] = {
            v0 - v1,
            v1 - v2,
            v2 - v0};

        for (int i = 0; i < 3; i++)
        {
            // The box normals are the same as it's edge tangents
            vec3_type axis = OP<vec3_type>::normalize(OP<vec3_type>::cross(triangleEdges[i], triangle_Normal));
            Util::projectOnAxis(obb.box_vertices, 8, axis, &boxMin, &boxMax);
            Util::projectOnAxis(triangle_Vertices, 3, axis, &triangleMin, &triangleMax);
            if ((boxMax < triangleMin - EPSILON) || (boxMin > triangleMax + EPSILON))
                return false; // No intersection possible
        }

        /*
        // Test the nine edge cross-products
        vec3_type triangleEdges[] = {
            v0 - v1,
            v1 - v2,
            v2 - v0
        };

        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
            {
                // The box normals are the same as it's edge tangents
                vec3_type axis = cross(triangleEdges[i], boxNormals[j]);
                projectOnAxis(box_Vertices, 8, axis, &boxMin, &boxMax);
                projectOnAxis(triangle_Vertices, 3, axis, &triangleMin, &triangleMax);
                if (boxMax < triangleMin - EPSILON || boxMin > triangleMax + EPSILON)
                    return false; // No intersection possible
            }
        */

        // No separating axis found.
        return true;
    }

    template <typename T> inline
    bool OBB<T, _CHKE_>::triangleIntersectsOBB(const Triangle<T> &t, const OBB<T> &obb)
    {
        return OBB<T>::triangleIntersectsOBB(t.a, t.b, t.c, obb);
    }

    //
    // Cloned methods from other collision classes
    //

    template <typename T> inline
    bool OBB<T, _CHKE_>::frustumOverlapsOBB(const Frustum<T> &f, const OBB<T> &obb)
    {
        return Frustum<T>::obbOverlapsFrustum(obb, f);
    }

}
