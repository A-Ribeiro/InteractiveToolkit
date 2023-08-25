
#include "LineSegment.h"

namespace CollisionCore
{

    template <typename T> inline
    LineSegment<T, _CHKE_>::LineSegment()
    {
    }

    template <typename T> inline
    LineSegment<T, _CHKE_>::LineSegment(const vec3_type &a, const vec3_type &b)
    {
        this->a = a;
        this->b = b;
    }

    // Given segment ab and point c, computes closest point d on ab.
    // Also returns t for the position of d, d(t)=a+ t*(b - a)
    template <typename T> inline
    typename LineSegment<T, _CHKE_>::vec3_type LineSegment<T, _CHKE_>::closestPointToSegment(const vec3_type &p, const vec3_type &a, const vec3_type &b)
    {
        using namespace MathCore;

        vec3_type ab = b - a;
        // Project p onto ab, computing parameterized position d(t)=a+ t*(b � a)
        float_type t = OP<vec3_type>::dot(p - a, ab) / OP<vec3_type>::dot(ab, ab);
        // If outside segment, clamp t (and therefore d) to the closest endpoint
        if (t < (float_type)0)
            t = (float_type)0;
        if (t > (float_type)1)
            t = (float_type)1;
        // Compute projected position from the clamped t
        return a + t * ab;
    }

    template <typename T> inline
    typename LineSegment<T, _CHKE_>::vec3_type LineSegment<T, _CHKE_>::closestPointToSegment(const vec3_type &p, const LineSegment<T> &ls)
    {
        return closestPointToSegment(p, ls.a, ls.b);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::aabbIntersectsSegment(const AABB<T> &aabb, const vec3_type &p0, const vec3_type &p1)
    {
        return AABB<T>::segmentIntersectsAABB(p0, p1, aabb);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::aabbIntersectsSegment(const AABB<T> &aabb, const LineSegment<T> &ls)
    {
        return AABB<T>::segmentIntersectsAABB(ls.a, ls.b, aabb);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::planeIntersectsSegment(const Plane<T> &plane, const vec3_type &a, const vec3_type &b)
    {
        return Plane<T>::segmentIntersectsPlane(a, b, plane);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::planeIntersectsSegment(const Plane<T> &plane, const LineSegment<T> &ls)
    {
        return Plane<T>::segmentIntersectsPlane(ls.a, ls.b, plane);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::sphereIntersectsSegment(const vec3_type &center, const float_type &radius, const vec3_type &a, const vec3_type &b)
    {
        return Sphere<T>::segmentIntersectsSphere(a, b, center, radius);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::sphereIntersectsSegment(const vec3_type &center, const float_type &radius, const LineSegment<T> &ls)
    {
        return Sphere<T>::segmentIntersectsSphere(ls.a, ls.b, center, radius);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::sphereIntersectsSegment(const Sphere<T> &sphere, const vec3_type &p, const vec3_type &q)
    {
        return Sphere<T>::segmentIntersectsSphere(p, q, sphere.center, sphere.radius);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::sphereIntersectsSegment(const Sphere<T> &sphere, const LineSegment<T> &ls)
    {
        return Sphere<T>::segmentIntersectsSphere(ls.a, ls.b, sphere.center, sphere.radius);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::triangleIntersectsSegment(
        const vec3_type &a, const vec3_type &b, const vec3_type &c,
        const vec3_type &p, const vec3_type &q,
        float_type *u, float_type *v, float_type *w)
    {
        return Triangle<T>::segmentIntersectsTriangle(p, q, a, b, c, u, v, w);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::triangleIntersectsSegment(
        const vec3_type &a, const vec3_type &b, const vec3_type &c,
        const LineSegment<T> &ls,
        float_type *u, float_type *v, float_type *w)
    {
        return Triangle<T>::segmentIntersectsTriangle(ls.a, ls.b, a, b, c, u, v, w);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::triangleIntersectsSegment(
        const Triangle<T> &t,
        const LineSegment<T> &ls,
        float_type *u, float_type *v, float_type *w)
    {
        return Triangle<T>::segmentIntersectsTriangle(ls.a, ls.b, t.a, t.b, t.c, u, v, w);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::triangleIntersectsSegment(
        const Triangle<T> &t,
        const vec3_type &p, const vec3_type &q,
        float_type *u, float_type *v, float_type *w)
    {
        return Triangle<T>::segmentIntersectsTriangle(p, q, t.a, t.b, t.c, u, v, w);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::obbIntersectsSegment(const OBB<T> &obb, const vec3_type &p0, const vec3_type &p1)
    {
        return OBB<T>::segmentIntersectsOBB(p0, p1, obb);
    }

    template <typename T> inline
    bool LineSegment<T, _CHKE_>::obbIntersectsSegment(const OBB<T> &obb, const LineSegment<T> &ls)
    {
        return OBB<T>::segmentIntersectsOBB(ls.a, ls.b, obb);
    }

}
