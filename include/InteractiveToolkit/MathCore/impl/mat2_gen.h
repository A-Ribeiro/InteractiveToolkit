#pragma once

#include "mat2_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    /// \brief Generation operations specialization for mat2 with no SIMD optimization.
    ///
    /// Provides generation (non-SIMD) utility functions for the mat2 class when
    /// SIMD optimizations are disabled (SIMD_TYPE::NONE). This specialization
    /// is selected via SFINAE when the _simd template parameter matches
    /// SIMD_TYPE::NONE.
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _type The scalar type of the mat2 components (e.g., float, double).
    /// \tparam _simd The SIMD strategy type; this specialization is selected when
    ///         _simd is SIMD_TYPE::NONE.
    ///
    template <typename _type, typename _simd>
    struct GEN<mat2<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_simd, SIMD_TYPE::NONE>::value>::type>
    {
        private:
        /// \brief Alias for the fully specialized mat2 type.
        ///
        using typeMat2 = mat2<_type, _simd>;
        /// \brief Alias for the fully specialized mat3 type.
        ///
        using typeMat3 = mat3<_type, _simd>;
        /// \brief Alias for the fully specialized mat4 type.
        ///
        using typeMat4 = mat4<_type, _simd>;
        /// \brief Alias for the fully specialized vec4 type.
        ///
        using typeVec4 = vec4<_type, _simd>;
        /// \brief Alias for the fully specialized vec3 type.
        ///
        using typeVec3 = vec3<_type, _simd>;
        /// \brief Alias for the fully specialized vec2 type.
        ///
        using typeVec2 = vec2<_type, _simd>;
        /// \brief Alias for the GEN specialization type.
        ///
        using self_type = GEN<typeMat2>;
        public:

        /// \brief Create a diagonal scale matrix from two scalar values.
        ///
        /// Constructs a 2x2 diagonal matrix whose diagonal entries are the
        /// given scale factors. The off-diagonal entries are zero:
        ///
        /// \code
        /// | _x_  0  |
        /// | 0   _y_ |
        /// \endcode
        ///
        /// Example:
        ///
        /// \code
        /// mat2<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat2<float, SIMD_TYPE::NONE>>::scale(2.0f, 3.0f);
        /// // m is | 2 0 |
        /// //       | 0 3 |
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _x_ Scale factor for the x axis (top-left entry).
        /// \param _y_ Scale factor for the y axis (bottom-right entry).
        /// \return A diagonal mat2 with the given scale factors.
        ///
        static ITK_INLINE typeMat2 scale(const _type &_x_, const _type &_y_) noexcept
        {
            return typeMat2(
                _x_, 0,
                0, _y_);
        }

        /// \brief Create a diagonal scale matrix from a vec2.
        ///
        /// Constructs a 2x2 diagonal matrix whose diagonal entries are the
        /// components of the given vector. The off-diagonal entries are zero:
        ///
        /// \code
        /// | _v_.x  0     |
        /// | 0     _v_.y |
        /// \endcode
        ///
        /// Example:
        ///
        /// \code
        /// vec2<float, SIMD_TYPE::NONE> v(2.0f, 3.0f);
        /// mat2<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat2<float, SIMD_TYPE::NONE>>::scale(v);
        /// // m is | 2 0 |
        /// //       | 0 3 |
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Vector whose components are used as the scale factors.
        /// \return A diagonal mat2 with the given scale factors.
        ///
        static ITK_INLINE typeMat2 scale(const typeVec2 &_v_) noexcept
        {
            return typeMat2(
                _v_.x, 0,
                0, _v_.y);
        }

        /// \brief Create a diagonal scale matrix from a vec3.
        ///
        /// Constructs a 2x2 diagonal matrix whose diagonal entries are the x
        /// and y components of the given vector. The z component is ignored.
        /// The off-diagonal entries are zero:
        ///
        /// \code
        /// | _v_.x  0     |
        /// | 0     _v_.y |
        /// \endcode
        ///
        /// Example:
        ///
        /// \code
        /// vec3<float, SIMD_TYPE::NONE> v(2.0f, 3.0f, 4.0f);
        /// mat2<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat2<float, SIMD_TYPE::NONE>>::scale(v);
        /// // m is | 2 0 |
        /// //       | 0 3 |
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Vector whose x and y components are used as the scale
        ///        factors; the z component is ignored.
        /// \return A diagonal mat2 with the given scale factors.
        ///
        static ITK_INLINE typeMat2 scale(const typeVec3 &_v_) noexcept
        {
            return typeMat2(
                _v_.x, 0,
                0, _v_.y);
        }

        /// \brief Create a diagonal scale matrix from a vec4.
        ///
        /// Constructs a 2x2 diagonal matrix whose diagonal entries are the x
        /// and y components of the given vector. The z and w components are
        /// ignored. The off-diagonal entries are zero:
        ///
        /// \code
        /// | _v_.x  0     |
        /// | 0     _v_.y |
        /// \endcode
        ///
        /// Example:
        ///
        /// \code
        /// vec4<float, SIMD_TYPE::NONE> v(2.0f, 3.0f, 4.0f, 5.0f);
        /// mat2<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat2<float, SIMD_TYPE::NONE>>::scale(v);
        /// // m is | 2 0 |
        /// //       | 0 3 |
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Vector whose x and y components are used as the scale
        ///        factors; the z and w components are ignored.
        /// \return A diagonal mat2 with the given scale factors.
        ///
        static ITK_INLINE typeMat2 scale(const typeVec4 &_v_) noexcept
        {
            return typeMat2(
                _v_.x, 0,
                0, _v_.y);
        }

        /// \brief Create a 2D rotation matrix from an angle in radians.
        ///
        /// Constructs a 2x2 rotation matrix for a counter-clockwise rotation
        /// by the given angle (in radians) around the origin:
        ///
        /// \code
        /// |  cos(psi)  -sin(psi) |
        /// |  sin(psi)   cos(psi) |
        /// \endcode
        ///
        /// Example:
        ///
        /// \code
        /// mat2<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat2<float, SIMD_TYPE::NONE>>::rotate(3.14159265f / 2.0f);
        /// // m is approximately | 0  -1 |
        /// //                    | 1   0 |
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _psi_ Rotation angle in radians.
        /// \return A 2x2 rotation matrix for the given angle.
        ///
        static ITK_INLINE typeMat2 rotate(const _type &_psi_) noexcept
        {
            _type c = OP<_type>::cos(_psi_);
            _type s = OP<_type>::sin(_psi_);
            return typeMat2(
                c, -s,
                s, c);
        }


        /// \brief Create a mat2 from the top-left 2x2 block of a mat3.
        ///
        /// Extracts the first two rows of the given 3x3 matrix, taking only
        /// the first two components of each row, to form a 2x2 matrix. The
        /// third column and third row of the source matrix are discarded.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m3;
        /// mat2<float, SIMD_TYPE::NONE> m2;
        /// m2 = GEN<mat2<float, SIMD_TYPE::NONE>>::fromMat3(m3);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The source 3x3 matrix.
        /// \return A mat2 containing the top-left 2x2 block of the source.
        ///
        static ITK_INLINE typeMat2 fromMat3(const typeMat3 &v) noexcept
        {
            return typeMat2(
                *(const typeVec2 *)&(v[0]),
                *(const typeVec2 *)&(v[1]));
        }

        /// \brief Create a mat2 from the top-left 2x2 block of a mat4.
        ///
        /// Extracts the first two rows of the given 4x4 matrix, taking only
        /// the first two components of each row, to form a 2x2 matrix. The
        /// third and fourth columns and rows of the source matrix are
        /// discarded.
        ///
        /// Example:
        ///
        /// \code
        /// mat4<float, SIMD_TYPE::NONE> m4;
        /// mat2<float, SIMD_TYPE::NONE> m2;
        /// m2 = GEN<mat2<float, SIMD_TYPE::NONE>>::fromMat4(m4);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The source 4x4 matrix.
        /// \return A mat2 containing the top-left 2x2 block of the source.
        ///
        static ITK_INLINE typeMat2 fromMat4(const typeMat4 &v) noexcept
        {
            return typeMat2(
                *(const typeVec2 *)&(v[0]),
                *(const typeVec2 *)&(v[1]));
        }

    };

}