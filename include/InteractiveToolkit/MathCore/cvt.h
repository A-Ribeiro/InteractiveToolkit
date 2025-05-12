#pragma once

#include "math_common.h"

#include "int.h"
#include "float.h"

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

#include "quat.h"

#include "mat2.h"
#include "mat3.h"
#include "mat4.h"

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

namespace MathCore
{

    template <>
    struct CVT<float>
    {
        // smart float to double conversion
        static ITK_INLINE double toDouble(float v) noexcept
        {
            char aux[32];
            snprintf(aux, 32, "%.6e\n", v);
            double result;
            if (sscanf(aux, "%le", &result) != 1)
                result = (double)v;
            return result;
        }
        static ITK_INLINE float toFloat(float v) noexcept
        {
            return v;
        }

        static ITK_INLINE float SRGBToLinear(float c)
        {
            return (c < 0.04045f) ? c * 0.0773993808f : OP<float>::pow(c * 0.9478672986f + 0.0521327014f, 2.4f);
        }

        static ITK_INLINE float LinearToSRGB(float c)
        {
            return (c < 0.0031308f) ? c * 12.92f : 1.055f * (OP<float>::pow(c, 0.41666f)) - 0.055f;
        }
    };

    template <>
    struct CVT<double>
    {
        // smart float to double conversion
        static ITK_INLINE double toDouble(double v) noexcept
        {
            return v;
        }
        static ITK_INLINE float toFloat(double v) noexcept
        {
            char aux[32];
            snprintf(aux, 32, "%.6e\n", v);
            float result;
            if (sscanf(aux, "%e", &result) != 1)
                result = (float)v;
            return result;
        }

        static ITK_INLINE double SRGBToLinear(double c)
        {
            return (c < 0.04045) ? c * 0.0773993808 : OP<double>::pow(c * 0.9478672986 + 0.0521327014, 2.4);
        }

        static ITK_INLINE double LinearToSRGB(double c)
        {
            return (c < 0.0031308) ? c * 12.92 : 1.055 * (OP<double>::pow(c, 0.41666)) - 0.055;
        }
    };

    template <typename _type, typename _simd>
    struct CVT<vec4<_type, _simd>>
    // ,typename std::enable_if<
    //               std::is_same<_simd, SIMD_TYPE::NONE >::value>::type>
    {
        using type4 = vec4<_type, _simd>;
        using lower_type3 = vec3<_type, _simd>;
        using lower_type2 = vec2<_type, _simd>;
        using self_type = CVT<type4>;

        static ITK_INLINE lower_type3 toVec3(const type4 &v) noexcept
        {
            return lower_type3(*(const lower_type3 *)&v);
        }

        static ITK_INLINE lower_type3 toVec3_PerspDiv(const type4 &v) noexcept
        {
            return self_type::toVec3(v) * ((_type)1 / v.w);
        }

        static ITK_INLINE lower_type2 toVec2(const type4 &v) noexcept
        {
            return lower_type2(*(const lower_type2 *)&v);
        }

        static ITK_INLINE lower_type2 toVec2_PerspDiv(const type4 &v) noexcept
        {
            return self_type::toVec2(v) * ((_type)1 / v.w);
        }

        template <typename in_type = _type,
                  typename std::enable_if<
                      std::is_same<in_type, float>::value ||
                          std::is_same<in_type, double>::value,
                      bool>::type = true>
        static ITK_INLINE type4 sRGBToLinear(const type4 &v) noexcept
        {
            return type4(
                CVT<_type>::SRGBToLinear(v.x),
                CVT<_type>::SRGBToLinear(v.y),
                CVT<_type>::SRGBToLinear(v.z),
                v.a);
        }

        template <typename in_type = _type,
                  typename std::enable_if<
                      std::is_same<in_type, float>::value ||
                          std::is_same<in_type, double>::value,
                      bool>::type = true>
        static ITK_INLINE type4 LinearTosRGB(const type4 &v) noexcept
        {
            return type4(
                CVT<_type>::LinearToSRGB(v.x),
                CVT<_type>::LinearToSRGB(v.y),
                CVT<_type>::LinearToSRGB(v.z),
                v.a);
        }
    };

