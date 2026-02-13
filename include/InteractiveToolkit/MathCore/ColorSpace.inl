#pragma once

#include "../ITKCommon/ITKCommon.h"

namespace MathCore
{
    namespace ColorSpace
    {

        static ITK_INLINE int32_t optimized_int_maximum(int32_t a, int32_t b, int32_t c)
        {
#if defined(ITK_SSE2)
            __m128i _a = _mm_set1_epi32(a);
            __m128i _b = _mm_set1_epi32(b);
            __m128i _c = _mm_set1_epi32(c);

            __m128i _min = _mm_max_epi32(_a, _b);
            _min = _mm_max_epi32(_min, _c);

            return _mm_i32_(_min, 0);
#elif defined(ITK_NEON)

            int32x2_t _a = (int32x2_t){a};
			int32x2_t _b = (int32x2_t){b};
			int32x2_t _c = (int32x2_t){c};

			int32x2_t _min = vmax_s32(_a, _b);
			_min = vmax_s32(_min, _c);

			return _min[0];
#else
            int32_t min = (a > b) ? a : b;
            min = (min > c) ? min : c;
            return min;
#endif
        }

        /// \brief Converts RGB to HSV
        ///
        /// RGB: Red Green Blue
        ///
        /// HSV: Hue Saturation Value
        ///
        /// The RGB are in the range 0..1
        ///
        /// The HSV uses the ranges: H = 0..360, S = 0..1, V = 0..1
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 rgb;
        ///
        /// vec3 hsv = FloatColorConversion::RGBtoHSV( rgb );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param rgb Red Green Blue
        /// \return Hue Saturation Value
        ///

        template <typename _vec3_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec3_>::_class, MathTypeClass::_class_vec3>::value && //
                          std::is_floating_point<typename MathTypeInfo<_vec3_>::_type>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE _vec3_ RGBtoHSV(const _vec3_ &rgb)
        {
            using _f_type = typename MathTypeInfo<_vec3_>::_type;

            _vec3_ result;
            _f_type min = OP<_vec3_>::minimum(rgb);
            _f_type max = OP<_vec3_>::maximum(rgb);

            _f_type r = rgb.r, g = rgb.g, b = rgb.b;

            _f_type delta = max - min;
            if (delta == (_f_type)0)
            {
                result.x = 0;
            }
            else if (max == r && g >= b)
            {
                result.x = (_f_type)60 * ((g - b) / delta) + (_f_type)0;
            }
            else if (max == r && g < b)
            {
                result.x = (_f_type)60 * ((g - b) / delta) + (_f_type)360;
            }
            else if (max == g)
            {
                result.x = (_f_type)60 * ((b - r) / delta) + (_f_type)120;
            }
            else if (max == b)
            {
                result.x = (_f_type)60 * ((r - g) / delta) + (_f_type)240;
            }
            if (max == 0)
                result.y = (_f_type)0;
            else
                result.y = (max - min) / max;
            result.z = max;
            return result;
        }

        /// \brief Converts HSV to RGB
        ///
        /// HSV: Hue Saturation Value
        ///
        /// RGB: Red Green Blue
        ///
        /// The HSV uses the ranges: H = 0..360, S = 0..1, V = 0..1
        ///
        /// The RGB are in the range 0..1
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 hsv;
        ///
        /// vec3 rgb = FloatColorConversion::HSVtoRGB( hsv );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param hsv Hue Saturation Value
        /// \return Red Green Blue
        ///
        template <typename _vec3_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec3_>::_class, MathTypeClass::_class_vec3>::value && //
                          std::is_floating_point<typename MathTypeInfo<_vec3_>::_type>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE _vec3_ HSVtoRGB(const _vec3_ &hsv)
        {
            using _f_type = typename MathTypeInfo<_vec3_>::_type;

            _f_type h = hsv[0];
            _f_type s = hsv[1];
            _f_type v = hsv[2];

            int Hi = (int)(h / (_f_type)60) % 6;
            // float f = h / 60.0f - Hi;
            _f_type f = OP<_f_type>::fmod(h / (_f_type)60, (_f_type)1);
            _f_type p = v * ((_f_type)1 - s);
            _f_type q = v * ((_f_type)1 - f * s);
            _f_type t = v * ((_f_type)1 - ((_f_type)1 - f) * s);

            if (Hi == 0)
                return _vec3_(v, t, p);
            else if (Hi == 1)
                return _vec3_(q, v, p);
            else if (Hi == 2)
                return _vec3_(p, v, t);
            else if (Hi == 3)
                return _vec3_(p, q, v);
            else if (Hi == 4)
                return _vec3_(t, p, v);
            else if (Hi == 5)
                return _vec3_(v, p, q);

            return _vec3_();
        }

