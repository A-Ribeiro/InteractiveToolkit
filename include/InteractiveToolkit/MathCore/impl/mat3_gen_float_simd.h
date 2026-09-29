#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'mat3_gen_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "mat3_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    /// \brief Generation operations specialization for SIMD-optimized float mat3.
    ///
    /// Provides generation utility functions for the mat3 class when SIMD
    /// optimizations are enabled (SIMD_TYPE::SSE or SIMD_TYPE::NEON) and the
    /// base type is float. This specialization is selected via SFINAE when
    /// the _type template parameter is float and _simd is either SSE or NEON.
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _type The scalar type of the mat3 components; this specialization
    ///         is selected when _type is float.
    /// \tparam _simd The SIMD strategy type; this specialization is selected when
    ///         _simd is SIMD_TYPE::SSE or SIMD_TYPE::NEON.
    ///
    template <typename _type, typename _simd>
    struct GEN<mat3<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_type, float>::value &&
                   (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                    std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type>
    {
    private:
        /// \brief Alias for the fully specialized mat3 type.
        ///
        using typeMat3 = mat3<_type, _simd>;
        /// \brief Alias for the fully specialized mat2 type.
        ///
        using typeMat2 = mat2<_type, _simd>;
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
        /// \brief Alias for the fully specialized quat type.
        ///
        using quatT = quat<_type, _simd>;
        /// \brief Alias for the GEN specialization type.
        ///
        using self_type = GEN<typeMat3>;

    public:
        
        /// \brief Build a homogeneous 2D translation matrix from scalar components.
        ///
        /// Returns a 3x3 matrix that translates a point by (_x_, _y_) in the
        /// x and y axes while leaving the z axis unchanged.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::translateHomogeneous(1.0f, 2.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _x_ Translation amount along the x axis.
        /// \param _y_ Translation amount along the y axis.
        /// \return A 3x3 homogeneous translation matrix.
        ///
        static ITK_INLINE typeMat3 translateHomogeneous(const _type &_x_, const _type &_y_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat3(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _mm_setr_ps(_x_, _y_, 1, 0));
#elif defined(ITK_NEON)
            return typeMat3(_neon_1000,
                            _neon_0100,
                            (float32x4_t){_x_, _y_, 1, 0});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a homogeneous 2D translation matrix from a vec2.
        ///
        /// Returns a 3x3 matrix that translates a point by the x and y
        /// components of the given vector while leaving the z axis unchanged.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::translateHomogeneous(vec2<float, SIMD_TYPE::NONE>(1.0f, 2.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Translation vector; its x and y components are used.
        /// \return A 3x3 homogeneous translation matrix.
        ///
        static ITK_INLINE typeMat3 translateHomogeneous(const typeVec2 &_v_) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            __m128 _t = _mm_setr_ps(_v_.x, _v_.y, 1.0f, 0.0f);
            return typeMat3(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _t);
#else
            return typeMat3(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _mm_blend_ps(_v_.array_sse, _vec4_0010_sse, 0xC));
#endif
#elif defined(ITK_NEON)
            return typeMat3(_neon_1000,
                            _neon_0100,
                            (float32x4_t){_v_.x, _v_.y, 1, 0});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a homogeneous 2D translation matrix from a vec3.
        ///
        /// Returns a 3x3 matrix that translates a point by the x and y
        /// components of the given vector while leaving the z axis unchanged.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::translateHomogeneous(vec3<float, SIMD_TYPE::NONE>(1.0f, 2.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Translation vector; its x and y components are used.
        /// \return A 3x3 homogeneous translation matrix.
        ///
        static ITK_INLINE typeMat3 translateHomogeneous(const typeVec3 &_v_) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            __m128 _t = _mm_setr_ps(_v_.x, _v_.y, 1.0f, 0.0f);
            return typeMat3(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _t);
#else
            return typeMat3(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _mm_blend_ps(_v_.array_sse, _vec4_0010_sse, 0xC));
#endif
#elif defined(ITK_NEON)
            return typeMat3(_neon_1000,
                            _neon_0100,
                            (float32x4_t){_v_.x, _v_.y, 1, 0});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a homogeneous 2D translation matrix from a vec4.
        ///
        /// Returns a 3x3 matrix that translates a point by the x and y
        /// components of the given vector while leaving the z axis unchanged.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::translateHomogeneous(vec4<float, SIMD_TYPE::NONE>(1.0f, 2.0f, 0.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Translation vector; its x and y components are used.
        /// \return A 3x3 homogeneous translation matrix.
        ///
        static ITK_INLINE typeMat3 translateHomogeneous(const typeVec4 &_v_) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            __m128 _t = _mm_setr_ps(_v_.x, _v_.y, 1.0f, 0.0f);
            return typeMat3(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _t);
#else
            return typeMat3(_vec4_1000_sse,
                            _vec4_0100_sse,
                            _mm_blend_ps(_v_.array_sse, _vec4_0010_sse, 0xC));
#endif
#elif defined(ITK_NEON)
            return typeMat3(_neon_1000,
                            _neon_0100,
                            (float32x4_t){_v_.x, _v_.y, 1, 0});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a homogeneous 2D scale matrix from scalar components.
        ///
        /// Returns a 3x3 matrix that scales a point by _x_ along the x axis and
        /// _y_ along the y axis while leaving the z axis unchanged.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::scaleHomogeneous(2.0f, 3.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _x_ Scale factor along the x axis.
        /// \param _y_ Scale factor along the y axis.
        /// \return A 3x3 homogeneous scale matrix.
        ///
        static ITK_INLINE typeMat3 scaleHomogeneous(const _type &_x_, const _type &_y_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat3(
                _mm_setr_ps(_x_, 0, 0, 0),
                _mm_setr_ps(0, _y_, 0, 0),
                _vec4_0010_sse);
#elif defined(ITK_NEON)
            return typeMat3((float32x4_t){_x_, 0, 0, 0},
                            (float32x4_t){0, _y_, 0, 0},
                            _neon_0010);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a homogeneous 2D scale matrix from a vec2.
        ///
        /// Returns a 3x3 matrix that scales a point by the x and y components of
        /// the given vector while leaving the z axis unchanged.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::scaleHomogeneous(vec2<float, SIMD_TYPE::NONE>(2.0f, 3.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Scale vector; its x and y components are used.
        /// \return A 3x3 homogeneous scale matrix.
        ///
        static ITK_INLINE typeMat3 scaleHomogeneous(const typeVec2 &_v_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat3(
                _mm_setr_ps(_v_.x, 0, 0, 0),
                _mm_setr_ps(0, _v_.y, 0, 0),
                _vec4_0010_sse);
#elif defined(ITK_NEON)
            return typeMat3((float32x4_t){_v_.x, 0, 0, 0},
                            (float32x4_t){0, _v_.y, 0, 0},
                            _neon_0010);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a homogeneous 2D scale matrix from a vec3.
        ///
        /// Returns a 3x3 matrix that scales a point by the x and y components of
        /// the given vector while leaving the z axis unchanged.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::scaleHomogeneous(vec3<float, SIMD_TYPE::NONE>(2.0f, 3.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Scale vector; its x and y components are used.
        /// \return A 3x3 homogeneous scale matrix.
        ///
        static ITK_INLINE typeMat3 scaleHomogeneous(const typeVec3 &_v_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat3(
                _mm_setr_ps(_v_.x, 0, 0, 0),
                _mm_setr_ps(0, _v_.y, 0, 0),
                _vec4_0010_sse);
#elif defined(ITK_NEON)
            return typeMat3((float32x4_t){_v_.x, 0, 0, 0},
                            (float32x4_t){0, _v_.y, 0, 0},
                            _neon_0010);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a homogeneous 2D scale matrix from a vec4.
        ///
        /// Returns a 3x3 matrix that scales a point by the x and y components of
        /// the given vector while leaving the z axis unchanged.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::scaleHomogeneous(vec4<float, SIMD_TYPE::NONE>(2.0f, 3.0f, 0.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Scale vector; its x and y components are used.
        /// \return A 3x3 homogeneous scale matrix.
        ///
        static ITK_INLINE typeMat3 scaleHomogeneous(const typeVec4 &_v_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat3(
                _mm_setr_ps(_v_.x, 0, 0, 0),
                _mm_setr_ps(0, _v_.y, 0, 0),
                _vec4_0010_sse);
#elif defined(ITK_NEON)
            return typeMat3((float32x4_t){_v_.x, 0, 0, 0},
                            (float32x4_t){0, _v_.y, 0, 0},
                            _neon_0010);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a homogeneous 2D rotation matrix about the z axis.
        ///
        /// Returns a 3x3 matrix that rotates a point by _psi_ radians about the
        /// z axis while leaving the z axis unchanged.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::zRotateHomogeneous(3.14159f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _psi_ Rotation angle in radians about the z axis.
        /// \return A 3x3 homogeneous rotation matrix.
        ///
        static ITK_INLINE typeMat3 zRotateHomogeneous(const _type &_psi_) noexcept
        {
            _type c = OP<_type>::cos(_psi_);
            _type s = OP<_type>::sin(_psi_);
#if defined(ITK_SSE2)
            return typeMat3(
                _mm_setr_ps(c, s, 0, 0),
                _mm_setr_ps(-s, c, 0, 0),
                _vec4_0010_sse);
#elif defined(ITK_NEON)
            return typeMat3((float32x4_t){c, s, 0, 0},
                            (float32x4_t){-s, c, 0, 0},
                            _neon_0010);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a 3D scale matrix from scalar components.
        ///
        /// Returns a 3x3 matrix that scales a point by _x_, _y_ and _z_ along
        /// the x, y and z axes respectively. The z scale defaults to 1.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::scale(2.0f, 3.0f, 4.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _x_ Scale factor along the x axis.
        /// \param _y_ Scale factor along the y axis.
        /// \param _z_ Scale factor along the z axis. Defaults to 1.
        /// \return A 3x3 scale matrix.
        ///
        static ITK_INLINE typeMat3 scale(const _type &_x_, const _type &_y_, const _type &_z_ = (_type)1) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat3(
                _mm_setr_ps(_x_, 0, 0, 0),
                _mm_setr_ps(0, _y_, 0, 0),
                _mm_setr_ps(0, 0, _z_, 0));
#elif defined(ITK_NEON)
            return typeMat3((float32x4_t){_x_, 0, 0, 0},
                            (float32x4_t){0, _y_, 0, 0},
                            (float32x4_t){0, 0, _z_, 0});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a 3D scale matrix from a vec2.
        ///
        /// Returns a 3x3 matrix that scales a point by the x and y components of
        /// the given vector along the x and y axes, leaving the z axis unchanged.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::scale(vec2<float, SIMD_TYPE::NONE>(2.0f, 3.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Scale vector; its x and y components are used.
        /// \return A 3x3 scale matrix.
        ///
        static ITK_INLINE typeMat3 scale(const typeVec2 &_v_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat3(
                _mm_setr_ps(_v_.x, 0, 0, 0),
                _mm_setr_ps(0, _v_.y, 0, 0),
                _vec4_0010_sse);
#elif defined(ITK_NEON)
            return typeMat3((float32x4_t){_v_.x, 0, 0, 0},
                            (float32x4_t){0, _v_.y, 0, 0},
                            _neon_0010);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a 3D scale matrix from a vec3.
        ///
        /// Returns a 3x3 matrix that scales a point by the x, y and z components
        /// of the given vector along the x, y and z axes respectively.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::scale(vec3<float, SIMD_TYPE::NONE>(2.0f, 3.0f, 4.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Scale vector; its x, y and z components are used.
        /// \return A 3x3 scale matrix.
        ///
        static ITK_INLINE typeMat3 scale(const typeVec3 &_v_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat3(
                _mm_setr_ps(_v_.x, 0, 0, 0),
                _mm_setr_ps(0, _v_.y, 0, 0),
                _mm_setr_ps(0, 0, _v_.z, 0));
#elif defined(ITK_NEON)
            return typeMat3((float32x4_t){_v_.x, 0, 0, 0},
                            (float32x4_t){0, _v_.y, 0, 0},
                            (float32x4_t){0, 0, _v_.z, 0});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a 3D scale matrix from a vec4.
        ///
        /// Returns a 3x3 matrix that scales a point by the x, y and z components
        /// of the given vector along the x, y and z axes respectively.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::scale(vec4<float, SIMD_TYPE::NONE>(2.0f, 3.0f, 4.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _v_ Scale vector; its x, y and z components are used.
        /// \return A 3x3 scale matrix.
        ///
        static ITK_INLINE typeMat3 scale(const typeVec4 &_v_) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat3(
                _mm_setr_ps(_v_.x, 0, 0, 0),
                _mm_setr_ps(0, _v_.y, 0, 0),
                _mm_setr_ps(0, 0, _v_.z, 0));
#elif defined(ITK_NEON)
            return typeMat3((float32x4_t){_v_.x, 0, 0, 0},
                            (float32x4_t){0, _v_.y, 0, 0},
                            (float32x4_t){0, 0, _v_.z, 0});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a 3D rotation matrix about the x axis.
        ///
        /// Returns a 3x3 matrix that rotates a point by _phi_ radians about the
        /// x axis.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::xRotate(3.14159f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _phi_ Rotation angle in radians about the x axis.
        /// \return A 3x3 rotation matrix.
        ///
        static ITK_INLINE typeMat3 xRotate(const _type &_phi_) noexcept
        {
            _type c = OP<_type>::cos(_phi_);
            _type s = OP<_type>::sin(_phi_);
#if defined(ITK_SSE2)
            return typeMat3(
                _vec4_1000_sse,
                _mm_setr_ps(0, c, s, 0),
                _mm_setr_ps(0, -s, c, 0));
#elif defined(ITK_NEON)
            return typeMat3(_neon_1000,
                            (float32x4_t){0, c, s, 0},
                            (float32x4_t){0, -s, c, 0});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a 3D rotation matrix about the y axis.
        ///
        /// Returns a 3x3 matrix that rotates a point by _theta_ radians about
        /// the y axis.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::yRotate(3.14159f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _theta_ Rotation angle in radians about the y axis.
        /// \return A 3x3 rotation matrix.
        ///
        static ITK_INLINE typeMat3 yRotate(const _type &_theta_) noexcept
        {
            _type c = OP<_type>::cos(_theta_);
            _type s = OP<_type>::sin(_theta_);
#if defined(ITK_SSE2)
            return typeMat3(
                _mm_setr_ps(c, 0, -s, 0),
                _vec4_0100_sse,
                _mm_setr_ps(s, 0, c, 0));
#elif defined(ITK_NEON)
            return typeMat3((float32x4_t){c, 0, -s, 0},
                            _neon_0100,
                            (float32x4_t){s, 0, c, 0});
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a 3D rotation matrix about the z axis.
        ///
        /// Returns a 3x3 matrix that rotates a point by _psi_ radians about the
        /// z axis.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::zRotate(3.14159f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _psi_ Rotation angle in radians about the z axis.
        /// \return A 3x3 rotation matrix.
        ///
        static ITK_INLINE typeMat3 zRotate(const _type &_psi_) noexcept
        {
            _type c = OP<_type>::cos(_psi_);
            _type s = OP<_type>::sin(_psi_);
#if defined(ITK_SSE2)
            return typeMat3(
                _mm_setr_ps(c, s, 0, 0),
                _mm_setr_ps(-s, c, 0, 0),
                _vec4_0010_sse);
#elif defined(ITK_NEON)
            return typeMat3((float32x4_t){c, s, 0, 0},
                            (float32x4_t){-s, c, 0, 0},
                            _neon_0010);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a 3D rotation matrix from Euler angles.
        ///
        /// Composes the rotation as zRotate(yaw) * yRotate(pitch) *
        /// xRotate(roll), applying roll about the x axis, then pitch about the
        /// y axis, then yaw about the z axis.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::fromEuler(0.1f, 0.2f, 0.3f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param roll Rotation angle in radians about the x axis.
        /// \param pitch Rotation angle in radians about the y axis.
        /// \param yaw Rotation angle in radians about the z axis.
        /// \return A 3x3 rotation matrix composed from the Euler angles.
        ///
        static ITK_INLINE typeMat3 fromEuler(const _type &roll, const _type &pitch, const _type &yaw) noexcept
        {
            return self_type::zRotate(yaw) * self_type::yRotate(pitch) * self_type::xRotate(roll);
        }

        /// \brief Build a 3D rotation matrix from an angle and an axis.
        ///
        /// Returns a 3x3 matrix that rotates a point by _ang_ radians about the
        /// axis defined by (_x, _y, _z). The axis is normalized internally; a
        /// zero-length axis is clamped to avoid division by zero.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::rotate(3.14159f, 0.0f, 0.0f, 1.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _ang_ Rotation angle in radians.
        /// \param _x x component of the rotation axis.
        /// \param _y y component of the rotation axis.
        /// \param _z z component of the rotation axis.
        /// \return A 3x3 rotation matrix.
        ///
        static ITK_INLINE typeMat3 rotate(const _type &_ang_, const _type &_x, const _type &_y, const _type &_z) noexcept
        {
            _type x = _x;
            _type y = _y;
            _type z = _z;

            using type_info = FloatTypeInfo<_type>;
            // depois tem como otimizar esta rotação
            _type length_inv = x * x + y * y + z * z;
            length_inv = OP<_type>::sqrt(length_inv);
            length_inv = OP<_type>::maximum(length_inv, type_info::min);
            // ITK_ABORT(length_inv == 0, "division by zero\n");
            length_inv = (_type)1 / length_inv;

            x *= length_inv;
            y *= length_inv;
            z *= length_inv;

            _type c = OP<_type>::cos(_ang_);
            _type s = OP<_type>::sin(_ang_);

            _type _1_m_c = (_type)1 - c;
            // original -- rotacao em sentido anti-horario
            return typeMat3(x * x * _1_m_c + c, x * y * _1_m_c - z * s, x * z * _1_m_c + y * s,
                            y * x * _1_m_c + z * s, y * y * _1_m_c + c, y * z * _1_m_c - x * s,
                            x * z * _1_m_c - y * s, y * z * _1_m_c + x * s, z * z * _1_m_c + c);

            // transposto -- rotacao em sentido horario
            //   return  mat4(x*x*(1-c)+c  ,  y*x*(1-c)+z*s  ,  x*z*(1-c)-y*s,   0  ,
            //                x*y*(1-c)-z*s,  y*y*(1-c)+c    ,  y*z*(1-c)+x*s,   0  ,
            //                x*z*(1-c)+y*s,  y*z*(1-c)-x*s  ,  z*z*(1-c)+c  ,   0  ,
            //                    0        ,        0        ,      0        ,   1  );
        }

        /// \brief Build a 3D rotation matrix from an angle and a vec2 axis.
        ///
        /// Returns a 3x3 matrix that rotates a point by _ang_ radians about the
        /// axis defined by the x and y components of the given vector, with the
        /// z component set to zero.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::rotate(3.14159f, vec2<float, SIMD_TYPE::NONE>(1.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _ang_ Rotation angle in radians.
        /// \param axis Rotation axis; its x and y components are used.
        /// \return A 3x3 rotation matrix.
        ///
        static ITK_INLINE typeMat3 rotate(const _type &_ang_, const typeVec2 &axis) noexcept
        {
            return self_type::rotate(_ang_, axis.x, axis.y, 0);
        }

        /// \brief Build a 3D rotation matrix from an angle and a vec3 axis.
        ///
        /// Returns a 3x3 matrix that rotates a point by _ang_ radians about the
        /// axis defined by the x, y and z components of the given vector.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::rotate(3.14159f, vec3<float, SIMD_TYPE::NONE>(0.0f, 0.0f, 1.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _ang_ Rotation angle in radians.
        /// \param axis Rotation axis; its x, y and z components are used.
        /// \return A 3x3 rotation matrix.
        ///
        static ITK_INLINE typeMat3 rotate(const _type &_ang_, const typeVec3 &axis) noexcept
        {
            return self_type::rotate(_ang_, axis.x, axis.y, axis.z);
        }

        /// \brief Build a 3D rotation matrix from an angle and a vec4 axis.
        ///
        /// Returns a 3x3 matrix that rotates a point by _ang_ radians about the
        /// axis defined by the x, y and z components of the given vector.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::rotate(3.14159f, vec4<float, SIMD_TYPE::NONE>(0.0f, 0.0f, 1.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _ang_ Rotation angle in radians.
        /// \param axis Rotation axis; its x, y and z components are used.
        /// \return A 3x3 rotation matrix.
        ///
        static ITK_INLINE typeMat3 rotate(const _type &_ang_, const typeVec4 &axis) noexcept
        {
            return self_type::rotate(_ang_, axis.x, axis.y, axis.z);
        }

        /// \brief Build a right-handed look-at rotation matrix from 2D vectors.
        ///
        /// Constructs an orthonormal basis from a front direction and a 2D
        /// position, producing a 3x3 rotation matrix for a right-handed
        /// coordinate system.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::lookAtRotationRH(vec2<float, SIMD_TYPE::NONE>(0.0f, 1.0f), vec2<float, SIMD_TYPE::NONE>(0.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _front The front (look) direction.
        /// \param position The 2D position used to build the basis.
        /// \return A 3x3 right-handed look-at rotation matrix.
        ///
        static ITK_INLINE typeMat3 lookAtRotationRH(const typeVec2 &_front, const typeVec2 &position) noexcept
        {
            typeVec2 front = OP<typeVec2>::normalize(_front);
            typeVec2 side = OP<typeVec2>::cross_z_up(front);

            typeVec3 x, y, z;
            z = typeVec3(position, 1);
            x = -typeVec3(front, 0);
            y = typeVec3(side, 0);

            return typeMat3(x, y, z);
        }

        /// \brief Build a left-handed look-at rotation matrix from 2D vectors.
        ///
        /// Constructs an orthonormal basis from a front direction and a 2D
        /// position, producing a 3x3 rotation matrix for a left-handed
        /// coordinate system.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::lookAtRotationLH(vec2<float, SIMD_TYPE::NONE>(0.0f, 1.0f), vec2<float, SIMD_TYPE::NONE>(0.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param _front The front (look) direction.
        /// \param position The 2D position used to build the basis.
        /// \return A 3x3 left-handed look-at rotation matrix.
        ///
        static ITK_INLINE typeMat3 lookAtRotationLH(const typeVec2 &_front, const typeVec2 &position) noexcept
        {
            typeVec2 front = OP<typeVec2>::normalize(_front);
            typeVec2 side = OP<typeVec2>::cross_z_up(front);

            typeVec3 x, y, z;
            z = typeVec3(position, 1);
            x = typeVec3(front, 0);
            y = typeVec3(side, 0);

            return typeMat3(x, y, z);
        }

        /// \brief Build a right-handed look-at rotation matrix from 3D vectors.
        ///
        /// Constructs an orthonormal basis from a front direction and an up
        /// vector, producing a 3x3 rotation matrix for a right-handed
        /// coordinate system.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::lookAtRotationRH(vec3<float, SIMD_TYPE::NONE>(0.0f, 0.0f, 1.0f), vec3<float, SIMD_TYPE::NONE>(0.0f, 1.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param front The front (look) direction.
        /// \param up The up direction used to build the basis.
        /// \return A 3x3 right-handed look-at rotation matrix.
        ///
        static ITK_INLINE typeMat3 lookAtRotationRH(const typeVec3 &front, const typeVec3 &up) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;
            z = -OP<typeVec3>::normalize(lookTo);
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(up, z));
            y = OP<typeVec3>::cross(z, x);
            return typeMat3(x, y, z);
        }

        /// \brief Build a left-handed look-at rotation matrix from 3D vectors.
        ///
        /// Constructs an orthonormal basis from a front direction and an up
        /// vector, producing a 3x3 rotation matrix for a left-handed
        /// coordinate system.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::lookAtRotationLH(vec3<float, SIMD_TYPE::NONE>(0.0f, 0.0f, 1.0f), vec3<float, SIMD_TYPE::NONE>(0.0f, 1.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param front The front (look) direction.
        /// \param up The up direction used to build the basis.
        /// \return A 3x3 left-handed look-at rotation matrix.
        ///
        static ITK_INLINE typeMat3 lookAtRotationLH(const typeVec3 &front, const typeVec3 &up) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;
            z = OP<typeVec3>::normalize(lookTo);
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(up, z));
            y = OP<typeVec3>::cross(z, x);
            return typeMat3(x, y, z);
        }

        /// \brief Build a right-handed look-at rotation matrix from 4D vectors.
        ///
        /// Constructs an orthonormal basis from a front direction and an up
        /// vector (using their 3D components), producing a 3x3 rotation matrix
        /// for a right-handed coordinate system.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::lookAtRotationRH(vec4<float, SIMD_TYPE::NONE>(0.0f, 0.0f, 1.0f, 0.0f), vec4<float, SIMD_TYPE::NONE>(0.0f, 1.0f, 0.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param front The front (look) direction.
        /// \param up The up direction used to build the basis.
        /// \return A 3x3 right-handed look-at rotation matrix.
        ///
        static ITK_INLINE typeMat3 lookAtRotationRH(const typeVec4 &front, const typeVec4 &up) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;
            z = -OP<typeVec3>::normalize(*(const typeVec3 *)&lookTo);
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(*(const typeVec3 *)&up, z));
            y = OP<typeVec3>::cross(z, x);
            return typeMat3(x, y, z);
        }

        /// \brief Build a left-handed look-at rotation matrix from 4D vectors.
        ///
        /// Constructs an orthonormal basis from a front direction and an up
        /// vector (using their 3D components), producing a 3x3 rotation matrix
        /// for a left-handed coordinate system.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::lookAtRotationLH(vec4<float, SIMD_TYPE::NONE>(0.0f, 0.0f, 1.0f, 0.0f), vec4<float, SIMD_TYPE::NONE>(0.0f, 1.0f, 0.0f, 0.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param front The front (look) direction.
        /// \param up The up direction used to build the basis.
        /// \return A 3x3 left-handed look-at rotation matrix.
        ///
        static ITK_INLINE typeMat3 lookAtRotationLH(const typeVec4 &front, const typeVec4 &up) noexcept
        {
            typeVec3 lookTo = front;
            typeVec3 x, y, z;
            z = OP<typeVec3>::normalize(*(const typeVec3 *)&lookTo);
            x = OP<typeVec3>::normalize(OP<typeVec3>::cross(*(const typeVec3 *)&up, z));
            y = OP<typeVec3>::cross(z, x);
            return typeMat3(x, y, z);
        }

        /// \brief Build a 3D rotation matrix from a quaternion.
        ///
        /// Converts a unit quaternion into the equivalent 3x3 rotation matrix.
        /// The input quaternion is expected to be of unit length.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::fromQuat(quat<float, SIMD_TYPE::NONE>(0.0f, 0.0f, 0.0f, 1.0f));
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param q The unit quaternion to convert.
        /// \return A 3x3 rotation matrix equivalent to the quaternion.
        ///
        static ITK_INLINE typeMat3 fromQuat(const quatT &q) noexcept
        {
#if defined(ITK_SSE2)
            __m128 elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 0, 1));
            __m128 elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 2, 1, 1));
            __m128 op0 = _mm_mul_ps(elm0, elm1);
            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 3, 3, 2));
            elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 2, 2));
            __m128 op1 = _mm_mul_ps(elm0, elm1);
            const __m128 mask01 = _mm_setr_ps(1.0f, 1.0f, -1.0f, 0.0f);
            op1 = _mm_mul_ps(mask01, op1);

            __m128 m0_ = _mm_add_ps(op0, op1);
            const __m128 mask02 = _mm_setr_ps(-2.0f, 2.0f, 2.0f, 0.0f);
            m0_ = _mm_mul_ps(mask02, m0_);
            // m0_[0] += 1.0f;
            m0_ = _mm_add_ps(m0_, _vec4_1000_sse);

            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 0, 0));
            elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 2, 0, 1));
            op0 = _mm_mul_ps(elm0, elm1);
            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 3, 2, 3));
            __m128 elm1_s = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 2, 2));
            op1 = _mm_mul_ps(elm0, elm1_s);
            const __m128 mask03 = _mm_setr_ps(-1.0f, 1.0f, 1.0f, 0.0f);
            op1 = _mm_mul_ps(mask03, op1);

            __m128 m1_ = _mm_add_ps(op0, op1);
            const __m128 mask04 = _mm_setr_ps(2.0f, -2.0f, 2.0f, 0.0f);
            m1_ = _mm_mul_ps(mask04, m1_);
            // m1_[1] += 1.0f;
            m1_ = _mm_add_ps(m1_, _vec4_0100_sse);

            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 1, 0));
            // elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 0, 2, 2));
            op0 = _mm_mul_ps(elm0, elm1_s);
            elm0 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 3, 3));
            elm1 = _mm_shuffle_ps(q.array_sse, q.array_sse, _MM_SHUFFLE(0, 1, 0, 1));
            op1 = _mm_mul_ps(elm0, elm1);
            const __m128 mask05 = _mm_setr_ps(1.0f, -1.0f, 1.0f, 0.0f);
            op1 = _mm_mul_ps(mask05, op1);

            __m128 m2_ = _mm_add_ps(op0, op1);
            const __m128 mask06 = _mm_setr_ps(2.0f, 2.0f, -2.0f, 0.0f);
            m2_ = _mm_mul_ps(mask06, m2_);
            // m2_[2] += 1.0f;
            m2_ = _mm_add_ps(m2_, _vec4_0010_sse);

            //__m128 m3_ = (__m128){0,0,0,1.0f};

            return typeMat3(m0_, m1_, m2_);
