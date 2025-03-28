#pragma once

#include <algorithm>
#include <vector>
#include <functional>
#include <type_traits>
#include <memory>
#include <stdexcept>

#include "../common.h"
#include "HandleCallback.h"
#include "../ITKCommon/STL_Tools.h"

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4407)
#endif

namespace EventCore
{

// The member function in C++:
//   void(_ClassType::*ptr_class_member)(args)
// is equivalent to C functor:
//   void(*ptr_class_member)(_ClassType*, args)
#define USE_C_FUNCTOR_PTR_FOR_MEMBER_FUNCTION false

    // forward template
    template <typename _single_fnc_, typename _BaseClassType>
    class Event;

    enum class CallType : uint8_t
    {
        None,
        Functor,
        ClassMember,
        STDFunction
    };

    template <typename _single_fnc_, typename _BaseClassType = HandleCallback>
    class Callback
    {
    };

    template <typename _RetType, typename... _ArgsType, typename _BaseClassType>
    class Callback<_RetType(_ArgsType...), _BaseClassType>
    {

        using self_type = Callback<_RetType(_ArgsType...), _BaseClassType>;

        using return_type = _RetType;

        using std_function_functor = typename std::function<_RetType(_ArgsType...)>;
        using ptr_functor = _RetType (*)(_ArgsType...);

        // using std_function_class_member = typename std::function<_RetType(_BaseClassType*, _ArgsType...)>;
        using ptr_class_member = _RetType (_BaseClassType::*)(_ArgsType...);
        using ptr_class_member_functor = _RetType(*)(_BaseClassType*, _ArgsType...);

        //
        // Functor
        //
        // - std::function<void(args)>
        std_function_functor _std_function_functor;
        // - void(*ptr_functor)(args)
        ptr_functor _ptr_functor;
        //
        // Class Member
        //
        // - std::function<void(_ClassType*,args)>
        // std_function_class_member _std_function_class_member;
        // - void(_ClassType::*ptr_class_member)(args)
#if USE_C_FUNCTOR_PTR_FOR_MEMBER_FUNCTION == true
        ptr_class_member_functor _ptr_class_member;
#else 
        ptr_class_member _ptr_class_member;
#endif
        // - ptr_instance*
        _BaseClassType *_ptr_instance;

        // _RetType call_functor(_ArgsType... _arg) const
        // {
        //     return _ptr_functor(std::forward<_ArgsType>(_arg)...);
        // }
        // _RetType call_instance(_ArgsType... _arg) const
        // {
        //     return (_ptr_instance->*_ptr_class_member)(std::forward<_ArgsType>(_arg)...);
        // }
        // _RetType call_std(_ArgsType... _arg) const
        // {
        //     return _std_function_functor(std::forward<_ArgsType>(_arg)...);
        // }
        //_RetType (self_type::*call_ptr)(_ArgsType...)const;

        CallType mCallType;

    public:
        _RetType operator()(_ArgsType... _arg) const
        {
            switch (mCallType)
            {
            case CallType::ClassMember:
#if USE_C_FUNCTOR_PTR_FOR_MEMBER_FUNCTION == true
                return _ptr_class_member(_ptr_instance, std::forward<_ArgsType>(_arg)...);
#else
                return (_ptr_instance->*_ptr_class_member)(std::forward<_ArgsType>(_arg)...);
#endif
            case CallType::Functor:
                return _ptr_functor(std::forward<_ArgsType>(_arg)...);
            default:
                return _std_function_functor(std::forward<_ArgsType>(_arg)...);
            }
            // return (this->*call_ptr)(std::forward<_ArgsType>(_arg)...);
            //  if (_ptr_functor)
            //      return _ptr_functor(std::forward<_ArgsType>(_arg)...);
            //  else if (_ptr_class_member)
            //      return (_ptr_instance->*_ptr_class_member)(std::forward<_ArgsType>(_arg)...);
            //  else
            //      return _std_function_functor(std::forward<_ArgsType>(_arg)...);

            // if (_std_function_functor != nullptr)
            // 	return _std_function_functor(std::forward<_ArgsType>(_arg)...);
            // else
            // 	return _std_function_class_member(_ptr_instance, std::forward<_ArgsType>(_arg)...);
        }

