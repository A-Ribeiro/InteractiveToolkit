#include "AABB.h"
#include "Triangle.h"
#include "Sphere.h"
#include "LineSegment.h"
#include "Frustum.h"

namespace CollisionCore
{
    //--------------------------------------------------------------------------
    template <typename T>
    inline AABB<T, _CHKE_>::AABB(const vec3_type &a, const vec3_type &b)
    {
        using namespace MathCore;
        // using vec4_type = typename AABB<T>::vec4_type;
        // using vec2_type = typename AABB<T>::vec2_type;
        // using vec3_type = typename AABB<T>::vec3_type;
        // using float_type = typename AABB<T>::float_type;
        // using quat_type = typename AABB<T>::quat_type;

#if defined(ITK_SSE2) || defined(ITK_NEON)
        min_box = OP<vec3_type>::minimum(a, b);
        max_box = OP<vec3_type>::maximum(a, b);
#else
        for (size_t i = 0; i < min_box.array_count; i++)
        {
            if (a[i] < b[i])
            {
                min_box[i] = a[i];
                max_box[i] = b[i];
            }
            else
            {
                max_box[i] = a[i];
                min_box[i] = b[i];
            }
        }
#endif
    }
    //--------------------------------------------------------------------------
    template <typename T>
    inline AABB<T, _CHKE_>::AABB(const vec2_type &a, const vec2_type &b)
    {
        using namespace MathCore;
        // using vec4_type = typename AABB<T>::vec4_type;
        // using vec2_type = typename AABB<T>::vec2_type;
        // using vec3_type = typename AABB<T>::vec3_type;
        // using float_type = typename AABB<T>::float_type;
        // using quat_type = typename AABB<T>::quat_type;

#if defined(ITK_SSE2) || defined(ITK_NEON)
        min_box = vec3_type(OP<vec2_type>::minimum(a, b), -1);
        max_box = vec3_type(OP<vec2_type>::maximum(a, b), 1);
#else
        for (size_t i = 0; i < a.array_count; i++)
        {
            if (a[i] < b[i])
            {
                min_box[i] = a[i];
                max_box[i] = b[i];
            }
            else
            {
                max_box[i] = a[i];
                min_box[i] = b[i];
            }
        }
        min_box.z = -1;
        max_box.z = 1;
#endif
    }
    //--------------------------------------------------------------------------
    template <typename T>
    inline AABB<T, _CHKE_>::AABB()
    {
        min_box = T( MathCore::FloatTypeInfo<float_type>::max );
        max_box = T( -MathCore::FloatTypeInfo<float_type>::max );
    }

    template <typename T>
    inline void AABB<T, _CHKE_>::makeEmpty() {
        min_box = T( MathCore::FloatTypeInfo<float_type>::max );
        max_box = T( -MathCore::FloatTypeInfo<float_type>::max );
    }

    template <typename T>
    inline bool AABB<T, _CHKE_>::isEmpty() const {
        return (min_box.x > max_box.x || min_box.y > max_box.y || min_box.z > max_box.z);
    }
    //--------------------------------------------------------------------------
    /*
    void AABB::glDrawLines(const AABB &aabb){
        #ifdef __gl_h_
        glBegin(GL_LINES);
          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);
          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);
          glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);

          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);
          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);

          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);
          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);
          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);
        glEnd();
        #endif
    }
    void AABB::glDrawQuads(const AABB &aabb){
        #ifdef __gl_h_
        glBegin(GL_QUADS);
          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);
          glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);

          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);

          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);
          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);

          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);

          glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.min_box.z);glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.min_box.z);
          glVertex3f(aabb.max_box.x,aabb.max_box.y,aabb.max_box.z);glVertex3f(aabb.max_box.x,aabb.min_box.y,aabb.max_box.z);

          glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.max_box.z);glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.max_box.z);
          glVertex3f(aabb.min_box.x,aabb.max_box.y,aabb.min_box.z);glVertex3f(aabb.min_box.x,aabb.min_box.y,aabb.min_box.z);
        glEnd();
        #endif
    }
    */
    //--------------------------------------------------------------------------
    template <typename T>
    inline bool AABB<T, _CHKE_>::pointInsideAABB(const vec2_type &ptn, const AABB<T> &b)
    {
        return (ptn.x < b.max_box.x && ptn.x > b.min_box.x &&
                ptn.y < b.max_box.y && ptn.y > b.min_box.y);
    }

