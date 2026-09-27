#pragma once

#include "vec2_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    /// \brief Generation operations specialization for vec2 with no SIMD optimization.
    ///
    /// Provides generation (non-SIMD) utility functions for the vec2 class when
    /// SIMD optimizations are disabled (SIMD_TYPE::NONE). This specialization
    /// is selected via SFINAE when the _simd template parameter matches
    /// SIMD_TYPE::NONE.
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _type The scalar type of the vec2 components (e.g., float, double).
    /// \tparam _simd The SIMD strategy type; this specialization is selected when
    ///         _simd is SIMD_TYPE::NONE.
    ///
    template <typename _type, typename _simd>
    struct GEN<vec2<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_simd, SIMD_TYPE::NONE>::value>::type>
    {
    private:
        /// \brief Alias for the fully specialized vec2 type.
        ///
        using typeVec2 = vec2<_type, _simd>;
        /// \brief Alias for the GEN specialization type.
        ///
        using self_type = GEN<typeVec2>;

    public:
        /// \brief Create a vec2 from polar (angle, radius) coordinates.
        ///
        /// Converts polar coordinates to a Cartesian vec2. The angle is given
        /// in degrees and is internally converted to radians. The resulting
        /// vector is computed as:
        ///
        /// \code
        /// x = cos(angle_in_radians) * radius
        /// y = sin(angle_in_radians) * radius
        /// \endcode
        ///
        /// Example:
        ///
        /// \code
        /// vec2<float, SIMD_TYPE::NONE> result;
        /// result = GEN<vec2<float, SIMD_TYPE::NONE>>::fromPolar( 45.0f, 10.0f );
        /// // result is approximately (7.071f, 7.071f)
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param pAngle Angle in degrees.
        /// \param pRadius Radius (distance from the origin).
        /// \return A vec2 representing the Cartesian coordinates corresponding
        ///         to the given polar coordinates.
        ///
        static ITK_INLINE typeVec2 fromPolar(const _type &pAngle, const _type &pRadius) noexcept
        {
            _type angleRad = OP<_type>::deg_2_rad(pAngle);
            return typeVec2(OP<_type>::cos(angleRad),
                            OP<_type>::sin(angleRad)) *
                   pRadius;
        }
    };

}