        operator bool() const
        {
            return _std_function_functor != nullptr ||
                   _ptr_class_member != nullptr ||
                   _ptr_instance != nullptr;
        }

        ITK_INLINE std_function_functor toFunction() const
        {
            return _std_function_functor;
        }

        //////////////////////////
        // COPY CONSTRUCTOR
        // COPY OPERATOR
        //////////////////////////
        Callback(const self_type &_v)
        {

            // functor
            _ptr_functor = _v._ptr_functor;
            _std_function_functor = _v._std_function_functor;

            // object
            _ptr_instance = _v._ptr_instance;
            _ptr_class_member = _v._ptr_class_member;
            //_std_function_class_member = _v._std_function_class_member;

            mCallType = _v.mCallType;
        }
        self_type &operator=(const self_type &_v)
        {

            // functor
            _ptr_functor = _v._ptr_functor;
            _std_function_functor = _v._std_function_functor;

            // object
            _ptr_instance = _v._ptr_instance;
            _ptr_class_member = _v._ptr_class_member;
            //_std_function_class_member = _v._std_function_class_member;

            mCallType = _v.mCallType;

            return *this;
        }

        // rvalue assignment
        Callback(self_type &&_v)
        {

            // functor
            _ptr_functor = _v._ptr_functor;
            _std_function_functor = std::move(_v._std_function_functor);

            // object
            _ptr_instance = _v._ptr_instance;
            _ptr_class_member = _v._ptr_class_member;
            //_std_function_class_member = std::move(_v._std_function_class_member);

            mCallType = _v.mCallType;
        }
        self_type &operator=(self_type &&_v)
        {

            // functor
            _ptr_functor = _v._ptr_functor;
            _std_function_functor = std::move(_v._std_function_functor);

            // object
            _ptr_instance = _v._ptr_instance;
            _ptr_class_member = _v._ptr_class_member;
            //_std_function_class_member = std::move(_v._std_function_class_member);

            mCallType = _v.mCallType;

            return *this;
        }

        //////////////////////////
        // CONSTRUCTOR PART
        //////////////////////////

        Callback()
        {
            // functor
            _ptr_functor = nullptr;
            _std_function_functor = nullptr;

            // object
            _ptr_instance = nullptr;
            _ptr_class_member = nullptr;
            //_std_function_class_member = nullptr;

            mCallType = CallType::None;
        }

        template <typename _unknown_type_,
                  typename std::enable_if<

                      std::is_assignable<std_function_functor, _unknown_type_>::value &&
                      !std::is_assignable<self_type, _unknown_type_>::value

                      >::type * = nullptr>
        Callback(_unknown_type_ &&_unknown_instance)
        {

            //
            // Can be std_bind, std_function, lambda
            //

            // can use to test the input parameter
            // std::is_bind_expression<_unknown_type_>::value
            // std::is_same<_unknown_type_, std_function_functor>::value

            // static_assert(std::is_assignable<std_function_functor, _unknown_type_>::value,
            // 	"parameter does not match the Callback signature."
            // 	);

            // functor
            _ptr_functor = nullptr;
            _std_function_functor = std::forward<_unknown_type_>(_unknown_instance);

            // object
            _ptr_instance = nullptr;
            _ptr_class_member = nullptr;
            //_std_function_class_member = nullptr;

            mCallType = CallType::STDFunction;
        }

        // template <typename _unknown_type_>
        // Callback(_unknown_type_&&_unknown_instance) {

        // 	//
        // 	// Can be std_bind, std_function, lambda
        // 	//

        // 	// can use to test the input parameter
        // 	// std::is_bind_expression<_unknown_type_>::value
        // 	// std::is_same<_unknown_type_, std_function_functor>::value

        // 	static_assert(std::is_assignable<std_function_functor, _unknown_type_>::value,
        // 		"parameter does not match the Callback signature."
        // 		);

        // 	// functor
        // 	_ptr_functor = nullptr;
        // 	_std_function_functor = std::move(_unknown_instance);

        // 	// object
        // 	_ptr_instance = nullptr;
        // 	_ptr_class_member = nullptr;
        // 	_std_function_class_member = nullptr;
        // }