        /// \brief Converts RGB to CIE 1931 color space
        ///
        /// RGB: Red Green Blue
        ///
        /// CIE: Commission internationale de l´Eclairage
        ///
        /// The RGB are in the range 0..1
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 rgb;
        ///
        /// vec3 cie = FloatColorConversion::RGBtoCIE( rgb );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param rgb Red Green Blue
        /// \return cie 1931 color space
        ///
        template <typename _vec3_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec3_>::_class, MathTypeClass::_class_vec3>::value && //
                          std::is_floating_point<typename MathTypeInfo<_vec3_>::_type>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE _vec3_ RGBtoCIE(const _vec3_ &rgb)
        {
            using _f_type = typename MathTypeInfo<_vec3_>::_type;
            using _f_simd = typename MathTypeInfo<_vec3_>::_simd;

            using _mat3_ = mat3<_f_type, _f_simd>;

            const _mat3_ m(
                (_f_type)0.49 / (_f_type)0.17697, (_f_type)0.31 / (_f_type)0.17697, (_f_type)0.2 / (_f_type)0.17697,
                (_f_type)0.17697 / (_f_type)0.17697, (_f_type)0.8124 / (_f_type)0.17697, (_f_type)0.01063 / (_f_type)0.17697,
                0, (_f_type)0.01 / (_f_type)0.17697, (_f_type)0.99 / (_f_type)0.17697);

            return m * rgb;

            // _vec3_ result;
            // result.x = rgb.r * 0.49f + rgb.g * 0.31f + rgb.b * 0.2f;
            // result.y = rgb.r * 0.17697f + rgb.g * 0.8124f + rgb.b * 0.01063f;
            // result.z = rgb.g * 0.01f + rgb.b * 0.99f;
            // result *= (1.0f / 0.17697f);
            // return result;
        }

        /// \brief Converts CIE 1931 color space to RGB
        ///
        /// CIE: Commission internationale de l´Eclairage
        ///
        /// RGB: Red Green Blue
        ///
        /// The RGB are in the range 0..1
        ///
        /// Example:
        ///
        /// \code
        ///
        /// vec3 cie;
        ///
        /// vec3 rgb = FloatColorConversion::RGBtoCIE( cie );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param cie cie 1931 color space
        /// \return Red Green Blue
        ///
        template <typename _vec3_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec3_>::_class, MathTypeClass::_class_vec3>::value && //
                          std::is_floating_point<typename MathTypeInfo<_vec3_>::_type>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE vec3f CIEtoRGB(const vec3f &cie)
        {
            using _f_type = typename MathTypeInfo<_vec3_>::_type;
            using _f_simd = typename MathTypeInfo<_vec3_>::_simd;

            using _mat3_ = mat3<_f_type, _f_simd>;

            const _mat3_ m(
                (_f_type)0.41847f, (_f_type)-0.15866, (_f_type)-0.082835,
                (_f_type)-0.091169, (_f_type)0.25243, (_f_type)0.015708,
                (_f_type)0.00092090f, (_f_type)-0.0025498, (_f_type)0.1786);

            return m * cie;

            // vec3f result;
            // result.r = cie.x * 0.41847f - cie.y * 0.15866f - cie.z * 0.082835f;
            // result.g = -cie.x * 0.091169f + cie.y * 0.25243f + cie.z * 0.015708f;
            // result.b = cie.x * 0.00092090f - cie.y * 0.0025498f + cie.z * 0.1786f;
            // return result;
        }

        /// \brief Converts RGB to CMY
        ///
        /// RGB: Red Green Blue
        ///
        /// CMY: Cian Magenta Yellow
        ///
        /// The RGB are in the range 0..255
        ///
        /// The CMY are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        ///
        /// uint8_t r, g, b;
        /// uint8_t c, m, y;
        ///
        /// UByteColorConversion::RGBtoCMY( r, g, b, &c, &m, &y );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r Red
        /// \param g Green
        /// \param b Blue
        /// \param c **return** Cian
        /// \param m **return** Magenta
        /// \param y **return** Yellow
        ///
        template <typename _vec3_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec3_>::_class, MathTypeClass::_class_vec3>::value && //
                          std::is_same<typename MathTypeInfo<_vec3_>::_type, uint8_t>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE _vec3_ RGBtoCMY(const _vec3_ &rgb)
        {
            return 255 - rgb;
        }

