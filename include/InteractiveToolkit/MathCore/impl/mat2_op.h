#pragma once

#include "mat2_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

#include "quat_op.h"

namespace MathCore
{

    template <typename _type, typename _simd, typename _algorithm>
    struct OP<mat2<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_simd, SIMD_TYPE::NONE>::value>::type,
              _algorithm>
    {
        private:
        using typeMat2 = mat2<_type, _simd>;
        using type2 = vec2<_type, _simd>;
        using self_type = OP<typeMat2>;
        public:

        static ITK_INLINE typeMat2 clamp(const typeMat2 &value, const typeMat2 &min, const typeMat2 &max) noexcept
        {
            return typeMat2(
                OP<type2>::clamp(value[0], min[0], max[0]),
                OP<type2>::clamp(value[1], min[1], max[1]));
        }

        static ITK_INLINE _type dot(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            _type dota = OP<type2>::dot(a[0], b[0]);
            _type dotb = OP<type2>::dot(a[1], b[1]);
            return dota + dotb;
        }

        static ITK_INLINE typeMat2 normalize(const typeMat2 &m) noexcept
        {
            _type mag2 = self_type::dot(m, m);
            _type mag2_rsqrt = OP<_type, void, _algorithm>::rsqrt(mag2);
            return m * mag2_rsqrt;
        }

        static ITK_INLINE _type sqrLength(const typeMat2 &a) noexcept
        {
            return self_type::dot(a, a);
        }

        static ITK_INLINE _type length(const typeMat2 &a) noexcept
        {
            return OP<_type>::sqrt(self_type::dot(a, a));
        }

        static ITK_INLINE _type sqrDistance(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            typeMat2 ab = b - a;
            return self_type::dot(ab, ab);
        }

        static ITK_INLINE _type distance(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            typeMat2 ab = b - a;
            return OP<_type>::sqrt(self_type::dot(ab, ab));
        }

        static ITK_INLINE _type maximum(const typeMat2 &a) noexcept
        {
            type2 max_a = OP<type2>::maximum(a[0], a[1]);
            return OP<type2>::maximum(max_a);
        }

        static ITK_INLINE typeMat2 maximum(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            return typeMat2(OP<type2>::maximum(a[0], b[0]),
                            OP<type2>::maximum(a[1], b[1]));
        }

        static ITK_INLINE _type minimum(const typeMat2 &a) noexcept
        {
            type2 max_a = OP<type2>::minimum(a[0], a[1]);
            return OP<type2>::minimum(max_a);
        }

        static ITK_INLINE typeMat2 minimum(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            return typeMat2(OP<type2>::minimum(a[0], b[0]),
                            OP<type2>::minimum(a[1], b[1]));
        }

        static ITK_INLINE typeMat2 abs(const typeMat2 &a) noexcept
        {
            return typeMat2(OP<type2>::abs(a[0]),
                            OP<type2>::abs(a[1]));
        }

        static ITK_INLINE typeMat2 lerp(const typeMat2 &a, const typeMat2 &b, const _type &factor) noexcept
        {
            //  return a+(b-a)*fator;
            return a * ((_type)1 - factor) + (b * factor);
        }

        static ITK_INLINE typeMat2 barylerp(const _type &u, const _type &v, const typeMat2 &v0, const typeMat2 &v1, const typeMat2 &v2) noexcept
        {
            // return v0*(1-uv[0]-uv[1])+v1*uv[0]+v2*uv[1];
            return v0 * ((_type)1 - u - v) + v1 * u + v2 * v;
        }

        static ITK_INLINE typeMat2 blerp(const typeMat2 &A, const typeMat2 &B, const typeMat2 &C, const typeMat2 &D,
                                          const _type &dx, const _type &dy) noexcept
        {
            _type omdx = (_type)1 - dx,
                  omdy = (_type)1 - dy;
            return (omdx * omdy) * A + (omdx * dy) * D + (dx * omdy) * B + (dx * dy) * C;
        }

        static ITK_INLINE type2 extractXaxis(const typeMat2 &m) noexcept
        {
            return m[0];
        }

        static ITK_INLINE type2 extractYaxis(const typeMat2 &m) noexcept
        {
            return m[1];
        }

        static ITK_INLINE typeMat2 transpose(const typeMat2 &m) noexcept
        {
            return typeMat2(m.a1, m.a2,
                            m.b1, m.b2);
        }

        static ITK_INLINE _type determinant(const typeMat2 &m) noexcept
        {
            return (m.a1 * m.b2 - m.b1 * m.a2);
        }

        static ITK_INLINE typeMat2 move(const typeMat2 &current, const typeMat2 &target, const _type &maxDistanceVariation) noexcept
        {
            _type deltaDistance = self_type::distance(current, target);

            deltaDistance = OP<_type>::maximum(deltaDistance, maxDistanceVariation);
            // avoid division by zero
            // deltaDistance = self_type::maximum(deltaDistance, EPSILON<float>::high_precision);
            deltaDistance = OP<_type>::maximum(deltaDistance, FloatTypeInfo<_type>::min);
            return self_type::lerp(current, target, maxDistanceVariation / deltaDistance);

            // if (deltaDistance < maxDistanceVariation + EPSILON<_type>::high_precision)
            //     return target;
            // return lerp(current, target, maxDistanceVariation / deltaDistance);
        }

        static ITK_INLINE typeMat2 sign(const typeMat2 &m) noexcept
        {
            return typeMat2(
                OP<type2>::sign(m[0]),
                OP<type2>::sign(m[1]));
        }

        static ITK_INLINE typeMat2 floor(const typeMat2 &m) noexcept
        {
            return typeMat2(
                OP<type2>::floor(m[0]),
                OP<type2>::floor(m[1]));
        }

        static ITK_INLINE typeMat2 ceil(const typeMat2 &m) noexcept
        {
            return typeMat2(
                OP<type2>::ceil(m[0]),
                OP<type2>::ceil(m[1]));
        }

        static ITK_INLINE typeMat2 round(const typeMat2 &m) noexcept
        {
            return typeMat2(
                OP<type2>::round(m[0]),
                OP<type2>::round(m[1]));
        }

        static ITK_INLINE typeMat2 fmod(const typeMat2 &a, const typeMat2 &b) noexcept
        {
            return typeMat2(
                OP<type2>::fmod(a[0], b[0]),
                OP<type2>::fmod(a[1], b[1]));
        }

        static ITK_INLINE typeMat2 step(const typeMat2 &threshould, const typeMat2 &v) noexcept
        {
            return typeMat2(
                OP<type2>::step(threshould[0], v[0]),
                OP<type2>::step(threshould[1], v[1]));
        }

        static ITK_INLINE typeMat2 smoothstep(const typeMat2 &edge0, const typeMat2 &edge1, const typeMat2 &x) noexcept
        {
            using type_info = FloatTypeInfo<_type>;
            typeMat2 dir = edge1 - edge0;

            _type length_dir = OP<_type>::maximum(self_type::length(dir), type_info::min);

            typeMat2 value = x - edge0;
            value *= (_type)1 / length_dir;

            typeMat2 t = self_type::clamp(value, typeMat2((_type)0), typeMat2((_type)1));
            return t * t * ((_type)3 - (_type)2 * t);
            // return typeMat2(
            //     OP<type2>::smoothstep(edge0[0], edge1[0], x[0]),
            //     OP<type2>::smoothstep(edge0[1], edge1[1], x[1]));
        }

    };

}