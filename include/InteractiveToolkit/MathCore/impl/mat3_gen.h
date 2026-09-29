#pragma once

#include "mat3_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

namespace MathCore
{

    /// \brief Generation operations specialization for mat3 with no SIMD optimization.
    ///
    /// Provides generation (non-SIMD) utility functions for the mat3 class when
    /// SIMD optimizations are disabled (SIMD_TYPE::NONE). This specialization
    /// is selected via SFINAE when the _simd template parameter matches
    /// SIMD_TYPE::NONE.
    ///
    /// The static factory methods build common 3x3 matrices: homogeneous
    /// translations and scales, axis-aligned rotations, Euler-angle rotations,
    /// arbitrary axis-angle rotations, look-at rotation matrices, and
    /// conversions from quaternions and other matrix types.
    ///
    /// Example:
    ///
    /// \code
    /// mat3<float, SIMD_TYPE::NONE> m;
    /// m = GEN<mat3<float, SIMD_TYPE::NONE>>::translateHomogeneous(1.0f, 2.0f);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    /// \tparam _type The scalar type of the mat3 components (e.g., float, double).
    /// \tparam _simd The SIMD strategy type; this specialization is selected when
    ///         _simd is SIMD_TYPE::NONE.
    ///
    template <typename _type, typename _simd>
    struct GEN<mat3<_type, _simd>,
               typename std::enable_if<
                   std::is_same<_simd, SIMD_TYPE::NONE>::value>::type>
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
            return typeMat3(
                1, 0, _x_,
                0, 1, _y_,
                0, 0, 1);
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
            return typeMat3(
                1, 0, _v_.x,
                0, 1, _v_.y,
                0, 0, 1);
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
            return typeMat3(
                1, 0, _v_.x,
                0, 1, _v_.y,
                0, 0, 1);
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
            return typeMat3(
                1, 0, _v_.x,
                0, 1, _v_.y,
                0, 0, 1);
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
            return typeMat3(
                _x_, 0, 0,
                0, _y_, 0,
                0, 0, 1);
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
            return typeMat3(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, 1);
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
            return typeMat3(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, 1);
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
            return typeMat3(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, 1);
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
            return typeMat3(
                c, -s, 0,
                s, c, 0,
                0, 0, 1);
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
            return typeMat3(
                _x_, 0, 0,
                0, _y_, 0,
                0, 0, _z_);
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
            return typeMat3(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, 1);
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
            return typeMat3(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, _v_.z);
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
            return typeMat3(
                _v_.x, 0, 0,
                0, _v_.y, 0,
                0, 0, _v_.z);
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
            return typeMat3(
                1, 0, 0,
                0, c, -s,
                0, s, c);
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
            return typeMat3(
                c, 0, s,
                0, 1, 0,
                -s, 0, c);
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
            return typeMat3(
                c, -s, 0,
                s, c, 0,
                0, 0, 1);
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
            _type x2 = q.x * q.x;
            _type y2 = q.y * q.y;
            _type z2 = q.z * q.z;
            _type xy = q.x * q.y;
            _type xz = q.x * q.z;
            _type yz = q.y * q.z;
            _type wx = q.w * q.x;
            _type wy = q.w * q.y;
            _type wz = q.w * q.z;

            // This calculation would be a lot more complicated for non-unit length quaternions
            // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
            //   OpenGL
            return typeMat3((_type)1 - (_type)2 * (y2 + z2), (_type)2 * (xy - wz), (_type)2 * (xz + wy),
                            (_type)2 * (xy + wz), (_type)1 - (_type)2 * (x2 + z2), (_type)2 * (yz - wx),
                            (_type)2 * (xz - wy), (_type)2 * (yz + wx), (_type)1 - (_type)2 * (x2 + y2));
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
            return typeMat3(
                typeVec3(v[0], 0),
                typeVec3(v[1], 0),
                typeVec3(0, 0, (_type)1));
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
            return typeMat3(
                *(const typeVec3 *)&(v[0]),
                *(const typeVec3 *)&(v[1]),
                *(const typeVec3 *)&(v[2]));
        }

    };

}