#elif defined(ITK_NEON)
            float32x4_t x2y2z2 = vmulq_f32(q.array_neon, q.array_neon);

            float32x4_t op0 = vshuffle_0100(q.array_neon);
            float32x4_t op1 = vshuffle_0221(q.array_neon);

            float32x4_t xy_xz_yz = vmulq_f32(op0, op1);
            float32x4_t wx_wy_wz = vmulq_f32(vshuffle_3333(q.array_neon), q.array_neon);

            //const float32x4_t _2 = vset1(2.0f);
            const float32x4_t mask_a = (float32x4_t){-2.0f, 2.0f, 2.0f, 0.0f};
            const float32x4_t mask_b = (float32x4_t){2.0f, -2.0f, 2.0f, 0.0f};
            const float32x4_t mask_c = (float32x4_t){2.0f, 2.0f, -2.0f, 0.0f};

            float32x4_t m0 =
                vmulq_f32(mask_a, (float32x4_t){
                                  (x2y2z2[1] + x2y2z2[2]),
                                  (xy_xz_yz[0] + wx_wy_wz[2]),
                                  (xy_xz_yz[1] - wx_wy_wz[1]),
                                  0});

            //m0[0] = 1.0f - m0[0];
            m0 = vaddq_f32(m0, _neon_1000);

            float32x4_t m1 =
                vmulq_f32(mask_b, (float32x4_t){
                                  (xy_xz_yz[0] - wx_wy_wz[2]),
                                  (x2y2z2[0] + x2y2z2[2]),
                                  (xy_xz_yz[2] + wx_wy_wz[0]),
                                  0});
            //m1[1] = 1.0f - m1[1];
            m1 = vaddq_f32(m1, _neon_0100);

            float32x4_t m2 =
                vmulq_f32(mask_c, (float32x4_t){
                                  (xy_xz_yz[1] + wx_wy_wz[1]),
                                  (xy_xz_yz[2] - wx_wy_wz[0]),
                                  (x2y2z2[0] + x2y2z2[1]),
                                  0});

            //m2[2] = 1.0f - m2[2];
            m2 = vaddq_f32(m2, _neon_0010);

            // const float32x4_t m3 = (float32x4_t){0,0,0,1.0f};

            return typeMat3(m0, m1, m2); // m3
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a 3D matrix from a 2D matrix.
        ///
        /// Embeds a 2x2 matrix into the upper-left 2x2 block of a 3x3 matrix,
        /// completing the third row and column with identity values.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::fromMat2(mat2<float, SIMD_TYPE::NONE>());
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The 2x2 matrix to embed.
        /// \return A 3x3 matrix with the 2x2 matrix in its upper-left block.
        ///
        static ITK_INLINE typeMat3 fromMat2(const typeMat2 &v) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            return typeMat3(
                _mm_setr_ps(v.a1, v.a2, 0, 0),
                _mm_setr_ps(v.b1, v.b2, 0, 0),
                _vec4_0010_sse,
                _vec4_0001_sse);
