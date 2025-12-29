#include "Plane.h"

namespace CollisionCore
{

    template <typename T>
    inline Plane<T, _CHKE_>::Plane()
    {
        using namespace MathCore;
        // using vec4_type = typename Plane<T>::vec4_type;
        // using vec3_type = typename Plane<T>::vec3_type;
        // using float_type = typename Plane<T>::float_type;
        // using quat_type = typename Plane<T>::quat_type;

        normal.y = (float_type)1;
        distance = (float_type)0;
    }

    template <typename T>
    inline Plane<T, _CHKE_>::Plane(const vec3_type &point, const vec3_type &normal)
    {
        using namespace MathCore;
        // using vec4_type = typename Plane<T>::vec4_type;
        // using vec3_type = typename Plane<T>::vec3_type;
        // using float_type = typename Plane<T>::float_type;
        // using quat_type = typename Plane<T>::quat_type;

        this->normal = OP<vec3_type>::normalize(normal);
        distance = OP<vec3_type>::dot(normal, point);
    }

    template <typename T>
    inline Plane<T, _CHKE_>::Plane(const vec3_type &a, const vec3_type &b, const vec3_type &c)
    {
        using namespace MathCore;
        // using vec4_type = typename Plane<T>::vec4_type;
        // using vec3_type = typename Plane<T>::vec3_type;
        // using float_type = typename Plane<T>::float_type;
        // using quat_type = typename Plane<T>::quat_type;

        normal = OP<vec3_type>::normalize(OP<vec3_type>::cross(b - a, c - a));
        distance = OP<vec3_type>::dot(normal, a);
    }

    template <typename T>
    inline void Plane<T, _CHKE_>::normalize()
    {
        using namespace MathCore;
        // using vec4_type = typename Plane<T>::vec4_type;
        // using vec3_type = typename Plane<T>::vec3_type;
        // using float_type = typename Plane<T>::float_type;
        // using quat_type = typename Plane<T>::quat_type;

        float_type mag2 = OP<vec3_type>::dot(normal, normal);
        float_type mag2_rsqrt = OP<float_type>::rsqrt(mag2);
        normal *= mag2_rsqrt;
        distance *= mag2_rsqrt;
    }

    template <typename T>
    inline Plane<T> Plane<T, _CHKE_>::fromTriangle(const vec3_type &a, const vec3_type &b, const vec3_type &c)
    {
        return Plane<T>(a, b, c);
    }

    template <typename T>
    inline Plane<T> Plane<T, _CHKE_>::fromTriangle(const Triangle<T> &t)
    {
        return fromTriangle(t.a, t.b, t.c);
    }

    /*
   Quando se aplica os valores: x,y e z na equacao de um plano,
     pode-se obter um numero positivo, negativo ou zero.
   Quando zero, significa que o ponto (x,y,z) esta dentro do plano
   Quando negativo ou positivo, o numero e' a distancia que este ponto
     esta do plano em relacao a sua normal, que em seu sentido positivo,
     deixa o numero positivo, e negativo o mesmo.
*/
    template <typename T>
    inline
        typename Plane<T, _CHKE_>::float_type
        Plane<T, _CHKE_>::pointDistanceToPlane(const vec3_type &q, const Plane<T> &plane)
    {
        using namespace MathCore;

        float_type t = (OP<vec3_type>::dot(plane.normal, q) - plane.distance); // / dot(plane.normal, plane.normal);
        return t;
    }

    template <typename T>
    inline
        typename Plane<T, _CHKE_>::vec3_type
        Plane<T, _CHKE_>::closestPointToPlane(const vec3_type &q, const Plane<T> &plane)
    {
        using namespace MathCore;

        float_type t = (OP<vec3_type>::dot(plane.normal, q) - plane.distance); // / dot(plane.normal, plane.normal);
        // If the plane equation is known to be normalized
        // float_type t = (dot(p_norm, q) - p_dist);
        return q - t * plane.normal;
    }

    template <typename T>
    inline bool Plane<T, _CHKE_>::raycastPlane(const Ray<T> &ray, const Plane<T> &plane, float_type *outT, vec3_type *outNormal)
    {
        using namespace MathCore;

        float_type aux_vd, v0, t;
        aux_vd = OP<vec3_type>::dot(plane.normal, ray.dir);
        if (aux_vd >= -EPSILON<float_type>::high_precision)
            return false; // paralelo na dire��o do raio ou na dire��o contr�ria do raio
        // if ray nearly parallel to plane, no intersection
        // if (absv(aux_vd) < 1e-6f)
        // return false;
        v0 = plane.distance - OP<vec3_type>::dot(plane.normal, ray.origin);
        // if (absv(v0) < EPSILON)
        if (v0 < EPSILON<float_type>::high_precision)
            return false; // surface pointing wrong way
        t = v0 / aux_vd;

        *outT = t;
        *outNormal = plane.normal;
        return true;
    }

    template <typename T>
    inline bool Plane<T, _CHKE_>::segmentIntersectsPlane(const vec3_type &a, const vec3_type &b, const Plane<T> &plane)
    {
        using namespace MathCore;

        // Compute the t value for the directed line ab intersecting the plane
        vec3_type ab = b - a;
        float_type t = (plane.distance - OP<vec3_type>::dot(plane.normal, a)) / OP<vec3_type>::dot(plane.normal, ab);
        // If t in [0..1] compute and return intersection point
        if (t >= (float_type)0 && t <= (float_type)1)
        {
            //*outT = t;
            // q = a + t * ab;
            return true;
        }
        // Else no intersection
        return false;
    }