        template <typename _vec3_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec3_>::_class, MathTypeClass::_class_vec3>::value && //
                          std::is_floating_point<typename MathTypeInfo<_vec3_>::_type>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE _vec3_ RGBtoCMY(const _vec3_ &rgb)
        {
            using _f_type = typename MathTypeInfo<_vec3_>::_type;
            return (_f_type)1 - rgb;
        }

        /// \brief Converts CMY to RGB
        ///
        /// CMY: Cian Magenta Yellow
        ///
        /// RGB: Red Green Blue
        ///
        /// The CMY are in the range 0..255
        ///
        /// The RGB are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        ///
        /// uint8_t c, m, y;
        /// uint8_t r, g, b;
        ///
        /// UByteColorConversion::CMYtoRGB( c, m, y, &r, &g, &b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param c Cian
        /// \param m Magenta
        /// \param y Yellow
        /// \param r **return** Red
        /// \param g **return** Green
        /// \param b **return** Blue
        ///
        template <typename _vec3_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec3_>::_class, MathTypeClass::_class_vec3>::value && //
                          std::is_same<typename MathTypeInfo<_vec3_>::_type, uint8_t>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE _vec3_ CMYtoRGB(const _vec3_ &cmy)
        {
            return 255 - cmy;
        }

        template <typename _vec3_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec3_>::_class, MathTypeClass::_class_vec3>::value && //
                          std::is_floating_point<typename MathTypeInfo<_vec3_>::_type>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE _vec3_ CMYtoRGB(const _vec3_ &cmy)
        {
            using _f_type = typename MathTypeInfo<_vec3_>::_type;
            return (_f_type)1 - cmy;
        }

        /// \brief Converts RGB to CMYK
        ///
        /// RGB: Red Green Blue
        ///
        /// CMYK: Cian Magenta Yellow (Amount of Black)
        ///
        /// The RGB are in the range 0..255
        ///
        /// The CMYK are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        ///
        /// uint8_t r, g, b;
        /// uint8_t c, m, y, k;
        ///
        /// UByteColorConversion::RGBtoCMYK( r, g, b, &c, &m, &y, &k );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r Red
        /// \param g Green
        /// \param b Blue
        /// \param c **return** Cian
        /// \param m **return** Magenta
        /// \param y **return** Yellow
        /// \param k **return** Amount of black
        ///
        template <typename _vec3_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec3_>::_class, MathTypeClass::_class_vec3>::value && //
                          std::is_same<typename MathTypeInfo<_vec3_>::_type, uint8_t>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE vec4<typename MathTypeInfo<_vec3_>::_type,typename MathTypeInfo<_vec3_>::_simd> RGBtoCMYK(const _vec3_ &rgb)
        {
            using _vec4_ = vec4<typename MathTypeInfo<_vec3_>::_type,typename MathTypeInfo<_vec3_>::_simd>;

            int r = rgb.r;
            int g = rgb.g;
            int b = rgb.b;

            int gray = optimized_int_maximum(r, g, b);
            int kaux = (255 - gray);

            _vec4_ result;
            result[3] = kaux;
            if (gray != 0)
            {
                int cAux = ((255 - (int)r - kaux) * 255) / gray;
                int mAux = ((255 - (int)g - kaux) * 255) / gray;
                int yAux = ((255 - (int)b - kaux) * 255) / gray;
                result[0] = (uint8_t)cAux;
                result[1] = (uint8_t)mAux;
                result[2] = (uint8_t)yAux;
            }
            // else {
            //     result[0] = 0;
            //     result[1] = 0;
            //     result[2] = 0;
            // }

            return result;
        }

