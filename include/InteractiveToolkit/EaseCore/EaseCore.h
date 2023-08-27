#pragma once
/*
General information:

I've implemented the easing equations based on the
Tweener implementation (MIT license).

The original code was time based interpolation.
I've changed it to be based on lerp param [0 to 1].

Can be used with any object that implements arithmetic operators.

There is only an equation that I've come up with:

*The Elastic equations.

The code in Tweener uses the period, duration and time directly in the equations.
To make it usable in the lerp (a,b,factor) fashion, I've
written a new function that generates similar values for the elastic equations.
*/

#include "../common.h"
#include "../MathCore/MathCore.h"

namespace EaseCore
{

    template <typename _type>
    struct EaseEq
    {

        /// \brief Compose two easing equations in sequence
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 interpolationFunctionA(const vec3& a, const vec3& b, _type lerp){
        ///     ...
        /// }
        ///
        /// vec3 interpolationFunctionB(const vec3& a, const vec3& b, _type lerp){
        ///     ...
        /// }
        ///
        /// vec3 result = easeCompose<vec3>( vec3(0.0), vec3(1.0), 0.5f, &interpolationFunctionA, &interpolationFunctionB );
        ///
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @param interplFuncIn First easing function pointer
        /// @param interplFuncOut Second easing function pointer
        /// @return        The value.
        ///
        template <class T>
        static inline T easeCompose(const T &_startValue, const T &_endValue, _type lerp,
                                    T (*interplFuncIn)(const T &_startValue, const T &_endValue, _type lerp),
                                    T (*interplFuncOut)(const T &_startValue, const T &_endValue, _type lerp))
        {
            T mid = (_endValue + _startValue) * (_type)0.5;
            lerp *= (_type)2;
            if (lerp <= (_type)1)
                return interplFuncIn(_startValue, mid, lerp);
            else
                return interplFuncOut(mid, _endValue, lerp - (_type)1);
        }

/// \brief Declare a composed ease template function inline.
///
/// Example:
///
/// \code
/// #include <aRibeiroCore/aRibeiroCore.h>
/// using namespace aRibeiro;
///
/// template <class T>
/// static inline T inQuad(const T& _startValue, const T& _endValue, _type lerp) {
///     ...
/// }
///
/// template <class T>
/// static inline T outQuad(const T& _startValue, const T& _endValue, _type lerp) {
///     ...
/// }
///
/// DECLARE_COMPOSED_EASE_FUNCTION(inOutQuad,inQuad,outQuad)
/// \endcode
///
/// \author Alessandro Ribeiro
/// @param name    The function template name.
/// @param fa   First function (from 0 to 0.5).
/// @param fb   Second function (from 0.5 to 1.0).
/// @return        The value.
///
#define DECLARE_COMPOSED_EASE_FUNCTION(__name__, __fa__, __fb__)                   \
    template <class T>                                                             \
    static inline T __name__(const T &_startValue, const T &_endValue, _type lerp) \
    {                                                                              \
        T mid = (_endValue + _startValue) * (_type)0.5;                            \
        lerp *= (_type)2;                                                          \
        if (lerp <= (_type)1)                                                      \
            return __fa__<T>(_startValue, mid, lerp);                              \
        else                                                                       \
            return __fb__<T>(mid, _endValue, lerp - (_type)1);                     \
    }

        /// \brief Easing equation function for a linear (t) easing.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = easeNone<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T none(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            return (delta) * (lerp) + _startValue;
        }

        /// \brief Easing equation function for a quadratic (t^2) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inQuad<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T inQuad(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            return (delta) * (lerp * lerp) + _startValue;
        }

