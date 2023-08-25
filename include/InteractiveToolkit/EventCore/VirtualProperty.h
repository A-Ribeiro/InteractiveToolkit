#pragma once

#include "../common.h"
#include "Callback.h"

namespace EventCore
{

    /// \brief Class to implement a completely virtual property.
    ///
    /// The #aRibeiro::Property class stores the content of the variable and check if a variation occurs to trigger an event #aRibeiro::Property::OnChange.
    ///
    /// This class do not store the values. It call internal class methods for get and set custom implementation instead.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aRibeiroCore/aRibeiroCore.h>
    /// using namespace aRibeiro;
    ///
    /// class Example {
    ///
    ///     vec2 size_internal;
    ///
    ///     vec2 size_get() const {
    ///         return size_internal;
    ///     }
    ///
    ///     void size_set(const vec2& v) {
    ///         size_internal = v;
    ///     }
    ///
    /// public:
    ///
    ///     VirtualProperty<vec2> Size;
    ///
    ///     Example() : Size(this, &Example::size_get, &Example::size_set) {
    ///
    ///     }
    /// };
    ///
    /// Example example;
    ///
    /// // set the value
    /// example.Size = vec2(1.0);
    ///
    /// // get the value
    /// vec2 aux = (vec2)example.Size;
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    class VirtualProperty
    {

        // avoid copy, using copy constructors
        VirtualProperty(const VirtualProperty &) {}
        void operator=(const VirtualProperty &) {}

        Callback<T()> _get;
        Callback<void(const T &)> _set;

    public:
        ITK_INLINE VirtualProperty(Callback<T()> p_get, Callback<void(const T &)> p_set)
        {
            _get = p_get;
            _set = p_set;
        }

        ITK_INLINE void operator=(const T &param) const
        {
            _set(param);
        }

        ITK_INLINE operator T() const
        {
            return _get();
        }

        ITK_INLINE bool operator==(const T &param) const
        {
            return _get() == param;
        }

        ITK_INLINE bool operator!=(const T &param) const
        {
            return _get() != param;
        }

        ITK_INLINE void operator+=(const T &v) const
        {
            _set(_get() + v);
            //return _get();
        }
        ITK_INLINE void operator-=(const T &v) const
        {
            _set(_get() - v);
            //return _get();
        }
        ITK_INLINE void operator*=(const T &v) const
        {
            _set(_get() * v);
            //return _get();
        }
        ITK_INLINE void operator/=(const T &v) const
        {
            _set(_get() / v);
            //return _get();
        }
        ITK_INLINE void operator^=(const T &v) const
        {
            _set(_get() ^ v);
            //return _get();
        }

    };

    #define _declare_vproperty_operator(OP)                                                    \
    template <typename T>                                                                 \
    static ITK_INLINE T operator OP(const T &a, const VirtualProperty<T> &b) noexcept           \
    {                                                                                     \
        return a OP (T)b;                                                              \
    }                                                                                     \
    template <typename T>                                                                 \
    static ITK_INLINE T operator OP(const VirtualProperty<T> &a, const T &b) noexcept           \
    {                                                                                     \
        return (T)a OP b;                                                              \
    }                                                                                     \
    template <typename T>                                                                 \
    static ITK_INLINE T operator OP(const VirtualProperty<T> &a, const VirtualProperty<T> &b) noexcept \
    {                                                                                     \
        return (T)a OP (T)b;                                                        \
    }

    _declare_vproperty_operator(+);
    _declare_vproperty_operator(-);
    _declare_vproperty_operator(*);
    _declare_vproperty_operator(/);
    _declare_vproperty_operator(^);

}