#pragma once

#include "../common.h"
#include "../MathCore/MathCore.h"

// Check Enable
#define _CHKE_ typename std::enable_if< \
    std::is_same<typename MathCore::MathTypeInfo<T>::_class, MathCore::MathTypeClass::_class_vec3>::value>::type

namespace CollisionCore
{

    template <typename T, class Enable = void>
    class Sphere
    {
    };
    template <typename T, class Enable = void>
    class LineSegment
    {
    };
    template <typename T, class Enable = void>
    class Plane
    {
    };
    template <typename T, class Enable = void>
    class Ray
    {
    };
    template <typename T, class Enable = void>
    class Triangle
    {
    };
    template <typename T, class Enable = void>
    class Frustum
    {
    };
    template <typename T, class Enable = void>
    class AABB
    {
    };
    template <typename T, class Enable = void>
    class OBB
    {
    };

}