    template <typename T>
    inline bool Plane<T, _CHKE_>::segmentIntersectsPlane(const LineSegment<T> &ls, const Plane<T> &plane)
    {
        return segmentIntersectsPlane(ls.a, ls.b, plane);
    }

    // Given planes p1 and p2, compute line L = p+t*d of their intersection.
    // Return 0 if no such line exists
    template <typename T>
    inline bool Plane<T, _CHKE_>::intersectPlaneToPlane(const Plane<T> &p1, const Plane<T> &p2, vec3_type *outP, vec3_type *outD)
    {
        using namespace MathCore;

        // Compute direction of intersection line
        vec3_type d = OP<vec3_type>::cross(p1.normal, p2.normal);
        // If d is (near) zero, the planes are parallel (and separated)
        // or coincident, so they�re not considered intersecting
        float_type denom = OP<vec3_type>::dot(d, d);
        if (denom < EPSILON<float_type>::high_precision)
            return false;
        // Compute point on intersection line
        denom = (float_type)1 / denom;
        *outP = OP<vec3_type>::cross(p1.distance * p2.normal - p2.distance * p1.normal, d) * denom;
        *outD = d;
        return true;
    }

    // Compute the point p at which the three planes p1, p2 and p3 intersect (if at all)
    template <typename T>
    inline bool Plane<T, _CHKE_>::intersectPlanes(const Plane<T> &p1, const Plane<T> &p2, const Plane<T> &p3, vec3_type *outP)
    {
        using namespace MathCore;
        // using vec4_type = typename Plane<T>::vec4_type;
        // using vec3_type = typename Plane<T>::vec3_type;
        // using float_type = typename Plane<T>::float_type;
        // using quat_type = typename Plane<T>::quat_type;

        vec3_type u = OP<vec3_type>::cross(p2.normal, p3.normal);
        float_type denom = OP<vec3_type>::dot(p1.normal, u);
        if (OP<float_type>::abs(denom) < EPSILON<float_type>::high_precision)
            return false; // Planes do not intersect in a point
        denom = (float_type)1 / denom;
        *outP = (p1.distance * u + OP<vec3_type>::cross(p1.normal, p3.distance * p2.normal - p2.distance * p3.normal)) * denom;
        return true;
    }

    template <typename T>
    inline bool Plane<T, _CHKE_>::aabbIntersectsPlane(const AABB<T> &b, const Plane<T> &plane)
    {
        return AABB<T>::planeIntersectsAABB(plane, b);
    }

    template <typename T>
    inline bool Plane<T, _CHKE_>::obbIntersectsPlane(const OBB<T> &obb, const Plane<T> &plane)
    {
        return OBB<T>::planeIntersectsOBB(plane, obb);
    }

    template <typename T>
    inline bool Plane<T, _CHKE_>::sphereIntersectsPlane(const vec3_type &center, const float_type &radius, const Plane<T> &plane, vec3_type *penetration)
    {
        using namespace MathCore;

        float distance_to_plane = pointDistanceToPlane(center, plane);
        float sphere_dst_to_plane_abs = MathCore::OP<float>::abs(distance_to_plane - radius);

        if (sphere_dst_to_plane_abs > (float_type)0.004 && distance_to_plane < radius) // (float_type)0.00002
        {
            // EPSILON - to avoid process the same triangle again...
            const float_type EPSILON = (float_type)0.002;
            *penetration = -plane.normal * (sphere_dst_to_plane_abs + EPSILON);
            return true;
        }

        // vec3_type closestPointInPlane = closestPointToPlane(center, plane);

        // vec3_type SphereToPlane = closestPointInPlane - center;
        // float_type sqrLength_SphereToPlane = OP<vec3_type>::dot(SphereToPlane, SphereToPlane);

        // float_type Max_Radius_sqr = radius * radius;

        // if (sqrLength_SphereToPlane > (float_type)0.00002 && sqrLength_SphereToPlane < Max_Radius_sqr)
        // {

        //     float_type Length_SphereToPlane = OP<float_type>::sqrt(sqrLength_SphereToPlane);
        //     vec3_type SphereToPlaneNorm = SphereToPlane * ((float_type)1 / Length_SphereToPlane); // normalize(SphereToPlane);
        //     // Vector3 triangleNormal = Vectormath::Aos::normalize( Vectormath::Aos::cross( p2-p1 , p3-p1 ) );

        //     // EPSILON - to avoid process the same triangle again...
        //     const float_type EPSILON = (float_type)0.002;
        //     *penetration = SphereToPlaneNorm * (radius - Length_SphereToPlane + EPSILON);
        //     return true;
        // }
        return false;
    }

    template <typename T>
    inline bool Plane<T, _CHKE_>::sphereIntersectsPlane(const Sphere<T> &sphere, const Plane<T> &plane, vec3_type *penetration)
    {
        return sphereIntersectsPlane(sphere.center, sphere.radius, plane, penetration);
    }

}
