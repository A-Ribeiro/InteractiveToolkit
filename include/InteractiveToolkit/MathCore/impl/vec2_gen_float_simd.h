#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'vec2_gen_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "vec2_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    /// \brief Generation operations specialization for SIMD-optimized float vec2.
    ///
    /// Provides generation utility functions for the vec2 class when SIMD
    /// optimizations are enabled (SIMD_TYPE::SSE or SIMD_TYPE::NEON) and the
    /// base type is float. This specialization is selected via SFINAE when
    /// the _type template parameter is float and _simd is either SSE or NEON.
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _type The scalar type of the vec2 components; this specialization
    ///         is selected when _type is float.
    /// \tparam _simd The SIMD strategy type; this specialization is selected when
    ///         _simd is SIMD_TYPE::SSE or SIMD_TYPE::NEON.
    ///
    template <typename _type, typename _simd>
    struct GEN<vec2<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_type, float>::value &&
                   (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                    std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type>
    {
    private:
        using typeVec2 = vec2<_type, _simd>;
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
        /// vec2<float, SIMD_TYPE::SSE> result;
        /// result = GEN<vec2<float, SIMD_TYPE::SSE>>::fromPolar( 45.0f, 10.0f );
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