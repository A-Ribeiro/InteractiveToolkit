#pragma once

#include "../ITKCommon/ITKCommon.h"

namespace MathCore
{

    template <typename TRandom>
    class MathRandomExt
    {
    public:
        TRandom *random;

        MathRandomExt(TRandom *_rnd_)
        {
            random = _rnd_;
        }

        template <typename f_type,
                  typename std::enable_if<
                      std::is_same<f_type, float>::value,
                      bool>::type = true>
        f_type next01()
        {
            return random->getFloat01();
        }

        template <typename f_type,
                  typename std::enable_if<
                      std::is_same<f_type, double>::value,
                      bool>::type = true>
        f_type next01()
        {
            return random->getDouble01();
        }

        template <typename _math_type,
                  typename std::enable_if<
                      std::is_floating_point<typename MathTypeInfo<_math_type>::_type>::value //
                      ,
                      bool>::type = true>
        ITK_INLINE _math_type nextRange(const _math_type &min, const _math_type &max)
        {
            _math_type delta = max - min;
            delta *= next<_math_type>();
            return min + delta;
        }

        template <typename _math_type,
                  typename std::enable_if<
                      std::is_integral<typename MathTypeInfo<_math_type>::_type>::value && //
                          MathTypeInfo<_math_type>::_is_vec::value                         //
                      ,
                      bool>::type = true>
        ITK_INLINE _math_type nextRange(const _math_type &min, const _math_type &max)
        {
            _math_type result = _math_type();
            for (int i = 0; i < (int)_math_type::array_count; i++)
                result[i] = random->getRange(min[i], max[i]);
            return result;
        }

        template <typename _math_type,
                  typename std::enable_if<
                      std::is_integral<typename MathTypeInfo<_math_type>::_type>::value && //
                          !MathTypeInfo<_math_type>::_is_vec::value                        //
                      ,
                      bool>::type = true>
        ITK_INLINE _math_type nextRange(const _math_type &min, const _math_type &max)
        {
            _math_type result = _math_type();
            for (int c = 0; c < (int)_math_type::cols; c++)
                for (int r = 0; r < (int)_math_type::rows; r++)
                    result[c][r] = random->getRange(min[c][r], max[c][r]);

            return result;
        }

        template <typename _math_type,
                  typename std::enable_if<
                      std::is_floating_point<typename MathTypeInfo<_math_type>::_type>::value && //
                          MathTypeInfo<_math_type>::_is_vec::value                               //
                      ,
                      bool>::type = true>
        ITK_INLINE _math_type next()
        {
            using _f_type = typename MathTypeInfo<_math_type>::_type;

            _math_type result = _math_type();
            for (int i = 0; i < (int)_math_type::array_count; i++)
                result[i] = next01<_f_type>();
            return result;
        }

        template <typename _math_type,
                  typename std::enable_if<
                      std::is_floating_point<typename MathTypeInfo<_math_type>::_type>::value && //
                          !MathTypeInfo<_math_type>::_is_vec::value                              //
                      ,
                      bool>::type = true>
        ITK_INLINE _math_type next()
        {
            using _f_type = typename MathTypeInfo<_math_type>::_type;

            _math_type result = _math_type();
            for (int c = 0; c < (int)_math_type::cols; c++)
                for (int r = 0; r < (int)_math_type::rows; r++)
                    result[c][r] = next01<_f_type>();
            return result;
        }

        template <typename _math_type,
                  typename std::enable_if<
                      std::is_floating_point<typename MathTypeInfo<_math_type>::_type>::value && //
                          MathTypeInfo<_math_type>::_is_vec::value                               //
                      ,
                      bool>::type = true>
        ITK_INLINE _math_type nextDirection(bool homogeneous = false)
        {
            using _f_type = typename MathTypeInfo<_math_type>::_type;
            _math_type result = _math_type();
            do
            {
                for (int i = 0; i < (int)_math_type::array_count - (int)homogeneous; i++)
                    result[i] = next01<_f_type>();
                result = result * (_f_type)2 - (_f_type)1;
                if (homogeneous)
                    result[_math_type::array_count - 1] = (_f_type)0;
                result = OP<_math_type>::normalize(result);
            } while (OP<_math_type>::sqrLength(result) < 0.25);
            return result;
        }

        template <typename _math_type,
                  typename std::enable_if<
                      (std::is_same<typename MathTypeInfo<_math_type>::_class, MathTypeClass::_class_vec3>::value ||  //
                       std::is_same<typename MathTypeInfo<_math_type>::_class, MathTypeClass::_class_vec4>::value) && //
                          std::is_floating_point<typename MathTypeInfo<_math_type>::_type>::value                     //
                      ,
                      bool>::type = true>
        ITK_INLINE _math_type nextUVW()
        {
            using _f_type = typename MathTypeInfo<_math_type>::_type;

            _f_type u = next01<_f_type>();
            _f_type v = next01<_f_type>();
            _f_type w = (_f_type)1 - u - v;
            if (w < (_f_type)0)
            {
                u = (_f_type)1 - u;
                v = (_f_type)1 - v;
                w = -w;
            }
            _math_type result;
            result[0] = u;
            result[1] = v;
            result[2] = w;
            return result;
        }

        template <typename _math_type,
                  typename std::enable_if<
                      (std::is_same<typename MathTypeInfo<_math_type>::_class, MathTypeClass::_class_vec3>::value ||  //
                       std::is_same<typename MathTypeInfo<_math_type>::_class, MathTypeClass::_class_vec4>::value) && //
                          std::is_floating_point<typename MathTypeInfo<_math_type>::_type>::value                     //
                      ,
                      bool>::type = true>
        ITK_INLINE _math_type nextPointInsideTriangle(const _math_type &a, const _math_type &b, const _math_type &c)
        {
            using _f_type = typename MathTypeInfo<_math_type>::_type;

            _f_type u = next01<_f_type>();
            _f_type v = next01<_f_type>();
            _f_type w = (_f_type)1 - u - v;
            if (w < (_f_type)0)
            {
                u = (_f_type)1 - u;
                v = (_f_type)1 - v;
                w = -w;
            }
            _math_type result = a * u + b * v + c * w;
            if (std::is_same<typename MathTypeInfo<_math_type>::_class, MathTypeClass::_class_vec4>::value)
                result[3] = (_f_type)1;
            return result;
        }

    };

}