    template <typename T>
    inline bool AABB<T, _CHKE_>::pointInsideAABB(const vec3_type &ptn, const AABB<T> &b)
    {
        return (ptn.x < b.max_box.x && ptn.x > b.min_box.x &&
                ptn.y < b.max_box.y && ptn.y > b.min_box.y &&
                ptn.z < b.max_box.z && ptn.z > b.min_box.z);
    }

    template <typename T>
    inline bool AABB<T, _CHKE_>::aabbOverlapsAABB(const AABB<T> &a, const AABB<T> &b)
    {

        if (a.max_box.x < b.min_box.x || a.min_box.x > b.max_box.x)
            return false;
        if (a.max_box.y < b.min_box.y || a.min_box.y > b.max_box.y)
            return false;
        if (a.max_box.z < b.min_box.z || a.min_box.z > b.max_box.z)
            return false;

        return true;
    }

    template <typename T>
    inline AABB<T> AABB<T, _CHKE_>::joinAABB(const AABB<T> &a, const AABB<T> &b)
    {
        using namespace MathCore;
        return AABB<T>(OP<vec3_type>::minimum(a.min_box, b.min_box), OP<vec3_type>::maximum(a.max_box, b.max_box));
    }

    template <typename T>
    inline AABB<T> AABB<T, _CHKE_>::fromTriangle(const vec3_type &a, const vec3_type &b, const vec3_type &c)
    {
        using namespace MathCore;
        vec3_type pos_min = OP<vec3_type>::minimum(OP<vec3_type>::minimum(a, b), c);
        vec3_type pos_max = OP<vec3_type>::maximum(OP<vec3_type>::maximum(a, b), c);
        return AABB<T>(pos_min, pos_max);
    }

    template <typename T>
    inline AABB<T> AABB<T, _CHKE_>::fromTriangle(const Triangle<T> &triangle)
    {
        return AABB<T>::fromTriangle(triangle.a, triangle.b, triangle.c);
    }

    template <typename T>
    inline AABB<T> AABB<T, _CHKE_>::fromSphere(const vec3_type &pos, const float_type &radius)
    {
        vec3_type pos_min = pos - vec3_type(radius, radius, radius);
        vec3_type pos_max = pos + vec3_type(radius, radius, radius);
        return AABB<T>(pos_min, pos_max);
    }

    template <typename T>
    inline AABB<T> AABB<T, _CHKE_>::fromSphere(const Sphere<T> &sphere)
    {
        return AABB<T>::fromSphere(sphere.center, sphere.radius);
    }

    template <typename T>
    inline AABB<T> AABB<T, _CHKE_>::fromLineSegment(const vec3_type &a, const vec3_type &b)
    {
        return AABB<T>(a, b);
    }

    template <typename T>
    inline AABB<T> AABB<T, _CHKE_>::fromLineSegment(const LineSegment<T> &ls)
    {
        return AABB<T>(ls.a, ls.b);
    }

    template <typename T>
    inline AABB<T> AABB<T, _CHKE_>::fromFrustum(const Frustum<T> &frustum)
    {
        /*
        vec3_type min = frustum.vertices[0], max = frustum.vertices[1];
        for (int i = 2; i < 8; i++) {
            min = minimum(min, frustum.vertices[i]);
            max = maximum(max, frustum.vertices[i]);
        }
        return AABB(min, max);
        */
        return frustum.aabb;
    }

