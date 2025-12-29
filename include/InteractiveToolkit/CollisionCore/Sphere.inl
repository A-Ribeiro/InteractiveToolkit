#include "Sphere.h"
#include "Frustum.h"

namespace CollisionCore
{

    template <typename T>
    inline Sphere<T, _CHKE_>::Sphere()
    {
        using namespace MathCore;
        // using vec3_type = typename Sphere<T>::vec3_type;
        // using float_type = typename Sphere<T>::float_type;
        // using quat_type = typename Sphere<T>::quat_type;

        radius = (float_type)1;
    }

    template <typename T>
    inline Sphere<T, _CHKE_>::Sphere(const vec3_type &center, float_type radius)
    {
        this->center = center;
        this->radius = radius;
    }

    template <typename T>
    inline
        typename Sphere<T, _CHKE_>::vec3_type
        Sphere<T, _CHKE_>::closestPointToSphere(const vec3_type &p, const vec3_type &center, const float_type &radius)
    {
        using namespace MathCore;
        return OP<vec3_type>::quadraticClamp(center, p, radius);
    }

    template <typename T>
    inline
        typename Sphere<T, _CHKE_>::vec3_type
        Sphere<T, _CHKE_>::closestPointToSphere(const vec3_type &p, const Sphere<T> &sphere)
    {
        return Sphere<T>::closestPointToSphere(p, sphere.center, sphere.radius);
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::sphereOverlapsSphere(const Sphere<T> &a, const Sphere<T> &b)
    {
        using namespace MathCore;

        float_type totalRadius = a.radius + b.radius;
        return OP<vec3_type>::sqrLength(a.center - b.center) <= totalRadius * totalRadius;
    }

    template <typename T>
    inline Sphere<T> Sphere<T, _CHKE_>::joinSpheres(const Sphere<T> &s0, const Sphere<T> &s1)
    {
        using namespace MathCore;

        Sphere<T> s;
        // Compute the squared distance between the sphere centers
        vec3_type d = s1.center - s0.center;
        float_type dist2 = OP<vec3_type>::dot(d, d);
        float_type sqr_r = (s1.radius - s0.radius);
        sqr_r *= sqr_r;
        if (sqr_r >= dist2)
        {
            // The sphere with the larger radius encloses the other;
            // just set s to be the larger of the two spheres
            if (s1.radius >= s0.radius)
                s = s1;
            else
                s = s0;
        }
        else
        {
            // Spheres partially overlapping or disjoint
            float_type dist = OP<float_type>::sqrt(dist2);
            s.radius = (dist + s0.radius + s1.radius) * (float_type)0.5;
            s.center = s0.center;
            if (dist > EPSILON<float_type>::high_precision)
                s.center += ((s.radius - s0.radius) / dist) * d;
        }
        return s;
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::raycastSphere(const Ray<T> &ray, const vec3_type &center, const float_type &radius, float_type *outT, vec3_type *outNormal)
    {
        using namespace MathCore;

        vec3_type m = ray.origin - center;
        float_type b = OP<vec3_type>::dot(m, ray.dir);
        float_type c = OP<vec3_type>::dot(m, m) - radius * radius;
        // Exit if ray origin outside s (c > 0) and r pointing away from s (b > 0)
        if (c > (float_type)0 && b > (float_type)0)
            return false;
        float_type discr = b * b - c;
        // A negative discriminant corresponds to ray missing sphere
        if (discr < (float_type)0)
            return false;
        // Ray now found to intersect sphere, compute smallest t value of intersection
        float_type t = -b - OP<float_type>::sqrt(discr);
        // If t is negative, ray started inside sphere so clamp t to zero
        if (t < (float_type)0)
            t = (float_type)0;
        // q = p + t * d;
        *outT = t;
        *outNormal = OP<vec3_type>::normalize((ray.origin + ray.dir * t) - center);
        return true;
    }

    // Intersects ray r = p + td, |d| = 1, with sphere s and, if intersecting,
    // returns t value of intersection and intersection point q
    template <typename T>
    inline bool Sphere<T, _CHKE_>::raycastSphere(const Ray<T> &ray, const Sphere<T> &sphere, float_type *outT, vec3_type *outNormal)
    {
        return Sphere<T>::raycastSphere(ray, sphere.center, sphere.radius, outT, outNormal);
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::segmentIntersectsSphere(const vec3_type &p, const vec3_type &q, const vec3_type &center, const float_type &radius)
    {
        using namespace MathCore;

        float_type t;
        vec3_type pq = q - p;
        float_type lengthPQ = OP<vec3_type>::dot(pq, pq);
        const float_type TOLERANCE = (float_type)0.001;
        // check if can normalize segment
        if (OP<float_type>::abs(lengthPQ) > TOLERANCE && OP<float_type>::abs(lengthPQ - (float_type)1) > TOLERANCE)
            pq *= (float_type)1 / lengthPQ;
        else
        {
            // test the point inside the sphere
            return pointInsideSphere(p, center, radius);
        }

        vec3_type n;
        if (raycastSphere(Ray<T>(p, pq), center, radius, &t, &n))
        {
            return (t < lengthPQ);
        }
        return false;
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::segmentIntersectsSphere(const LineSegment<T> &ls, const vec3_type &center, const float_type &radius)
    {
        return Sphere<T>::segmentIntersectsSphere(ls.a, ls.b, center, radius);
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::segmentIntersectsSphere(const vec3_type &p, const vec3_type &q, const Sphere<T> &sphere)
    {
        return Sphere<T>::segmentIntersectsSphere(p, q, sphere.center, sphere.radius);
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::segmentIntersectsSphere(const LineSegment<T> &ls, const Sphere<T> &sphere)
    {
        return Sphere<T>::segmentIntersectsSphere(ls.a, ls.b, sphere.center, sphere.radius);
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::pointInsideSphere(const vec3_type &p, const vec3_type &center, const float_type &radius)
    {
        using namespace MathCore;

        vec3_type p_sc = center - p;
        float_type sqrDst = OP<vec3_type>::dot(p_sc, p_sc);
        return sqrDst <= (radius * radius);
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::pointInsideSphere(const vec3_type &p, const Sphere<T> &sphere)
    {
        return Sphere<T>::pointInsideSphere(p, sphere.center, sphere.radius);
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::aabbOverlapsSphere(const AABB<T> &aabb, const vec3_type &center, const float_type &radius, vec3_type *penetration)
    {
        bool result = AABB<T>::sphereOverlapsAABB(center, radius, aabb, penetration);
        if (result)
            *penetration = -(*penetration);
        return result;
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::aabbOverlapsSphere(const AABB<T> &aabb, const Sphere<T> &sphere, vec3_type *penetration)
    {
        bool result = AABB<T>::sphereOverlapsAABB(sphere.center, sphere.radius, aabb, penetration);
        if (result)
            *penetration = -(*penetration);
        return result;
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::frustumOverlapsSphere(const Frustum<T> &f, const vec3_type &center, const float_type &radius)
    {
        return Frustum<T>::sphereOverlapsFrustum(center, radius, f);
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::frustumOverlapsSphere(const Frustum<T> &f, const Sphere<T> &s)
    {
        return Frustum<T>::sphereOverlapsFrustum(s.center, s.radius, f);
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::triangleIntersectsSphere(const vec3_type &a, const vec3_type &b, const vec3_type &c, const vec3_type &center, const float_type &radius, vec3_type *penetration)
    {
        bool result = Triangle<T>::sphereIntersectsTriangle(center, radius, a, b, c, penetration);
        if (result)
            *penetration = -(*penetration);
        return result;
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::triangleIntersectsSphere(const vec3_type &a, const vec3_type &b, const vec3_type &c, const Sphere<T> &sphere, vec3_type *penetration)
    {
        bool result = Triangle<T>::sphereIntersectsTriangle(sphere.center, sphere.radius, a, b, c, penetration);
        if (result)
            *penetration = -(*penetration);
        return result;
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::triangleIntersectsSphere(const Triangle<T> &t, const vec3_type &center, const float_type &radius, vec3_type *penetration)
    {
        bool result = Triangle<T>::sphereIntersectsTriangle(center, radius, t.a, t.b, t.c, penetration);
        if (result)
            *penetration = -(*penetration);
        return result;
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::triangleIntersectsSphere(const Triangle<T> &t, const Sphere<T> &sphere, vec3_type *penetration)
    {
        bool result = Triangle<T>::sphereIntersectsTriangle(sphere.center, sphere.radius, t.a, t.b, t.c, penetration);
        if (result)
            *penetration = -(*penetration);
        return result;
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::obbOverlapsSphere(const OBB<T> &obb, const vec3_type &center, const float_type &radius, vec3_type *penetration)
    {
        bool result = OBB<T>::sphereOverlapsOBB(center, radius, obb, penetration);
        if (result)
            *penetration = -(*penetration);
        return result;
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::obbOverlapsSphere(const OBB<T> &obb, const Sphere<T> &sphere, vec3_type *penetration)
    {
        bool result = OBB<T>::sphereOverlapsOBB(sphere.center, sphere.radius, obb, penetration);
        if (result)
            *penetration = -(*penetration);
        return result;
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::planeIntersectsSphere(const Plane<T> &plane, const vec3_type &center, const float_type &radius, vec3_type *penetration)
    {
        bool result = Plane<T>::sphereIntersectsPlane(center, radius, plane, penetration);
        if (result)
            *penetration = -(*penetration);
        return result;
    }

    template <typename T>
    inline bool Sphere<T, _CHKE_>::planeIntersectsSphere(const Plane<T> &plane, const Sphere<T> &sphere, vec3_type *penetration)
    {
        bool result = Plane<T>::sphereIntersectsPlane(sphere.center, sphere.radius, plane, penetration);
        if (result)
            *penetration = -(*penetration);
        return result;
    }

    template <typename T>
    inline Sphere<T> Sphere<T, _CHKE_>::from4Points(const vec3_type &a, const vec3_type &b, const vec3_type &c, const vec3_type &d)
    {
        using namespace MathCore;

        /*
        vec3_type a = _a;
        vec3_type b = _b;
        vec3_type c = _c;
        vec3_type d = _d;
        */

        // https://gamedev.stackexchange.com/questions/162731/welzl-algorithm-to-find-the-smallest-bounding-sphere
        // https://mathworld.wolfram.com/Circumsphere.html

        // Construct a matrix with the vectors as rows
        mat4_type matrix = OP<mat4_type>::transpose(mat4_type(
            CVT<vec3_type>::toPtn4(a),
            CVT<vec3_type>::toPtn4(b),
            CVT<vec3_type>::toPtn4(c),
            CVT<vec3_type>::toPtn4(d)));
        float_type det_a = OP<mat4_type>::determinant(matrix);

        /*
         vec3_type offset = vec3_type(0);
        if (absv(det_a) <= EPSILON)
        {
            offset = vec3_type(10,10,10);

            a += offset;
            b += offset;
            c += offset;
            d += offset;

            matrix = transpose(mat4(toPtn4(a), toPtn4(b), toPtn4(c), toPtn4(d)));
            det_a = mat4_determinant(matrix);
        }
        */

        // ARIBEIRO_ABORT(det_a == 0, "All 4 points are coplanar.\n");

        // Copy the matrix so we can modify it
        // and still read rows from the original.
        mat4_type D = matrix;
        vec3_type center;

        D.a1 = OP<vec4_type>::sqrLength(a);
        D.a2 = OP<vec4_type>::sqrLength(b);
        D.a3 = OP<vec4_type>::sqrLength(c);
        D.a4 = OP<vec4_type>::sqrLength(d);

        center.x = OP<mat4_type>::determinant(D);

        D[1] = matrix[0];
        center.y = -OP<mat4_type>::determinant(D);

        D[2] = matrix[1];
        center.z = OP<mat4_type>::determinant(D);

        center *= (float_type)1 / ((float_type)2 * det_a);

        float_type radius = OP<vec3_type>::distance(a, center);

        /*

        float_type radius_a = distance(a, center);
        float_type radius_b = distance(b, center);
        float_type radius_c = distance(c, center);
        float_type radius_d = distance(d, center);

        float_type radius = maximum(maximum(radius_a,radius_b),maximum(radius_c,radius_d));


        printf("radius: %e\n", radius);
        printf("dst from a to center: %e\n", distance(a, center));
        printf("dst from b to center: %e\n", distance(b, center));
        printf("dst from c to center: %e\n", distance(c, center));
        printf("dst from d to center: %e\n", distance(d, center));
        */

        // center -= offset;

        return Sphere<T>(center, radius);
    }

    template <typename T>
    inline Sphere<T> Sphere<T, _CHKE_>::fromFrustum(const Frustum<T> &frustum)
    {
        Sphere<T> result = Sphere<T>::from4Points(
            frustum.vertices[FrustumVertex::Near_Right_Bottom],
            frustum.vertices[FrustumVertex::Near_Left_Top],
            frustum.vertices[FrustumVertex::Far_Left_Top],
            frustum.vertices[FrustumVertex::Far_Left_Bottom]);

        return result;
    }

    template <typename T>
    inline Sphere<T> Sphere<T, _CHKE_>::fromAABB(const AABB<T> &aabb, bool discard_z)
    {
        using namespace MathCore;

        if (discard_z)
        {
            vec3_type min = vec3_type(aabb.min_box.x, aabb.min_box.y, 0);
            vec3_type max = vec3_type(aabb.max_box.x, aabb.max_box.y, 0);
            vec3_type center = (min + max) * (float_type)0.5;
            float_type radius = OP<vec3_type>::distance(center, max);
            return Sphere<T>(center, radius);
        }
        else
        {
            vec3_type center = (aabb.min_box + aabb.max_box) * (float_type)0.5;
            float_type radius = OP<vec3_type>::distance(center, aabb.max_box);
            return Sphere<T>(center, radius);
        }
    }

    template <typename T>
    inline Sphere<T> Sphere<T, _CHKE_>::fromLineSegment(const LineSegment<T> &ls)
    {
        using namespace MathCore;

        vec3_type center = (ls.a + ls.b) * (float_type)0.5;
        float_type radius = OP<vec3_type>::distance(center, ls.a);
        return Sphere(center, radius);
    }

    template <typename T>
    inline Sphere<T> Sphere<T, _CHKE_>::fromTriangle(const Triangle<T> &triangle)
    {
        using namespace MathCore;
        //
        // Not tested...
        //
        vec3_type ac = triangle.a - triangle.c;
        vec3_type bc = triangle.b - triangle.c;

        vec3_type _cross = OP<vec3_type>::cross(ac, bc);

        vec3_type center = triangle.c +
                           OP<vec3_type>::cross(OP<vec3_type>::sqrLength(ac) * bc - OP<vec3_type>::sqrLength(bc) * ac, _cross) * ((float_type)1 / ((float_type)2 * OP<vec3_type>::sqrLength(_cross)));

        float_type radius = OP<vec3_type>::distance(triangle.a, center);

        return Sphere<T>(center, radius);
    }

    template <typename T>
    inline Sphere<T> Sphere<T, _CHKE_>::fromOBB(const OBB<T> &obb, bool discard_z)
    {
        using namespace MathCore;
        if (discard_z)
        {
            vec3_type center = vec3_type(obb.center.x, obb.center.y, 0);
            float_type radius = OP<vec3_type>::length(vec3_type(obb.dimension_2.x, obb.dimension_2.y, 0));
            return Sphere<T>(center, radius);
        }
        else
        {
            vec3_type center = obb.center;
            float_type radius = OP<vec3_type>::length(obb.dimension_2);
            return Sphere<T>(center, radius);
        }
    }

}
