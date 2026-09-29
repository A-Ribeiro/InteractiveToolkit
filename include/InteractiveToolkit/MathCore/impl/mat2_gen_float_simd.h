#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'mat2_gen_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "mat2_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    /// \brief Generation operations specialization for SIMD-optimized float mat2.
    ///
    /// Provides generation utility functions for the mat2 class when SIMD
    /// optimizations are enabled (SIMD_TYPE::SSE or SIMD_TYPE::NEON) and the
    /// base type is float. This specialization is selected via SFINAE when
    /// the _type template parameter is float and _simd is either SSE or NEON.
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _type The scalar type of the mat2 components; this specialization
    ///         is selected when _type is float.
    /// \tparam _simd The SIMD strategy type; this specialization is selected when
    ///         _simd is SIMD_TYPE::SSE or SIMD_TYPE::NEON.
    ///
    template <typename _type, typename _simd>
    struct GEN<mat2<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_type, float>::value &&
                   (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                    std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type>
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
#if defined(ITK_SSE2)
            return typeMat2(_mm_setr_ps(_x_, 0,
                                        0, _y_));
#elif defined(ITK_NEON)
            return typeMat2((float32x4_t){_x_, 0,
                                          0, _y_});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
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
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            return _mm_setr_ps(
                _v_.x, 0,
                0, _v_.y);
#else
            __m128 _tmp0 = _mm_shuffle_ps(_v_.array_sse, _v_.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
            _tmp0 = _mm_blend_ps(_tmp0, _vec4_zero_sse, 0x6);

            return _tmp0;
#endif

            // return typeMat2(_mm_setr_ps(_v_.x, 0,
            //                             0, _v_.y));
#elif defined(ITK_NEON)
            return (float32x4_t){_v_.x, 0,
                                 0, _v_.y};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
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
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            return _mm_setr_ps(
                _v_.x, 0,
                0, _v_.y);
#else
            __m128 _tmp0 = _mm_shuffle_ps(_v_.array_sse, _v_.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
            _tmp0 = _mm_blend_ps(_tmp0, _vec4_zero_sse, 0x6);

            return _tmp0;
#endif

            // return typeMat2(_mm_setr_ps(_v_.x, 0,
            //                             0, _v_.y));
#elif defined(ITK_NEON)
            return (float32x4_t){_v_.x, 0,
                                 0, _v_.y};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
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
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            return _mm_setr_ps(
                _v_.x, 0,
                0, _v_.y);
#else
            __m128 _tmp0 = _mm_shuffle_ps(_v_.array_sse, _v_.array_sse, _MM_SHUFFLE(1, 0, 1, 0));
            _tmp0 = _mm_blend_ps(_tmp0, _vec4_zero_sse, 0x6);

            return _tmp0;
#endif

            // return typeMat2(_mm_setr_ps(_v_.x, 0,
            //                             0, _v_.y));
#elif defined(ITK_NEON)
            return (float32x4_t){_v_.x, 0,
                                 0, _v_.y};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
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
#if defined(ITK_SSE2)
            return _mm_setr_ps(c, s,
                               -s, c);
#elif defined(ITK_NEON)
            return (float32x4_t){c, s,
                                 -s, c};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
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
#if defined(ITK_SSE2)
            return typeMat2(v.array_sse[0],
                            v.array_sse[1]);
#elif defined(ITK_NEON)
            return typeMat2(vcombine_f32(vget_low_f32(v.array_neon[0]),
                                         vget_low_f32(v.array_neon[1])));
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
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
#if defined(ITK_SSE2)
            return typeMat2(v.array_sse[0],
                            v.array_sse[1]);
#elif defined(ITK_NEON)
            return typeMat2(vcombine_f32(vget_low_f32(v.array_neon[0]),
                                         vget_low_f32(v.array_neon[1])));
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
    };

}