        Callback(ptr_functor ptr_functor_ref)
        {
            //
            // ptr_functor
            //

            // functor
            _ptr_functor = ptr_functor_ref;
            _std_function_functor = ptr_functor_ref; // std_function_functor(_ptr_functor);

            // object
            _ptr_instance = nullptr;
            _ptr_class_member = nullptr;
            //_std_function_class_member = nullptr;

            mCallType = CallType::Functor;
        }

        Callback(const std_function_functor &std_function_instance)
        {
            //
            // std_function_functor
            //

            // functor
            _ptr_functor = nullptr;
            _std_function_functor = std_function_instance;

            // object
            _ptr_instance = nullptr;
            _ptr_class_member = nullptr;
            //_std_function_class_member = nullptr;

            mCallType = CallType::STDFunction;
        }

        Callback(std_function_functor &&std_function_instance)
        {
            //
            // std_function_functor
            //

            // functor
            _ptr_functor = nullptr;
            _std_function_functor = std::move(std_function_instance);

            // object
            _ptr_instance = nullptr;
            _ptr_class_member = nullptr;
            //_std_function_class_member = nullptr;

            mCallType = CallType::STDFunction;
        }

        template <typename _ClassType, typename Indices = STL_Tools::make_index_sequence<(sizeof...(_ArgsType))>>
        Callback(_RetType (_ClassType::*class_member)(_ArgsType...), _ClassType *instance)
        {
            setCallback(class_member, instance, Indices());
        }

        template <typename _ClassType, typename Indices = STL_Tools::make_index_sequence<(sizeof...(_ArgsType))>>
        Callback(_RetType (_ClassType::*class_member)(_ArgsType...) const, _ClassType *instance)
        {
            setCallback((_RetType(_ClassType::*)(_ArgsType...))class_member, instance, Indices());
        }

    private:
        template <typename _ClassType, std::size_t... I>
        void setCallback(_RetType (_ClassType::*class_member)(_ArgsType...), _ClassType *instance, STL_Tools::index_sequence<I...>)
        {
            //
            // ptr_class_member
            //

            static_assert(std::is_base_of<_BaseClassType, _ClassType>::value,
                          "instance need to derive HandleCallback "
                          "to be used in the Callback class. "
                          "Example 'class Obj:public EventCore::HandleCallback'");

            /*static_assert(std::is_same<_RetType(_BaseClassType::*)(_ArgsType...), ptr_class_member>::value,
                "parameter does not match the Callback signature (ptr_class_member)."
                );*/

            // functor
            _ptr_functor = nullptr;
            //_std_function_functor = nullptr;

            // object
            _ptr_instance = reinterpret_cast<_BaseClassType *>(instance);
#if USE_C_FUNCTOR_PTR_FOR_MEMBER_FUNCTION == true
            _ptr_class_member = *(ptr_class_member_functor*)&class_member;
#else
            _ptr_class_member = *(ptr_class_member*)&class_member;
#endif
            //_std_function_class_member = _ptr_class_member;//std_function_class_member(_ptr_class_member);


            {
                _std_function_functor = std::bind(_ptr_class_member, _ptr_instance,
                                                  STL_Tools::placeholder<I + 1>{}...);
            }

            mCallType = CallType::ClassMember;
        }

    public:
        //////////////////////////
        // COMPARISON PART
        //////////////////////////

        template <typename _unknown_type_,
                  typename std::enable_if<
                      std::is_assignable<std_function_functor, _unknown_type_>::value>::type * = nullptr>
        bool operator==(const _unknown_type_ &_unknown_instance) const
        {

            static_assert(
                std::is_same<self_type, _unknown_type_>::value,
                "cannot compare a callback against std::bind, std::function, lambda or any other pointer.");

#pragma push_macro("__to__String2")
#pragma push_macro("__to__String")

#undef __to__String2
#undef __to__String

#define __to__String2(x) #x
#define __to__String(x) __to__String2(x)

            throw std::runtime_error(
                "[" __FILE__ ":" __to__String(__LINE__) "]"
                                                        " cannot compare a callback against std::bind, std::function, lambda or any other pointer.");

#pragma pop_macro("__to__String2")
#pragma pop_macro("__to__String")

            return false;
        }

