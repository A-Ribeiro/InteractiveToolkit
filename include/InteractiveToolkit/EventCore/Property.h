#pragma once

#include "../common.h"
#include "Event.h"

namespace EventCore
{

    /// \brief Define a custom property with a callback event list to manage the changes.
    ///
    /// The property class store the value you set through the template pattern.
    ///
    /// When the class detects it changed, it will call the #Property::OnChange event.
    ///
    /// Example:
    ///
    /// \code
    /// #include <aRibeiroCore/aRibeiroCore.h>
    /// using namespace aRibeiro;
    ///
    /// Property<vec2> Size;
    ///
    /// // you can listen to property changes
    ///
    /// void MyListeningFunction(Property<vec2> *prop) {
    ///     vec2 vOld = prop->oldValue;
    ///     vec2 vNew = prop->value;
    ///     //you can rollback the modification.
    ///     //  The next listeners will receive the old value as new to set...
    ///     prop->rollback();
    /// }
    ///
    /// Size.OnChange.add(MyListeningFunction);
    ///
    /// // trigger the property change
    /// Size = MathCore::vec2f(1.0f);
    /// \endcode
    ///
    /// \author Alessandro Ribeiro
    ///
    template <typename T>
    class Property
    {

        // avoid copy, using copy constructors
        Property(const Property &) {}
        void operator=(const Property &) {}

        T oldValue; ///< The property last value, before the modification
        T value;    ///< The property current value

    public:
        Event<void(const T &value, const T &oldValue)> OnChange; ///< Called when a modification occurs

        /// \brief Construct this property with an initial value.
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// Property<vec2> Size = Property<vec2>( MathCore::vec2f(1.0f) );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ITK_INLINE Property(const T &defaultValue)
        {
            oldValue = defaultValue;
            value = defaultValue;
        }

        ITK_INLINE Property()
        {
            oldValue = T();
            value = T();
        }

        /// \brief Undo the last modification
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// Property<vec2> Size;
        ///
        /// // you can listen to property changes
        ///
        /// void MyListeningFunction(Property<vec2> *prop) {
        ///     vec2 vOld = prop->oldValue;
        ///     vec2 vNew = prop->value;
        ///     //you can rollback the modification.
        ///     //  The next listeners will receive the old value as new to set...
        ///     prop->rollback();
        /// }
        ///
        /// Size.OnChange.add(MyListeningFunction);
        ///
        /// // trigger the property change
        /// Size = MathCore::vec2f(1.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ITK_INLINE void rollback()
        {
            value = oldValue;
        }

        /// \brief Set the property value from a parameter of the type that is defined in the property creation
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// Property<vec2> Size;
        ///
        /// ...
        ///
        /// // Set the property value
        /// Size = MathCore::vec2f(1.0f);
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ITK_INLINE void operator=(const T &v)
        {
            if (value != v)
            {
                oldValue = value;
                value = v;
                OnChange(value, oldValue); // OnChange(this);
            }
        }

        /// \brief Cast the property to the template parameter class
        ///
        /// Example:
        ///
        /// \code
        /// #include <aRibeiroCore/aRibeiroCore.h>
        /// using namespace aRibeiro;
        ///
        /// Property<vec2> Size;
        ///
        /// ...
        ///
        /// vec2 vec_content = (vec2)Size;
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        ITK_INLINE operator T() const
        {
            return value;
        }

        ITK_INLINE bool operator==(const T &param) const
        {
            return value == param;
        }

        ITK_INLINE bool operator!=(const T &param) const
        {
            return value != param;
        }

        ITK_INLINE void operator+=(const T &v)
        {
            oldValue = value;
            value += v;
            if (value != oldValue)
            {
                OnChange(value, oldValue); // OnChange(this);
            }
            // return value;
        }
        ITK_INLINE void operator-=(const T &v)
        {
            oldValue = value;
            value -= v;
            if (value != oldValue)
            {
                OnChange(value, oldValue); // OnChange(this);
            }
            // return value;
        }
        ITK_INLINE void operator*=(const T &v)
        {
            oldValue = value;
            value *= v;
            if (value != oldValue)
            {
                OnChange(value, oldValue); // OnChange(this);
            }
            // return value;
        }
        ITK_INLINE void operator/=(const T &v)
        {
            oldValue = value;
            value /= v;
            if (value != oldValue)
            {
                OnChange(value, oldValue); // OnChange(this);
            }
            // return value;
        }
        ITK_INLINE void operator^=(const T &v)
        {
            oldValue = value;
            value ^= v;
            if (value != oldValue)
            {
                OnChange(value, oldValue); // OnChange(this);
            }
            // return value;
        }

        // ITK_INLINE const T* operator->()const {
        //     return &value;
        // }

        ITK_INLINE const T *c_ptr() const
        {
            return &value;
        }

        ITK_INLINE const T &c_val() const
        {
            return value;
        }

        ITK_INLINE void forceTriggerOnChange() const
        {
            OnChange(value, value);
        }

        ITK_INLINE void setValueNoCallback(const T &v)
        {
            if (value != v)
            {
                oldValue = value;
                value = v;
                //OnChange(value, oldValue); // OnChange(this);
            }
        }
    };

#define _declare_property_operator(OP)                                                   \
    template <typename T>                                                                \
    static ITK_INLINE T operator OP(const T &a, const Property<T> &b) noexcept           \
    {                                                                                    \
        return a OP(T) b;                                                                \
    }                                                                                    \
    template <typename T>                                                                \
    static ITK_INLINE T operator OP(const Property<T> &a, const T &b) noexcept           \
    {                                                                                    \
        return (T)a OP b;                                                                \
    }                                                                                    \
    template <typename T>                                                                \
    static ITK_INLINE T operator OP(const Property<T> &a, const Property<T> &b) noexcept \
    {                                                                                    \
        return (T)a OP(T) b;                                                             \
    }                                                                                    \
    template <typename T, typename _T>                                                   \
    static ITK_INLINE _T operator OP(const _T &a, const Property<T> &b) noexcept         \
    {                                                                                    \
        return a OP(T) b;                                                                \
    }                                                                                    \
    template <typename T, typename _T>                                                   \
    static ITK_INLINE _T operator OP(const Property<T> &a, const _T &b) noexcept         \
    {                                                                                    \
        return (T)a OP b;                                                                \
    }

    _declare_property_operator(+);
    _declare_property_operator(-);
    _declare_property_operator(*);
    _declare_property_operator(/);
    _declare_property_operator(^);

    _declare_property_operator(&&);
    _declare_property_operator(||);

#undef _declare_property_operator

}
