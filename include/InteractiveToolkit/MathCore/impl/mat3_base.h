#pragma once

#include "../math_common.h"
#include "../float.h"

namespace MathCore
{

    /// \brief Forward declaration of the 3x3 matrix class template.
    ///
    /// Declares the primary template for the 3x3 matrix `mat3`, which stores
    /// nine components arranged in three rows and three columns to represent a
    /// tridimensional linear transformation.
    ///
    /// This header only provides the forward declaration so that other headers
    /// can reference `mat3` (e.g., in operator overloads, generation, and SIMD
    /// specializations) before its full definition is available. The concrete
    /// definitions are supplied by the corresponding specialization headers.
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _BaseType The scalar type of each component (e.g., float, double).
    /// \tparam _SimdType The SIMD strategy used for the matrix; defaults to SIMD_TYPE::DEFAULT.
    /// \tparam Enable SFINAE tag used to select the correct specialization; defaults to void.
    ///
    template <typename _BaseType, typename _SimdType=SIMD_TYPE::DEFAULT, class Enable = void >
    class mat3{};

}