    template <typename T>
    inline AABB<T> AABB<T, _CHKE_>::fromOBB(const OBB<T> &obb)
    {
        using namespace MathCore;
        AABB<T> aabb(obb.box_vertices[0], obb.box_vertices[1]);
        for (int i = 2; i < 8; i++)
        {
            aabb.min_box = OP<vec3_type>::minimum(aabb.min_box, obb.box_vertices[i]);
            aabb.max_box = OP<vec3_type>::maximum(aabb.max_box, obb.box_vertices[i]);
        }
        return aabb;
    }

    // Intersect ray R(t) = p + t*d against AABB a. When intersecting,
    // return intersection distance tmin and point q of intersection
    template <typename T>
    inline bool AABB<T, _CHKE_>::raycastAABB(const Ray<T> &r, const AABB<T> &a, float_type *outTmin, vec3_type *outNormal)
    {
        using namespace MathCore;

        const float_type EPSILON = (float_type)0.002;
        int tminIndex = -1;
        float_type tmin = 0.0f;    // set to -FLT_MAX to get first hit on line
        float_type tmax = FLT_MAX; // set to max distance ray can travel (for segment)
        // For all three slabs
        for (int i = 0; i < 3; i++)
        {
            if (OP<float_type>::abs(r.dir[i]) < EPSILON)
            {
                // Ray is parallel to slab. No hit if origin not within slab
                if (r.origin[i] < a.min_box[i] || r.origin[i] > a.max_box[i])
                    return false;
            }
            else
            {
                // Compute intersection t value of ray with near and far plane of slab
                float_type ood = (float_type)1 / r.dir[i];
                float_type t1 = (a.min_box[i] - r.origin[i]) * ood;
                float_type t2 = (a.max_box[i] - r.origin[i]) * ood;
                // Make t1 be intersection with near plane, t2 with far plane
                if (t1 > t2)
                {
                    // Swap(t1, t2);
                    float_type taux = t1;
                    t1 = t2;
                    t2 = taux;
                }
                // Compute the intersection of slab intersection intervals
                if (t1 > tmin)
                {
                    tmin = t1;
                    tminIndex = i;
                }
                if (t2 < tmax)
                    tmax = t2;
                // Exit with no collision as soon as slab intersection becomes empty
                if (tmin > tmax)
                    return false;
            }
        }
        // Ray intersects all 3 slabs. Return point (q) and intersection t value (tmin)
        *outTmin = tmin;
        if (tminIndex == 0)
            *outNormal = vec3_type(-OP<float_type>::sign(r.dir.x), 0, 0);
        else if (tminIndex == 1)
            *outNormal = vec3_type(0, -OP<float_type>::sign(r.dir.y), 0);
        else if (tminIndex == 2)
            *outNormal = vec3_type(0, 0, -OP<float_type>::sign(r.dir.z));
        //*outQ = p + d * tmin;
        return true;
    }

    // Test if segment specified by points p0 and p1 intersects AABB b
    template <typename T>
    inline bool AABB<T, _CHKE_>::segmentIntersectsAABB(const vec3_type &p0, const vec3_type &p1, const AABB<T> &b)
    {
        using namespace MathCore;

        const float_type EPSILON = (float_type)0.002;

        vec3_type c = (b.min_box + b.max_box) * (float_type)0.5; // Box center-point
        vec3_type e = b.max_box - c;                             // Box halflength extents
        vec3_type m = (p0 + p1) * (float_type)0.5;               // Segment midpoint
        vec3_type d = p1 - m;                                    // Segment halflength vector
        m = m - c;                                               // Translate box and segment to origin
        // Try world coordinate axes as separating axes
        float_type adx = OP<float_type>::abs(d.x);
        if (OP<float_type>::abs(m.x) > e.x + adx)
            return false;
        float_type ady = OP<float_type>::abs(d.y);
        if (OP<float_type>::abs(m.y) > e.y + ady)
            return false;
        float_type adz = OP<float_type>::abs(d.z);
        if (OP<float_type>::abs(m.z) > e.z + adz)
            return false;
        // Add in an epsilon term to counteract arithmetic errors when segment is
        // (near) parallel to a coordinate axis (see text for detail)
        adx += EPSILON;
        ady += EPSILON;
        adz += EPSILON;
        // Try cross products of segment direction vector with coordinate axes
        if (OP<float_type>::abs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady)
            return false;
        if (OP<float_type>::abs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx)
            return false;
        if (OP<float_type>::abs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx)
            return false;
        // No separating axis found; segment must be overlapping AABB
        return true;
    }