    template <typename _type, typename _simd>
    struct CVT<vec3<_type, _simd>>
    // ,typename std::enable_if<
    //               std::is_same<_simd, SIMD_TYPE::NONE >::value>::type>
    {
        using type3 = vec3<_type, _simd>;
        using upper_type4 = vec4<_type, _simd>;
        using lower_type2 = vec2<_type, _simd>;
        using self_type = CVT<type3>;

        static ITK_INLINE upper_type4 toVec4(const type3 &v) noexcept
        {
            return upper_type4(v, 0);
        }

        static ITK_INLINE upper_type4 toPtn4(const type3 &v) noexcept
        {
            return upper_type4(v, (_type)1);
        }

        static ITK_INLINE lower_type2 toVec2(const type3 &v) noexcept
        {
            return lower_type2(*(const lower_type2 *)&v);
        }

        template <typename in_type = _type,
                  typename std::enable_if<
                      std::is_same<in_type, float>::value ||
                          std::is_same<in_type, double>::value,
                      bool>::type = true>
        static ITK_INLINE type3 sRGBToLinear(const type3 &v) noexcept
        {
            return type3(
                CVT<_type>::SRGBToLinear(v.x),
                CVT<_type>::SRGBToLinear(v.y),
                CVT<_type>::SRGBToLinear(v.z));
        }

        template <typename in_type = _type,
                  typename std::enable_if<
                      std::is_same<in_type, float>::value ||
                          std::is_same<in_type, double>::value,
                      bool>::type = true>
        static ITK_INLINE type3 LinearTosRGB(const type3 &v) noexcept
        {
            return type3(
                CVT<_type>::LinearToSRGB(v.x),
                CVT<_type>::LinearToSRGB(v.y),
                CVT<_type>::LinearToSRGB(v.z));
        }
    };

    template <typename _type, typename _simd>
    struct CVT<vec2<_type, _simd>>
    // ,typename std::enable_if<
    //               std::is_same<_simd, SIMD_TYPE::NONE >::value>::type>
    {
        using type2 = vec2<_type, _simd>;
        using upper_type4 = vec4<_type, _simd>;
        using upper_type3 = vec3<_type, _simd>;
        using self_type = CVT<type2>;

        static ITK_INLINE upper_type4 toVec4(const type2 &v) noexcept
        {
            return upper_type4(v, type2(0, 0));
        }

        static ITK_INLINE upper_type4 toPtn4(const type2 &v) noexcept
        {
            return upper_type4(v, type2(0, (_type)1));
        }

        static ITK_INLINE upper_type3 toVec3(const type2 &v) noexcept
        {
            return upper_type3(v, 0);
        }

        static ITK_INLINE upper_type3 toPtn3(const type2 &v) noexcept
        {
            return upper_type3(v, (_type)1);
        }
    };

    template <typename _type, typename _simd>
    struct CVT<mat4<_type, _simd>>
    // ,typename std::enable_if<
    //               std::is_same<_simd, SIMD_TYPE::NONE >::value>::type>
    {
        using typeMat4 = mat4<_type, _simd>;
        using lower_typeMat3 = mat3<_type, _simd>;
        using lower_typeMat2 = mat2<_type, _simd>;
        using lower_type3 = vec3<_type, _simd>;
        using lower_type2 = vec2<_type, _simd>;
        using quatT = quat<_type, _simd>;
        using self_type = CVT<typeMat4>;

        // static ITK_INLINE lower_typeMat3 toMat3(const typeMat4 &v) noexcept
        // {
        //     return lower_typeMat3(
        //         *(const lower_type3 *)&(v[0]),
        //         *(const lower_type3 *)&(v[1]),
        //         *(const lower_type3 *)&(v[2]));
        // }

        // static ITK_INLINE lower_typeMat2 toMat2(const typeMat4 &v) noexcept
        // {
        //     return lower_typeMat2(
        //         *(const lower_type2 *)&(v[0]),
        //         *(const lower_type2 *)&(v[1]));
        // }

        // // original: extractQuat
        // static ITK_INLINE quatT toQuat(const typeMat4 &mp) noexcept
        // {
        // 	typeMat4 m = mp;

        // 	// normalize rotation part
        // 	*(lower_type3 *)&(m[0]) = OP<lower_type3>::normalize(*(lower_type3 *)&(m[0]));
        // 	*(lower_type3 *)&(m[1]) = OP<lower_type3>::normalize(*(lower_type3 *)&(m[1]));
        // 	*(lower_type3 *)&(m[2]) = OP<lower_type3>::normalize(*(lower_type3 *)&(m[2]));

        // 	_type t = (_type)1 + m.a1 + m.b2 + m.c3;
        // 	// large enough
        // 	if (t > (_type)0.001)
        // 	{
        // 		_type s = OP<_type>::sqrt(t) * (_type)2;
        // 		return quatT((m.b3 - m.c2) / s,
        // 					 (m.c1 - m.a3) / s,
        // 					 (m.a2 - m.b1) / s,
        // 					 (_type)0.25 * s);
        // 	} // else we have to check several cases
        // 	else if (m.a1 > m.b2 && m.a1 > m.c3)
        // 	{
        // 		// Column 0:
        // 		_type s = OP<_type>::sqrt((_type)1 + m.a1 - m.b2 - m.c3) * (_type)2;
        // 		return quatT((_type)0.25 * s,
        // 					 (m.a2 + m.b1) / s,
        // 					 (m.c1 + m.a3) / s,
        // 					 (m.b3 - m.c2) / s);
        // 	}
        // 	else if (m.b2 > m.c3)
        // 	{
        // 		// Column 1:
        // 		_type s = OP<_type>::sqrt((_type)1 + m.b2 - m.a1 - m.c3) * (_type)2;
        // 		return quatT((m.a2 + m.b1) / s,
        // 					 (_type)0.25 * s,
        // 					 (m.b3 + m.c2) / s,
        // 					 (m.c1 - m.a3) / s);
        // 	}
        // 	else
        // 	{
        // 		// Column 2:
        // 		_type s = OP<_type>::sqrt((_type)1 + m.c3 - m.a1 - m.b2) * (_type)2;
        // 		return quatT((m.c1 + m.a3) / s,
        // 					 (m.b3 + m.c2) / s,
        // 					 (_type)0.25 * s,
        // 					 (m.a2 - m.b1) / s);
        // 	}
        // }
    };

