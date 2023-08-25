#include "OBB.h"

namespace CollisionCore
{

    template <typename T> inline
    Ray<T, _CHKE_>::Ray()
    {
    }

    template <typename T> inline
    Ray<T, _CHKE_>::Ray(const vec3_type &origin, const vec3_type &dir)
    {
        using namespace MathCore;
        // using vec4_type = typename Ray<T>::vec4_type;
        // using vec3_type = typename Ray<T>::vec3_type;
        // using float_type = typename Ray<T>::float_type;
        // using quat_type = typename Ray<T>::quat_type;

        this->origin = origin;
        this->dir = dir;
    }

    template <typename T> inline
    bool Ray<T, _CHKE_>::raycastAABB(const Ray<T> &r, const AABB<T> &a, float_type *outTmin, vec3_type *outNormal)
    {
        return AABB<T>::raycastAABB(r, a, outTmin, outNormal);
    }

    template <typename T> inline
    bool Ray<T, _CHKE_>::raycastPlane(const Ray<T> &r, const Plane<T> &plane, float_type *outT, vec3_type *outNormal)
    {
        return Plane<T>::raycastPlane(r, plane, outT, outNormal);
    }

    template <typename T> inline
    bool Ray<T, _CHKE_>::raycastSphere(const Ray<T> &r, const vec3_type &center, const float_type &radius, float_type *outT, vec3_type *outNormal)
    {
        return Sphere<T>::raycastSphere(r, center, radius, outT, outNormal);
    }

    template <typename T> inline
    bool Ray<T, _CHKE_>::raycastSphere(const Ray<T> &r, const Sphere<T> &sphere, float_type *outT, vec3_type *outNormal)
    {
        return Sphere<T>::raycastSphere(r, sphere.center, sphere.radius, outT, outNormal);
    }

    template <typename T> inline
    bool Ray<T, _CHKE_>::raycastTriangle(const Ray<T> &ray, const vec3_type &a, const vec3_type &b, const vec3_type &c, float_type *outT, vec3_type *outNormal)
    {
        return Triangle<T>::raycastTriangle(ray, a, b, c, outT, outNormal);
    }

    template <typename T> inline
    bool Ray<T, _CHKE_>::raycastTriangle(const Ray<T> &ray, const Triangle<T> &t, float_type *outT, vec3_type *outNormal)
    {
        return Triangle<T>::raycastTriangle(ray, t.a, t.b, t.c, outT, outNormal);
    }

    template <typename T> inline
    bool Ray<T, _CHKE_>::raycastOBB(const Ray<T> &r, const OBB<T> &a, float_type *outTmin, vec3_type *outNormal)
    {
        return OBB<T>::raycastOBB(r, a, outTmin, outNormal);
    }

}