    template <typename T>
    inline bool AABB<T, _CHKE_>::segmentIntersectsAABB(const LineSegment<T> &ls, const AABB<T> &b)
    {
        return segmentIntersectsAABB(ls.a, ls.b, b);
    }

    // Given point p, return the point q on or in AABB b that is closest to p
    template <typename T>
    inline
        typename AABB<T, _CHKE_>::vec3_type
        AABB<T, _CHKE_>::closestPointToAABB(const vec3_type &p, const AABB<T> &b)
    {
        using namespace MathCore;

        vec3_type q;
        // For each coordinate axis, if the point coordinate value is
        // outside box, clamp it to the box, else keep it as is
        for (int i = 0; i < 3; i++)
        {
            float_type v = p[i];
            // if (v < b.min_box[i])
            //     v = b.min_box[i]; // v = max(v, b.min[i])
            v = OP<float_type>::maximum(v, b.min_box[i]);
            // if (v > b.max_box[i])
            //     v = b.max_box[i]; // v = min(v, b.max[i])
            v = OP<float_type>::minimum(v, b.max_box[i]);
            q[i] = v;
        }
        return q;
    }

    template <typename T>
    inline bool AABB<T, _CHKE_>::sphereOverlapsAABB(const vec3_type &center, const float_type &radius, const AABB<T> &aabb, vec3_type *penetration)
    {
        using namespace MathCore;

        vec3_type closestPointInTriangle = closestPointToAABB(center, aabb);

        vec3_type SphereToAABB = closestPointInTriangle - center;
        float_type sqrLength_SphereToAABB = OP<vec3_type>::dot(SphereToAABB, SphereToAABB);

        float_type Max_Radius_sqr = radius * radius;

        if (sqrLength_SphereToAABB > (float_type)0.00002 && sqrLength_SphereToAABB < Max_Radius_sqr)
        {

            float_type Length_SphereToTriangle = OP<float_type>::sqrt(sqrLength_SphereToAABB);
            vec3_type SphereToTriangleNorm = SphereToAABB * ((float_type)1 / Length_SphereToTriangle); // normalize(SphereToTriangle);
            // Vector3 triangleNormal = Vectormath::Aos::normalize( Vectormath::Aos::cross( p2-p1 , p3-p1 ) );

            // EPSILON - to avoid process the same triangle again...
            const float_type EPSILON = (float_type)0.002;
            *penetration = SphereToTriangleNorm * (radius - Length_SphereToTriangle + EPSILON);
            return true;
        }
        return false;
    }

    template <typename T>
    inline bool AABB<T, _CHKE_>::sphereOverlapsAABB(
        const Sphere<T> &sphere,
        const AABB<T> &aabb,
        vec3_type *penetration)
    {
        return AABB<T>::sphereOverlapsAABB(sphere.center, sphere.radius, aabb, penetration);
    }

    // Test if AABB b intersects plane p
    // p_n = plane normal
    // p_d = plane distance
    template <typename T>
    inline bool AABB<T, _CHKE_>::planeIntersectsAABB(const Plane<T> &plane, const AABB<T> &b)
    {
        using namespace MathCore;

        // These two lines not necessary with a (center, extents) AABB representation
        vec3_type c = (b.max_box + b.min_box) * (float_type)0.5; // Compute AABB center
        vec3_type e = b.max_box - c;                             // Compute positive extents
        // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
        float_type r = e[0] * OP<float_type>::abs(plane.normal[0]) +
                       e[1] * OP<float_type>::abs(plane.normal[1]) +
                       e[2] * OP<float_type>::abs(plane.normal[2]);
        // Compute distance of box center from plane
        float_type s = OP<vec3_type>::dot(plane.normal, c) - plane.distance;
        // Intersection occurs when distance s falls within [-r,+r] interval
        return OP<float_type>::abs(s) <= r;
    }