#else
            return typeMat3(
                _mm_blend_ps(
                    _mm_shuffle_ps(v.array_sse, v.array_sse, _MM_SHUFFLE(1, 0, 1, 0)),
                    _vec4_zero_sse, 0xC),
                _mm_blend_ps(
                    _mm_shuffle_ps(v.array_sse, v.array_sse, _MM_SHUFFLE(3, 2, 3, 2)),
                    _vec4_zero_sse, 0xC),
                _vec4_0010_sse);
#endif
#elif defined(ITK_NEON)
            return typeMat3(
                (float32x4_t){v.a1, v.a2, 0, 0},
                (float32x4_t){v.b1, v.b2, 0, 0},
                _neon_0010);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        /// \brief Build a 3D matrix from a 4D matrix.
        ///
        /// Extracts the upper-left 3x3 block of a 4x4 matrix.
        ///
        /// Example:
        ///
        /// \code
        /// mat3<float, SIMD_TYPE::NONE> m;
        /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::fromMat4(mat4<float, SIMD_TYPE::NONE>());
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param v The 4x4 matrix to extract from.
        /// \return A 3x3 matrix containing the upper-left 3x3 block.
        ///
        static ITK_INLINE typeMat3 fromMat4(const typeMat4 &v) noexcept
        {
#if defined(ITK_SSE2)
            return typeMat3(
                v.array_sse[0],
                v.array_sse[1],
                v.array_sse[2]);
#elif defined(ITK_NEON)
            return typeMat3(
                v.array_neon[0],
                v.array_neon[1],
                v.array_neon[2]);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }
    };

}