    template <typename _type, typename _simd>
    struct CVT<mat3<_type, _simd>>
    // ,typename std::enable_if<
    //               std::is_same<_simd, SIMD_TYPE::NONE >::value>::type>
    {
        using typeMat3 = mat3<_type, _simd>;
        using upper_typeMat4 = mat4<_type, _simd>;
        using lower_typeMat2 = mat2<_type, _simd>;
        using upper_type4 = vec4<_type, _simd>;
        using lower_type2 = vec2<_type, _simd>;
        using type3 = vec3<_type, _simd>;
        using quatT = quat<_type, _simd>;
        using self_type = CVT<typeMat3>;

        // static ITK_INLINE upper_typeMat4 toMat4(const typeMat3 &v) noexcept
        // {
        //     return upper_typeMat4(
        //         upper_type4(v[0], 0),
        //         upper_type4(v[1], 0),
        //         upper_type4(v[2], 0),
        //         upper_type4(0, 0, 0, (_type)1));
        // }

        // static ITK_INLINE lower_typeMat2 toMat2(const typeMat3 &v) noexcept
        // {
        //     return lower_typeMat2(
        //         *(const lower_type2 *)&(v[0]),
        //         *(const lower_type2 *)&(v[1]));
        // }

        // // original: extractQuat
        // static ITK_INLINE quatT toQuat(const typeMat3 &mp) noexcept
        // {
        // 	typeMat3 m = mp;

        // 	// normalize rotation part
        // 	m[0] = OP<type3>::normalize(m[0]);
        // 	m[1] = OP<type3>::normalize(m[1]);
        // 	m[2] = OP<type3>::normalize(m[2]);

        // 	_type t = (_type)1 + m.a1 + m.b2 + m.c3;
        // 	// large enough
        // 	if (t > (_type)0.001)
        // 	{
        // 		_type s = OP<_type>::sqrt(t) * (_type)2;
        // 		return quatT((m.b3 - m.c2) / s,
        // 					 (m.c1 - m.a3) / s,
        // 					 (m.a2 - m.b1) / s,
        // 					 (_type)0.25 * s);
        // 	} // else we have to check several cases
        // 	else if (m.a1 > m.b2 && m.a1 > m.c3)
        // 	{
        // 		// Column 0:
        // 		_type s = OP<_type>::sqrt((_type)1 + m.a1 - m.b2 - m.c3) * (_type)2;
        // 		return quatT((_type)0.25 * s,
        // 					 (m.a2 + m.b1) / s,
        // 					 (m.c1 + m.a3) / s,
        // 					 (m.b3 - m.c2) / s);
        // 	}
        // 	else if (m.b2 > m.c3)
        // 	{
        // 		// Column 1:
        // 		_type s = OP<_type>::sqrt((_type)1 + m.b2 - m.a1 - m.c3) * (_type)2;
        // 		return quatT((m.a2 + m.b1) / s,
        // 					 (_type)0.25 * s,
        // 					 (m.b3 + m.c2) / s,
        // 					 (m.c1 - m.a3) / s);
        // 	}
        // 	else
        // 	{
        // 		// Column 2:
        // 		_type s = OP<_type>::sqrt((_type)1 + m.c3 - m.a1 - m.b2) * (_type)2;
        // 		return quatT((m.c1 + m.a3) / s,
        // 					 (m.b3 + m.c2) / s,
        // 					 (_type)0.25 * s,
        // 					 (m.a2 - m.b1) / s);
        // 	}
        // }
    };