        template <typename _unknown_type_,
                  typename std::enable_if<
                      std::is_assignable<std_function_functor, _unknown_type_>::value>::type * = nullptr>
        bool operator!=(const _unknown_type_ &_unknown_instance) const
        {

            static_assert(
                std::is_same<self_type, _unknown_type_>::value,
                "cannot compare a callback against std::bind, std::function, lambda or any other pointer.");

#pragma push_macro("__to__String2")
#pragma push_macro("__to__String")

#undef __to__String2
#undef __to__String

#define __to__String2(x) #x
#define __to__String(x) __to__String2(x)

            throw std::runtime_error(
                "[" __FILE__ ":" __to__String(__LINE__) "]"
                                                        " cannot compare a callback against std::bind, std::function, lambda or any other pointer.");

#pragma pop_macro("__to__String2")
#pragma pop_macro("__to__String")

            return true;
        }

        bool operator==(nullptr_t) const
        {
            return !(*this);
        }

        bool operator!=(nullptr_t) const
        {
            return (*this);
        }

        // template<typename _RetType_param, typename... _ArgsType_param>
        // bool operator==(_RetType_param(*ptr_functor_ref)(_ArgsType_param...))const {
        bool operator==(ptr_functor ptr_functor_ref) const
        {
            //
            // ptr_functor
            //
            return //(std::is_same<_RetType_param(*)(_ArgsType_param...), ptr_functor>::value) &&
                   // functor
                (_std_function_functor != nullptr) &&
                _ptr_functor == ptr_functor_ref &&
                // object
                _ptr_class_member == nullptr &&
                _ptr_instance == nullptr;
        }

        // template<typename _RetType_param, typename... _ArgsType_param>
        // bool operator!=(_RetType_param(*ptr_functor_ref)(_ArgsType_param...))const {
        bool operator!=(ptr_functor ptr_functor_ref) const
        {
            //
            // ptr_functor
            //
            return !((*this) == ptr_functor_ref);
        }

        bool operator==(const self_type &_c) const
        {
            return // functor
                   //(_std_function_functor == nullptr) == (_c._std_function_functor == nullptr) &&
                _ptr_functor == _c._ptr_functor &&
                // object
                _ptr_class_member == _c._ptr_class_member &&
                _ptr_instance == _c._ptr_instance &&
                // check if has any ptr functor or class member...
                (_ptr_functor != nullptr || _ptr_class_member != nullptr);
        }

        bool operator!=(const self_type &_c) const
        {
            return !((*this) == _c);
        }

    public:
        // class Event<_RetType(_ArgsType...), _BaseClassType>;
        friend class Event<_RetType(_ArgsType...), _BaseClassType>;
    };

    //
    // nullptr comparison
    //
    template <typename _RetType, typename... _ArgsType, typename _BaseClassType>
    bool operator==(nullptr_t, const Callback<_RetType(_ArgsType...), _BaseClassType> &_a)
    {
        return !static_cast<bool>(_a);
    }
    template <typename _RetType, typename... _ArgsType, typename _BaseClassType>
    bool operator!=(nullptr_t, const Callback<_RetType(_ArgsType...), _BaseClassType> &_a)
    {
        return static_cast<bool>(_a);
    }

    template <typename _RetType, typename... _ArgsType, typename _BaseClassType = HandleCallback, typename _ClassTypeA, typename _ClassTypeB>
    Callback<_RetType(_ArgsType...), _BaseClassType> CallbackWrapper(_RetType (_ClassTypeA::*class_member)(_ArgsType...), _ClassTypeB *instance)
    {
        return Callback<_RetType(_ArgsType...), _BaseClassType>(class_member, (_ClassTypeA*)instance);
    }

    template <typename _RetType, typename... _ArgsType, typename _BaseClassType = HandleCallback, typename _ClassTypeA, typename _ClassTypeB>
    Callback<_RetType(_ArgsType...), _BaseClassType> CallbackWrapper(_RetType (_ClassTypeA::*class_member)(_ArgsType...) const, _ClassTypeB *instance)
    {
        return Callback<_RetType(_ArgsType...), _BaseClassType>(class_member, (_ClassTypeA*)instance);
    }

}

#if defined(_WIN32)
#pragma warning(pop)
#endif
