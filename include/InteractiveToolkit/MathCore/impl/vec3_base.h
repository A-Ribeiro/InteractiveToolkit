#pragma once

#include "../math_common.h"
#include "../float.h"

namespace MathCore
{

    /// \brief Forward declaration of the 3D vector class template.
    ///
    /// Declares the primary template for the tridimensional vector `vec3`, which
    /// stores three components (x, y, z) to represent a 3D vector or point.
    ///
    /// This header only provides the forward declaration so that other headers
    /// can reference `vec3` (e.g., in operator overloads, generation, and SIMD
    /// specializations) before its full definition is available. The concrete
    /// definitions are supplied by the corresponding specialization headers.
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _BaseType The scalar type of each component (e.g., float, double).
    /// \tparam _SimdType The SIMD strategy used for the vector; defaults to SIMD_TYPE::DEFAULT.
    /// \tparam Enable SFINAE tag used to select the correct specialization; defaults to void.
    ///
    template <typename _BaseType, typename _SimdType=SIMD_TYPE::DEFAULT, class Enable = void >
    class vec3{};

}