        /// \brief Converts CMYK to RGB
        ///
        /// CMYK: Cian Magenta Yellow (Amount of Black)
        ///
        /// RGB: Red Green Blue
        ///
        /// The CMYK are in the range 0..255
        ///
        /// The RGB are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        ///
        /// uint8_t c, m, y, k;
        /// uint8_t r, g, b;
        ///
        /// UByteColorConversion::CMYKtoRGB( c, m, y, k, &r, &g, &b );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param c Cian
        /// \param m Magenta
        /// \param y Yellow
        /// \param k Amount of black
        /// \param r **return** Red
        /// \param g **return** Green
        /// \param b **return** Blue
        ///
        template <typename _vec4_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec4_>::_class, MathTypeClass::_class_vec4>::value && //
                          std::is_same<typename MathTypeInfo<_vec4_>::_type, uint8_t>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE vec3<typename MathTypeInfo<_vec4_>::_type,typename MathTypeInfo<_vec4_>::_simd> CMYKtoRGB(const _vec4_ &cmyk)
        {
            using _vec3_ = vec3<typename MathTypeInfo<_vec4_>::_type,typename MathTypeInfo<_vec4_>::_simd>;

            uint8_t c = cmyk[0];
            uint8_t m = cmyk[1];
            uint8_t y = cmyk[2];
            uint8_t k = cmyk[3];

            int gray = 255 - k;
            int raux = ((255 - (int)c) * gray) / 255;
            int gaux = ((255 - (int)m) * gray) / 255;
            int baux = ((255 - (int)y) * gray) / 255;

            return _vec3_(
                (uint8_t)raux,
                (uint8_t)gaux,
                (uint8_t)baux);
        }

        /// \brief Converts RGB to YUV
        ///
        /// RGB: Red Green Blue
        ///
        /// YUV: luma component (Y) chrominance U (blue projection) chrominance V (red projection)
        ///
        /// The RGB are in the range 0..255
        ///
        /// The YUV are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        ///
        /// uint8_t r, g, b;
        /// uint8_t y, u, v;
        ///
        /// UByteColorConversion::RGBtoYUV( r, g, b, &y, &u, &v );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param r Red
        /// \param g Green
        /// \param b Blue
        /// \param y **return** Luma
        /// \param u **return** Blue Projection
        /// \param v **return** Red Projection
        ///
        template <typename _vec3_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec3_>::_class, MathTypeClass::_class_vec3>::value && //
                          std::is_same<typename MathTypeInfo<_vec3_>::_type, uint8_t>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE _vec3_ RGBtoYUV(const _vec3_ &rgb)
        {

            int ir = (int)rgb.r;
            int ig = (int)rgb.g;
            int ib = (int)rgb.b;

            uint8_t y = (uint8_t)(((66 * ir + 129 * ig + 25 * ib + 128) >> 8) + 16);
            uint8_t u = (uint8_t)(((-38 * ir - 74 * ig + 112 * ib + 128) >> 8) + 128);
            uint8_t v = (uint8_t)(((112 * ir - 94 * ig - 18 * ib + 128) >> 8) + 128);

            return _vec3_(y, u, v);
        }

        /// \brief Converts YUV to RGB
        ///
        /// YUV: luma component (Y) chrominance U (blue projection) chrominance V (red projection)
        ///
        /// RGB: Red Green Blue
        ///
        /// The YUV are in the range 0..255
        ///
        /// The RGB are in the range 0..255
        ///
        /// Example:
        ///
        /// \code
        ///
        /// uint8_t y, u, v;
        /// uint8_t r, g, b;
        ///
        /// UByteColorConversion::YUVtoRGB( y, u, v, &r, &g, &b, );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \param y Luma
        /// \param u Blue Projection
        /// \param v Red Projection
        /// \param r **return** Red
        /// \param g **return** Green
        /// \param b **return** Blue
        ///
        template <typename _vec3_,
                  typename std::enable_if<
                      std::is_same<typename MathTypeInfo<_vec3_>::_class, MathTypeClass::_class_vec3>::value && //
                          std::is_same<typename MathTypeInfo<_vec3_>::_type, uint8_t>::value                   //
                      ,
                      bool>::type = true>
        static ITK_INLINE _vec3_ YUVtoRGB(const _vec3_ &yuv)
        {

            int y = yuv[0];
            int u = yuv[1];
            int v = yuv[2];

            int c = y - 16;
            int d = u - 128;
            int e = v - 128;

            int rAux = (298 * c + 409 * e + 128) >> 8;
            int gAux = (298 * c - 100 * d - 208 * e + 128) >> 8;
            int bAux = (298 * c + 516 * d + 128) >> 8;

            rAux = OP<int>::clamp(rAux, 0, 255);
            gAux = OP<int>::clamp(gAux, 0, 255);
            bAux = OP<int>::clamp(bAux, 0, 255);

            uint8_t r = (uint8_t)rAux;
            uint8_t g = (uint8_t)gAux;
            uint8_t b = (uint8_t)bAux;

            return _vec3_(r, g, b);
        }

    }

}