
#include "Triangle.h"

namespace CollisionCore
{

    template <typename T>
    inline Triangle<T, _CHKE_>::Triangle()
    {
    }

    template <typename T>
    inline Triangle<T, _CHKE_>::Triangle(const vec3_type &a, const vec3_type &b, const vec3_type &c)
    {
        using namespace MathCore;
        // using vec4_type = typename Triangle<T>::vec4_type;
        // using vec3_type = typename Triangle<T>::vec3_type;
        // using float_type = typename Triangle<T>::float_type;
        // using quat_type = typename Triangle<T>::quat_type;

        this->a = a;
        this->b = b;
        this->c = c;
    }

    // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
    template <typename T> inline
    bool Triangle<T, _CHKE_>::raycastTriangle(const Ray<T> &ray, const vec3_type &vertex0, const vec3_type &vertex1, const vec3_type &vertex2, float_type *outT, vec3_type *outNormal)
    {
        using namespace MathCore;

        const float_type EPSILON = (float_type)1e-6; // 0.0000001;
        vec3_type edge1, edge2, h, s, q;
        float_type a, f, u, v;

        edge1 = vertex1 - vertex0;
        edge2 = vertex2 - vertex0;

        h = OP<vec3_type>::cross(ray.dir, edge2);
        a = OP<vec3_type>::dot(edge1, h);
        if (a > -EPSILON && a < EPSILON)
            return false; // This ray is parallel to this triangle.
        f = (float_type)1 / a;
        s = ray.origin - vertex0;
        u = f * OP<vec3_type>::dot(s, h);
        if (u < 0.0 || u > 1.0)
            return false;
        q = OP<vec3_type>::cross(s, edge1);
        v = f * OP<vec3_type>::dot(ray.dir, q);
        if (v < (float_type)0 || u + v > (float_type)1)
            return false;
        // At this stage we can compute t to find out where the intersection point is on the line.
        float_type t = f * OP<vec3_type>::dot(edge2, q);
        if (t > EPSILON) // ray intersection
        {
            *outT = t;
            *outNormal = OP<vec3_type>::normalize(OP<vec3_type>::cross(edge1, edge2));
            // outIntersectionPoint = r_origin + r_dir * t;
            return true;
        }
        else // This means that there is a line intersection but not a ray intersection.
            return false;
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::raycastTriangle(const Ray<T> &ray, const Triangle<T> &t, float_type *outT, vec3_type *outNormal)
    {
        return raycastTriangle(ray, t.a, t.b, t.c, outT, outNormal);
    }

template <typename T> inline
    typename Triangle<T, _CHKE_>::vec3_type Triangle<T, _CHKE_>::closestPointToTriangle(const vec3_type &p, const vec3_type &a, const vec3_type &b, const vec3_type &c)
    {
        using namespace MathCore;

        // Check if P in vertex region outside A
        vec3_type ab = b - a;
        vec3_type ac = c - a;
        vec3_type ap = p - a;
        float_type d1 = OP<vec3_type>::dot(ab, ap);
        float_type d2 = OP<vec3_type>::dot(ac, ap);
        if (d1 <= (float_type)0 && d2 <= (float_type)0)
            return a; // barycentric coordinates (1,0,0)

        // Check if P in vertex region outside B
        vec3_type bp = p - b;
        float_type d3 = OP<vec3_type>::dot(ab, bp);
        float_type d4 = OP<vec3_type>::dot(ac, bp);
        if (d3 >= (float_type)0 && d4 <= d3)
            return b; // barycentric coordinates (0,1,0)

        // Check if P in edge region of AB, if so return projection of P onto AB
        float_type vc = d1 * d4 - d3 * d2;
        if (vc <= (float_type)0 && d1 >= (float_type)0 && d3 <= (float_type)0)
        {
            float_type v = d1 / (d1 - d3);
            return a + v * ab; // barycentric coordinates (1-v,v,0)
        }

        // Check if P in vertex region outside C
        vec3_type cp = p - c;
        float_type d5 = OP<vec3_type>::dot(ab, cp);
        float_type d6 = OP<vec3_type>::dot(ac, cp);
        if (d6 >= (float_type)0 && d5 <= d6)
            return c; // barycentric coordinates (0,0,1)

        // Check if P in edge region of AC, if so return projection of P onto AC
        float_type vb = d5 * d2 - d1 * d6;
        if (vb <= (float_type)0 && d2 >= (float_type)0 && d6 <= (float_type)0)
        {
            float_type w = d2 / (d2 - d6);
            return a + w * ac; // barycentric coordinates (1-w,0,w)
        }

        // Check if P in edge region of BC, if so return projection of P onto BC
        float_type va = d3 * d6 - d5 * d4;
        if (va <= (float_type)0 && (d4 - d3) >= (float_type)0 && (d5 - d6) >= (float_type)0)
        {
            float_type w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
            return b + w * (c - b); // barycentric coordinates (0,1-w,w)
        }

        // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
        float_type denom = (float_type)1 / (va + vb + vc);
        float_type v = vb * denom;
        float_type w = vc * denom;
        return a + ab * v + ac * w; //=u*a+v*b+w*c,u=va*denom=1.0f-v-w
    }

template <typename T> inline
    typename Triangle<T, _CHKE_>::vec3_type Triangle<T, _CHKE_>::closestPointToTriangle(const vec3_type &p, const Triangle<T> &t)
    {
        return closestPointToTriangle(p, t.a, t.b, t.c);
    }

    // Given line pq and ccw triangle abc, return whether line pierces triangle. If
    // so, also return the barycentric coordinates (u,v,w) of the intersection point
    template <typename T> inline
    bool Triangle<T, _CHKE_>::segmentIntersectsTriangle(const vec3_type &p, const vec3_type &q,
                                             const vec3_type &a, const vec3_type &b, const vec3_type &c,
                                             float_type *_u, float_type *_v, float_type *_w)
    {
        using namespace MathCore;

        float_type u, v, w;
        vec3_type pq = q - p;
        vec3_type pa = a - p;
        vec3_type pb = b - p;
        vec3_type pc = c - p;
        // Test if pq is inside the edges bc, ca and ab. Done by testing
        // that the signed tetrahedral volumes, computed using scalar triple
        // products, are all positive

        vec3_type m = OP<vec3_type>::cross(pq, pc);
        u = OP<vec3_type>::dot(pb, m); // ScalarTriple(pq, pc, pb);
        if (u < (float_type)0)
            return false;
        v = -OP<vec3_type>::dot(pa, m); // ScalarTriple(pq, pa, pc);
        if (v < (float_type)0)
            return false;
        m = OP<vec3_type>::cross(pq, pb);
        w = -OP<vec3_type>::dot(pa, m); // ScalarTriple(pq, pb, pa);
        if (w < (float_type)0)
            return false;

        // Compute the barycentric coordinates (u, v, w) determining the
        // intersection point r, r = u*a + v*b + w*c
        float_type denom = (float_type)1 / (u + v + w);
        u *= denom;
        v *= denom;
        w *= denom; // w = 1.0f - u - v;

        *_u = u;
        *_v = v;
        *_w = w;
        return true;
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::segmentIntersectsTriangle(const LineSegment<T> &ls,
                                             const vec3_type &a, const vec3_type &b, const vec3_type &c,
                                             float_type *u, float_type *v, float_type *w)
    {
        return segmentIntersectsTriangle(ls.a, ls.b,
                                         a, b, c,
                                         u, v, w);
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::segmentIntersectsTriangle(const LineSegment<T> &ls,
                                             const Triangle<T> &t,
                                             float_type *u, float_type *v, float_type *w)
    {
        return segmentIntersectsTriangle(ls.a, ls.b,
                                         t.a, t.b, t.c,
                                         u, v, w);
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::segmentIntersectsTriangle(const vec3_type &p, const vec3_type &q,
                                             const Triangle<T> &t,
                                             float_type *u, float_type *v, float_type *w)
    {
        return segmentIntersectsTriangle(p, q,
                                         t.a, t.b, t.c,
                                         u, v, w);
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::sphereIntersectsTriangle(const vec3_type &center, const float_type &radius, const vec3_type &a, const vec3_type &b, const vec3_type &c, vec3_type *penetration)
    {
        using namespace MathCore;

        vec3_type closestPointInTriangle = closestPointToTriangle(center, a, b, c);

        vec3_type SphereToTriangle = closestPointInTriangle - center;
        float_type sqrLength_SphereToTriangle = OP<vec3_type>::dot(SphereToTriangle, SphereToTriangle);

        float_type Max_Radius_sqr = radius * radius;

        if (sqrLength_SphereToTriangle > (float_type)0.00002 && sqrLength_SphereToTriangle < Max_Radius_sqr)
        {

            float_type Length_SphereToTriangle = OP<float_type>::sqrt(sqrLength_SphereToTriangle);
            vec3_type SphereToTriangleNorm = SphereToTriangle * ((float_type)1 / Length_SphereToTriangle); // normalize(SphereToTriangle);
            // Vector3 triangleNormal = Vectormath::Aos::normalize( Vectormath::Aos::cross( p2-p1 , p3-p1 ) );

            // EPSILON - to avoid process the same triangle again...
            const float_type EPSILON = (float_type)0.002;
            *penetration = SphereToTriangleNorm * (radius - Length_SphereToTriangle + EPSILON);
            return true;
        }
        return false;
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::sphereIntersectsTriangle(
        const Sphere<T> &sphere,
        const vec3_type &a, const vec3_type &b, const vec3_type &c,
        vec3_type *penetration)
    {
        return sphereIntersectsTriangle(sphere.center, sphere.radius, a, b, c, penetration);
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::sphereIntersectsTriangle(const vec3_type &center, const float_type &radius, const Triangle<T> &t, vec3_type *penetration)
    {
        return sphereIntersectsTriangle(center, radius, t.a, t.b, t.c, penetration);
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::sphereIntersectsTriangle(const Sphere<T> &sphere, const Triangle<T> &t, vec3_type *penetration)
    {
        return sphereIntersectsTriangle(sphere.center, sphere.radius, t.a, t.b, t.c, penetration);
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::aabbIntersectsTriangle(const AABB<T> &box, const vec3_type &v0, const vec3_type &v1, const vec3_type &v2)
    {
        return AABB<T>::triangleIntersectsAABB(v0, v1, v2, box);
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::aabbIntersectsTriangle(const AABB<T> &box, const Triangle<T> &triangle)
    {
        return AABB<T>::triangleIntersectsAABB(triangle.a, triangle.b, triangle.c, box);
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::obbIntersectsTriangle(const OBB<T> &obb, const vec3_type &v0, const vec3_type &v1, const vec3_type &v2)
    {
        return OBB<T>::triangleIntersectsOBB(v0, v1, v2, obb);
    }

template <typename T> inline
    bool Triangle<T, _CHKE_>::obbIntersectsTriangle(const OBB<T> &obb, const Triangle<T> &triangle)
    {
        return OBB<T>::triangleIntersectsOBB(triangle.a, triangle.b, triangle.c, obb);
    }

}