    template <typename _type, typename _simd>
    struct CVT<mat2<_type, _simd>>
    // ,typename std::enable_if<
    //               std::is_same<_simd, SIMD_TYPE::NONE >::value>::type>
    {
        using typeMat2 = mat2<_type, _simd>;
        using upper_typeMat4 = mat4<_type, _simd>;
        using upper_typeMat3 = mat3<_type, _simd>;
        using upper_type4 = vec4<_type, _simd>;
        using upper_type3 = vec3<_type, _simd>;
        using type2 = vec2<_type, _simd>;
        using self_type = CVT<typeMat2>;

        // static ITK_INLINE upper_typeMat4 toMat4(const typeMat2 &v) noexcept
        // {
        //     return upper_typeMat4(
        //         upper_type4(v.a1, v.a2, 0, 0),
        //         upper_type4(v.b1, v.b2, 0, 0),
        //         upper_type4(0, 0, (_type)1, 0),
        //         upper_type4(0, 0, 0, (_type)1));
        // }

        // static ITK_INLINE upper_typeMat3 toMat3(const typeMat2 &v) noexcept
        // {
        //     return lower_typeMat3(
        //         upper_type3(v[0], 0),
        //         upper_type3(v[1], 0),
        //         upper_type3(0, 0, (_type)1));
        // }
    };

    template <typename _type, typename _simd>
    struct CVT<quat<_type, _simd>>
    // ,typename std::enable_if<
    //               std::is_same<_simd, SIMD_TYPE::NONE >::value>::type>
    {
        using quatT = quat<_type, _simd>;
        using typeMat3 = mat3<_type, _simd>;
        using typeMat4 = mat4<_type, _simd>;
        using self_type = CVT<quatT>;

        // static ITK_INLINE typeMat4 toMat4(const quatT &q) noexcept
        // {

        //     _type x2 = q.x * q.x;
        //     _type y2 = q.y * q.y;
        //     _type z2 = q.z * q.z;
        //     _type xy = q.x * q.y;
        //     _type xz = q.x * q.z;
        //     _type yz = q.y * q.z;
        //     _type wx = q.w * q.x;
        //     _type wy = q.w * q.y;
        //     _type wz = q.w * q.z;

        //     // This calculation would be a lot more complicated for non-unit length quaternions
        //     // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
        //     //   OpenGL
        //     return typeMat4((_type)1 - (_type)2 * (y2 + z2), (_type)2 * (xy - wz), (_type)2 * (xz + wy), 0,
        //                     (_type)2 * (xy + wz), (_type)1 - (_type)2 * (x2 + z2), (_type)2 * (yz - wx), 0,
        //                     (_type)2 * (xz - wy), (_type)2 * (yz + wx), (_type)1 - (_type)2 * (x2 + y2), 0,
        //                     0, 0, 0, 1);
        // }

        // static ITK_INLINE typeMat3 toMat3(const quatT &q) noexcept
        // {
        //     _type x2 = q.x * q.x;
        //     _type y2 = q.y * q.y;
        //     _type z2 = q.z * q.z;
        //     _type xy = q.x * q.y;
        //     _type xz = q.x * q.z;
        //     _type yz = q.y * q.z;
        //     _type wx = q.w * q.x;
        //     _type wy = q.w * q.y;
        //     _type wz = q.w * q.z;

        //     // This calculation would be a lot more complicated for non-unit length quaternions
        //     // Note: The constructor of Matrix4 expects the Matrix in column-major format like expected by
        //     //   OpenGL
        //     return typeMat3((_type)1 - (_type)2 * (y2 + z2), (_type)2 * (xy - wz), (_type)2 * (xz + wy),
        //                     (_type)2 * (xy + wz), (_type)1 - (_type)2 * (x2 + z2), (_type)2 * (yz - wx),
        //                     (_type)2 * (xz - wy), (_type)2 * (yz + wx), (_type)1 - (_type)2 * (x2 + y2));
        // }
    };

}

#if defined(_WIN32)
#pragma warning(pop)
#endif