    //
    // https://stackoverflow.com/questions/17458562/efficient-aabb-triangle-intersection-in-c-sharp
    //
    template <typename T>
    inline bool AABB<T, _CHKE_>::triangleIntersectsAABB(const vec3_type &v0, const vec3_type &v1, const vec3_type &v2, const AABB<T> &box)
    {
        using namespace MathCore;

        const float_type EPSILON = (float_type)1e-6;

        float_type triangleMin, triangleMax;
        float_type boxMin, boxMax;

        vec3_type triangle_Vertices[] = {
            v0, v1, v2};

        // Test the box normals (x-, y- and z-axes)
        vec3_type boxNormals[] = {
            vec3_type(1, 0, 0),
            vec3_type(0, 1, 0),
            vec3_type(0, 0, 1)};

        for (int i = 0; i < 3; i++)
        {
            Util::projectOnAxis(triangle_Vertices, 3, boxNormals[i], &triangleMin, &triangleMax);
            if ((triangleMax < box.min_box[i] - EPSILON) || (triangleMin > box.max_box[i] + EPSILON))
                return false; // No intersection possible.
        }

        // Test the triangle normal
        vec3_type triangle_Normal = OP<vec3_type>::normalize(OP<vec3_type>::cross(v1 - v0, v2 - v0));
        float_type triangleOffset = OP<vec3_type>::dot(triangle_Normal, v0);

        vec3_type box_Vertices[] = {
            vec3_type(box.min_box.x, box.min_box.y, box.min_box.z), // 000
            vec3_type(box.min_box.x, box.min_box.y, box.max_box.z), // 001
            vec3_type(box.min_box.x, box.max_box.y, box.min_box.z), // 010
            vec3_type(box.min_box.x, box.max_box.y, box.max_box.z), // 011
            vec3_type(box.max_box.x, box.min_box.y, box.min_box.z), // 100
            vec3_type(box.max_box.x, box.min_box.y, box.max_box.z), // 101
            vec3_type(box.max_box.x, box.max_box.y, box.min_box.z), // 110
            vec3_type(box.max_box.x, box.max_box.y, box.max_box.z), // 111
        };

        Util::projectOnAxis(box_Vertices, 8, triangle_Normal, &boxMin, &boxMax);

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
            Util::projectOnAxis(box_Vertices, 8, axis, &boxMin, &boxMax);
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
                vec3_type axis = OP<vec3_type>::cross(triangleEdges[i], boxNormals[j]);
                Util::projectOnAxis(box_Vertices, 8, axis, &boxMin, &boxMax);
                Util::projectOnAxis(triangle_Vertices, 3, axis, &triangleMin, &triangleMax);
                if (boxMax < triangleMin - EPSILON || boxMin > triangleMax + EPSILON)
                    return false; // No intersection possible
            }
        */

        // No separating axis found.
        return true;
    }

    template <typename T>
    inline bool AABB<T, _CHKE_>::triangleIntersectsAABB(const Triangle<T> &t, const AABB<T> &box)
    {
        return triangleIntersectsAABB(t.a, t.b, t.c, box);
    }

    template <typename T>
    inline bool AABB<T, _CHKE_>::frustumOverlapsAABB(const Frustum<T> &f, const AABB<T> &aabb)
    {
        return Frustum<T>::aabbOverlapsFrustum(aabb, f);
    }

    template <typename T>
    inline bool AABB<T, _CHKE_>::obbOverlapsAABB(const OBB<T> &obb, const AABB<T> &aabb)
    {
        return OBB<T>::aabbOverlapsOBB(aabb, obb);
    }

}
