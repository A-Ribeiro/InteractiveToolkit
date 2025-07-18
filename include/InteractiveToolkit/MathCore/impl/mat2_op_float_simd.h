#pragma once

#if !defined(ITK_SSE2) && !defined(ITK_NEON)
#error Invalid header 'mat2_op_float_simd.h' included. \
        Need at least one of the following build flags set: \
        ITK_SSE2, ITK_NEON
#endif

#include "simd_common.h"

#include "mat2_base.h"

#include "../cvt.h"
#include "../operator_overload.h"

#include "quat_op.h"
#include "vec4_op_float_simd.h"

namespace MathCore
{

    template <typename _type, typename _simd, typename _algorithm>
    struct OP<mat2<_type, _simd>,
              typename std::enable_if<
                  std::is_same<_type, float>::value &&
                  (std::is_same<_simd, SIMD_TYPE::SSE>::value ||
                   std::is_same<_simd, SIMD_TYPE::NEON>::value)>::type,
              _algorithm>
    {
    private:
        using typeMat2 = mat2<_type, _simd>;
        using type2 = vec2<_type, _simd>;
        using type4 = vec4<_type, _simd>;
        using self_type = OP<typeMat2>;

    public:
        static ITK_INLINE typeMat2 clamp(const typeMat2 &value, const typeMat2 &min, const typeMat2 &max) noexcept
        {
#if defined(ITK_SSE2)
            return clamp_sse_4(value.array_sse, min.array_sse, max.array_sse);
#elif defined(ITK_NEON)
            return clamp_neon_4(value.array_neon, min.array_neon, max.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE _type dot(const typeMat2 &a, const typeMat2 &b) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_f32_read_0(dot_sse_4(a.array_sse, b.array_sse));
#elif defined(ITK_NEON)
            return vgetq_lane_f32(dot_neon_4(a.array_neon, b.array_neon), 0);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE typeMat2 normalize(const typeMat2 &m) noexcept
        {
#if defined(ITK_SSE2)
            __m128 mag2 = dot_sse_4(m.array_sse, m.array_sse);
            _type mag2_rsqrt = OP<_type, void, _algorithm>::rsqrt(_mm_f32_read_0(mag2));
            __m128 magInv = _mm_set1_ps(mag2_rsqrt);
            return _mm_mul_ps(m.array_sse, magInv);
#elif defined(ITK_NEON)
            float32x4_t mag2 = dot_neon_4(m.array_neon, m.array_neon);
            _type mag2_rsqrt = OP<_type, void, _algorithm>::rsqrt(vgetq_lane_f32(mag2, 0));
            float32x4_t magInv = vset1(mag2_rsqrt);
            return vmulq_f32(m.array_neon, magInv);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
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
#if defined(ITK_SSE2)
            return _mm_f32_read_0(max_sse_4(a.array_sse));
#elif defined(ITK_NEON)
            float32x4_t max_neon = vmaxq_f32(a.array_neon, vshuffle_1032(a.array_neon));
            max_neon = vmaxq_f32(max_neon, vshuffle_1111(max_neon));
            return vgetq_lane_f32(max_neon, 0);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE typeMat2 maximum(const typeMat2 &a, const typeMat2 &b) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_max_ps(a.array_sse, b.array_sse);
#elif defined(ITK_NEON)
            return vmaxq_f32(a.array_neon, b.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE _type minimum(const typeMat2 &a) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_f32_read_0(min_sse_4(a.array_sse));
#elif defined(ITK_NEON)
            float32x4_t min_neon = vminq_f32(a.array_neon, vshuffle_1032(a.array_neon));
            min_neon = vminq_f32(min_neon, vshuffle_1111(min_neon));
            return vgetq_lane_f32(min_neon, 0);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE typeMat2 minimum(const typeMat2 &a, const typeMat2 &b) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_min_ps(a.array_sse, b.array_sse);
#elif defined(ITK_NEON)
            return vminq_f32(a.array_neon, b.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE typeMat2 abs(const typeMat2 &a) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_andnot_ps(_vec4_sign_mask_sse, a.array_sse);
#elif defined(ITK_NEON)
            return vabsq_f32(a.array_neon);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
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
            return (type2)m[0];
        }

        static ITK_INLINE type2 extractYaxis(const typeMat2 &m) noexcept
        {
            return (type2)m[1];
        }

        static ITK_INLINE typeMat2 transpose(const typeMat2 &m) noexcept
        {
#if defined(ITK_SSE2)
            return _mm_shuffle_ps(m.array_sse, m.array_sse, _MM_SHUFFLE(3, 1, 2, 0));
#elif defined(ITK_NEON)
            return vshuffle_3120(m.array_neon);
            // return typeMat2(m.a1, m.a2,
            //                 m.b1, m.b2);
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
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
#if defined(ITK_SSE2)
            __m128 sign_aux = _mm_and_ps(m.array_sse, _vec4_sign_mask_sse);
            __m128 sign = _mm_or_ps(sign_aux, _vec4_one_sse);
            return sign;
#elif defined(ITK_NEON)
            // const int32x4_t v4_sign_mask_i = vdupq_n_s32(0x80000000);
            // const int32x4_t _vec4_one_i = vreinterpretq_s32_f32(vdupq_n_f32(1.0f));

            int32x4_t sign_aux = vreinterpretq_s32_f32(m.array_neon);
            sign_aux = vandq_s32(sign_aux, v4_sign_mask_i);
            int32x4_t sign = vorrq_s32(sign_aux, _vec4_one_i);

            return vreinterpretq_f32_s32(sign);

            // return (float32x4_t){
            //     OP<_type>::sign(m.array_neon[0]),
            //     OP<_type>::sign(m.array_neon[1]),
            //     OP<_type>::sign(m.array_neon[2]),
            //     OP<_type>::sign(m.array_neon[3])};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE typeMat2 floor(const typeMat2 &m) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            return _sse2_mm_floor_ps(m.array_sse);
#else
            return _mm_floor_ps(m.array_sse);
#endif
#elif defined(ITK_NEON)
#if defined(__aarch64__)
            return vrndmq_f32(m.array_neon);
#else
            return _neon_mm_floor_ps(m.array_neon);
            // return (float32x4_t){
            //     OP<_type>::floor(m.a1),
            //     OP<_type>::floor(m.a2),
            //     OP<_type>::floor(m.b1),
            //     OP<_type>::floor(m.b2)};
#endif
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE typeMat2 ceil(const typeMat2 &m) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            return _sse2_mm_ceil_ps(m.array_sse);
#else
            return _mm_ceil_ps(m.array_sse);
#endif
#elif defined(ITK_NEON)
#if defined(__aarch64__)
            return vrndpq_f32(m.array_neon);
#else
            return _neon_mm_ceil_ps(m.array_neon);
            // return (float32x4_t){
            //     OP<_type>::ceil(m.a1),
            //     OP<_type>::ceil(m.a2),
            //     OP<_type>::ceil(m.b1),
            //     OP<_type>::ceil(m.b2)};
#endif
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE typeMat2 round(const typeMat2 &m) noexcept
        {
#if defined(ITK_SSE2)
#if defined(ITK_SSE_SKIP_SSE41)
            return _sse2_mm_round_ps(m.array_sse);
#else
            return _mm_round_ps(m.array_sse, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
#endif
#elif defined(ITK_NEON)
#if defined(__aarch64__)
            return vrndnq_f32(m.array_neon);
#else
            return _neon_mm_round_ps(m.array_neon);
            // return (float32x4_t){
            //     OP<_type>::round(m.a1),
            //     OP<_type>::round(m.a2),
            //     OP<_type>::round(m.b1),
            //     OP<_type>::round(m.b2)};
#endif
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE typeMat2 fmod(const typeMat2 &a, const typeMat2 &b) noexcept
        {
#if defined(ITK_SSE2)

            // float f = (a / b);
            __m128 f = _mm_div_ps(a.array_sse, b.array_sse);

            // float r = (float)(int)f;
            __m128 r = _mm_cvtepi32_ps(_mm_cvttps_epi32(f));

            // two possible values:
            // - 8388608.f (23bits)
            // - 2147483648.f (31bits)
            // Any value greater than this, will have integral mantissa...
            // and no decimal part
            //
            // uint32_t &r_uint = *(uint32_t *)&r;
            // uint32_t mask = -(int)(8388608.f > OP<float>::abs(f));
            // r_uint = r_uint & mask;

            // if ((abs(f) > 2**31 )) r = f;
            const __m128 _sign_bit = _mm_set1_ps(-0.f);
            const __m128 _max_f = _mm_set1_ps(8388608.f);
            __m128 m = _mm_cmpgt_ps(_max_f, _mm_andnot_ps(_sign_bit, f));
            r = _mm_and_ps(m, r);

            // return a - r * b;
            __m128 result = _mm_mul_ps(r, b.array_sse);

            result = _mm_sub_ps(a.array_sse, result);
            return result;

#elif defined(ITK_NEON)
            // return type4(
            //     OP<_type>::fmod(a.x, b.x),
            //     OP<_type>::fmod(a.y, b.y),
            //     OP<_type>::fmod(a.z, b.z),
            //     OP<_type>::fmod(a.w, b.w));

            // float f = (a / b);
            float32x4_t f = vdivq_f32(a.array_neon, b.array_neon);

            // float r = (float)(int)f;
            uint32x4_t r = vreinterpretq_u32_f32(vcvtq_f32_s32(vcvtq_s32_f32(f)));

            // two possible values:
            // - 8388608.f (23bits)
            // - 2147483648.f (31bits)
            // Any value greater than this, will have integral mantissa...
            // and no decimal part
            //
            // uint32_t &r_uint = *(uint32_t *)&r;
            // uint32_t mask = -(int)(8388608.f > OP<float>::abs(f));
            // r_uint = r_uint & mask;

            // if ((abs(f) > 2**31 )) r = f;
            // const __m128 _sign_bit = _mm_set1_ps(-0.f);
            const float32x4_t _max_f = vdupq_n_f32(8388608.f);
            uint32x4_t m = vcgtq_f32(_max_f, vabsq_f32(f));
            r = vandq_u32(m, r);

            // return a - r * b;
            float32x4_t result = vmulq_f32(vreinterpretq_f32_u32(r), b.array_neon);

            result = vsubq_f32(a.array_neon, result);
            return result;
            // return (float32x4_t){
            //     OP<_type>::fmod(a.a1, b.a1),
            //     OP<_type>::fmod(a.a2, b.a2),
            //     OP<_type>::fmod(a.b1, b.b1),
            //     OP<_type>::fmod(a.b2, b.b2)};
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
        }

        static ITK_INLINE typeMat2 step(const typeMat2 &threshould, const typeMat2 &v) noexcept
        {
#if defined(ITK_SSE2)
            __m128 _cmp = _mm_cmpge_ps(v.array_sse, threshould.array_sse);
            __m128 _rc = _mm_and_ps(_cmp, _vec4_one_sse);
            return _rc;

            // typeMat2 _sub = v - threshould;
            // typeMat2 _sign = self_type::sign(_sub);
            // return self_type::maximum(_sign, _vec4_zero_sse).array_sse;
#elif defined(ITK_NEON)
            uint32x4_t _cmp = vcgeq_f32(v.array_neon, threshould.array_neon);
            uint32x4_t _rc = vandq_u32(_cmp, _vec4_one_u);
            return vreinterpretq_f32_u32(_rc);

            // typeMat2 _sub = v - threshould;
            // typeMat2 _sign = self_type::sign(_sub);
            // return self_type::maximum(_sign, _vec4_zero).array_neon;
#else
#error Missing ITK_SSE2 or ITK_NEON compile option
#endif
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