        /// \brief Easing equation function for a quadratic (t^2) easing out: decelerating to zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outQuad<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T outQuad(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            return (delta) * (lerp * ((_type)2 - lerp)) + _startValue;
        }
        // public static function outQuad (t:Number, b:Number, c:Number, d:Number, p_params:Object = null):Number {
        //     return -c *(t/=d)*(t-2) + b;
        // }

        /// \brief Easing equation function for a quadratic (t^2) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inOutQuad<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(inOutQuad, inQuad, outQuad)

        /// \brief Easing equation function for a quadratic (t^2) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outInQuad<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(outInQuad, outQuad, inQuad)

        /// \brief Easing equation function for a cubic (t^3) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inCubic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T inCubic(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            return (delta) * (lerp * lerp * lerp) + _startValue;
        }

        /// \brief Easing equation function for a cubic (t^3) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outCubic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T outCubic(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            lerp -= (_type)1;
            return (delta) * (lerp * lerp * lerp + (_type)1) + _startValue;
        }

        /// \brief Easing equation function for a cubic (t^3) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inOutCubic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(inOutCubic, inCubic, outCubic)

        /// \brief Easing equation function for a cubic (t^3) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outInCubic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(outInCubic, outCubic, inCubic)

        /// \brief Easing equation function for a quartic (t^4) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inQuart<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T inQuart(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            return (delta) * (lerp * lerp * lerp * lerp) + _startValue;
        }

        /// \brief Easing equation function for a quartic (t^4) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outQuart<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T outQuart(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            lerp -= (_type)1;
            return (delta) * ((_type)1 - lerp * lerp * lerp * lerp) + _startValue;
        }

        /// \brief Easing equation function for a quartic (t^4) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inOutQuart<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(inOutQuart, inQuart, outQuart)

        /// \brief Easing equation function for a quartic (t^4) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outInQuart<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(outInQuart, outQuart, inQuart)

        /// \brief Easing equation function for a quintic (t^5) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inQuint<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T inQuint(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            return (delta) * (lerp * lerp * lerp * lerp * lerp) + _startValue;
        }

        /// \brief Easing equation function for a quintic (t^5) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outQuint<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T outQuint(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            lerp -= (_type)1;
            return (delta) * ((_type)1 + lerp * lerp * lerp * lerp * lerp) + _startValue;
        }

        /// \brief Easing equation function for a quintic (t^5) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inOutQuint<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(inOutQuint, inQuint, outQuint)

        /// \brief Easing equation function for a quintic (t^5) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outInQuint<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(outInQuint, outQuint, inQuint)

        /// \brief Easing equation function for a sinusoidal (sin(t)) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inSine<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T inSine(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            return delta * ((_type)1 - OP<_type>::cos(lerp * CONSTANT<_type>::PI * (_type)0.5)) + _startValue;
        }

        /// \brief Easing equation function for a sinusoidal (sin(t)) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outSine<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T outSine(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            return delta * (OP<_type>::sin(lerp * CONSTANT<_type>::PI * (_type)0.5)) + _startValue;
        }

        /// \brief Easing equation function for a sinusoidal (sin(t)) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inOutSine<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(inOutSine, inSine, outSine)

        /// \brief Easing equation function for a sinusoidal (sin(t)) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outInSine<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(outInSine, outSine, inSine)

        /// \brief Easing equation function for an exponential (2^t) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inExpo<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T inExpo(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            if (lerp <= (_type)0)
                return _startValue;
            T delta = _endValue - _startValue;
            return delta * (OP<_type>::pow((_type)2, (_type)10 * lerp - (_type)10) - (_type)0.001) + _startValue;
        }

        /// \brief Easing equation function for an exponential (2^t) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outExpo<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T outExpo(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            if (lerp >= (_type)1)
                return _endValue;
            T delta = _endValue - _startValue;
            return delta * ((_type)1.001 - ((_type)1.001 * OP<_type>::pow((_type)2, (_type)-10 * lerp))) + _startValue;
        }

        /// \brief Easing equation function for an exponential (2^t) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inOutExpo<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(inOutExpo, inExpo, outExpo)

        /// \brief Easing equation function for an exponential (2^t) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outInExpo<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(outInExpo, outExpo, inExpo)

        /// \brief Easing equation function for a circular (sqrt(1-t^2)) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inCirc<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T inCirc(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            return delta * ((_type)1 - OP<_type>::sqrt((_type)1 - lerp * lerp)) + _startValue;
        }

        /// \brief Easing equation function for a circular (sqrt(1-t^2)) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outCirc<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T outCirc(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            lerp -= (_type)1;
            return delta * (OP<_type>::sqrt((_type)1 - lerp * lerp)) + _startValue;
        }

        /// \brief Easing equation function for a circular (sqrt(1-t^2)) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inOutCirc<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(inOutCirc, inCirc, outCirc)

        /// \brief Easing equation function for a circular (sqrt(1-t^2)) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outInCirc<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(outInCirc, outCirc, inCirc)

        /// \brief Easing equation function for an elastic (exponentially decaying sine wave) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inElastic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T inElastic(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            if (lerp <= (_type)0)
                return _startValue;
            T delta = _endValue - _startValue;
            return delta * (OP<_type>::cos(lerp * CONSTANT<_type>::PI * (_type)7)) *
                       ((_type)0.001 - OP<_type>::pow((_type)2, (_type)10 * lerp - (_type)10)) +
                   _startValue;
        }

        /// \brief Easing equation function for an elastic (exponentially decaying sine wave) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outElastic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T outElastic(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            if (lerp >= (_type)1)
                return _endValue;
            T delta = _endValue - _startValue;
            return delta * ((OP<_type>::cos(lerp * CONSTANT<_type>::PI * (_type)7)) *
                                ((_type)-1.001 * OP<_type>::pow((_type)2, (_type)-10 * lerp)) +
                            (_type)1) +
                   _startValue;
        }

        /// \brief Easing equation function for an elastic (exponentially decaying sine wave) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inOutElastic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(inOutElastic, inElastic, outElastic)

        /// \brief Easing equation function for an elastic (exponentially decaying sine wave) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outInElastic<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(outInElastic, outElastic, inElastic)

        /// \brief Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inBack<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T inBack(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            const _type overshoot = (_type)1.70158;
            T delta = _endValue - _startValue;
            return delta * lerp * lerp * ((overshoot + (_type)1) * lerp - overshoot) + _startValue;
        }

        /// \brief Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outBack<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T outBack(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            const _type overshoot = (_type)1.70158;
            T delta = _endValue - _startValue;
            lerp -= (_type)1;
            return delta * (lerp * lerp * ((overshoot + (_type)1) * lerp + overshoot) + (_type)1) + _startValue;
        }

        /// \brief Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inOutBack<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(inOutBack, inBack, outBack)

        /// \brief Easing equation function for a back (overshooting cubic easing: (s+1)*t^3 - s*t^2) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outInBack<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(outInBack, outBack, inBack)

        /// \brief Easing equation function for a bounce (exponentially decaying parabolic bounce) easing in: accelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inBounce<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T inBounce(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            lerp = (_type)1 - lerp;
            if (lerp < (_type)1 / (_type)2.75)
            {
                lerp = (_type)7.5625 * lerp * lerp;
            }
            else if (lerp < (_type)2 / (_type)2.75)
            {
                lerp -= (_type)1.5 / (_type)2.75;
                lerp = (_type)7.5625 * lerp * lerp + (_type)0.75;
            }
            else if (lerp < (_type)2.5 / (_type)2.75)
            {
                lerp -= (_type)2.25 / (_type)2.75;
                lerp = (_type)7.5625 * lerp * lerp + (_type)0.9375;
            }
            else
            {
                lerp -= (_type)2.625 / (_type)2.75;
                lerp = (_type)7.5625 * lerp * lerp + (_type)0.984375;
            }
            return delta * ((_type)1 - lerp) + _startValue;
        }

        /// \brief Easing equation function for a bounce (exponentially decaying parabolic bounce) easing out: decelerating from zero velocity.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outBounce<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        template <class T>
        static inline T outBounce(const T &_startValue, const T &_endValue, _type lerp)
        {
            using namespace MathCore;
            lerp = OP<_type>::clamp(lerp, 0, 1);
            T delta = _endValue - _startValue;
            if (lerp < (_type)1 / (_type)2.75)
            {
                lerp = (_type)7.5625 * lerp * lerp;
            }
            else if (lerp < (_type)2 / (_type)2.75)
            {
                lerp -= (_type)1.5 / (_type)2.75;
                lerp = (_type)7.5625 * lerp * lerp + (_type)0.75;
            }
            else if (lerp < (_type)2.5 / (_type)2.75)
            {
                lerp -= (_type)2.25 / (_type)2.75;
                lerp = (_type)7.5625 * lerp * lerp + (_type)0.9375;
            }
            else
            {
                lerp -= (_type)2.625 / (_type)2.75;
                lerp = (_type)7.5625 * lerp * lerp + (_type)0.984375;
            }
            return delta * lerp + _startValue;
        }

        /// \brief Easing equation function for a bounce (exponentially decaying parabolic bounce) easing in/out: acceleration until halfway, then deceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = inOutBounce<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(inOutBounce, inBounce, outBounce)

        /// \brief Easing equation function for a bounce (exponentially decaying parabolic bounce) easing out/in: deceleration until halfway, then acceleration.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// vec3 result = outInBounce<vec3>(vec3(0.0f),vec3(1.0f),0.5f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// @param _startValue    Starting value.
        /// @param _endValue    Ending value.
        /// @param lerp         Value between [0..1] to indicate the interpolation position
        /// @return        The value.
        ///
        DECLARE_COMPOSED_EASE_FUNCTION(outInBounce, outBounce, inBounce)
    };

    using Easef32 = EaseEq<float>;
    using Easef64 = EaseEq<double>;

    using Easef = Easef32;
    using Eased = Easef64;

}
