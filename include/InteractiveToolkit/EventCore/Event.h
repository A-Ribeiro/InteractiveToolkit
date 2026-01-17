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

#include "../Platform/Core/SmartVector.h"

#include "Callback.h"

#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4407)
#endif

namespace ITKCommon
{
    namespace Internal
    {
        static void event_triggerAbort(const char *file, int line, const char *format, ...);
    }
}

namespace EventCore
{

    template <typename _single_fnc_, typename _BaseClassType = HandleCallback>
    class Event
    {
    };

    template <typename _RetType, typename... _ArgsType, typename _BaseClassType>
    class Event<_RetType(_ArgsType...), _BaseClassType>
    {

        using self_type = Event<_RetType(_ArgsType...), _BaseClassType>;

        using return_type = _RetType;

        using std_function_functor = typename std::function<_RetType(_ArgsType...)>;
        using ptr_functor = _RetType (*)(_ArgsType...);

        // using std_function_class_member = typename std::function<_RetType(_BaseClassType*, _ArgsType...)>;
        using ptr_class_member = _RetType (_BaseClassType::*)(_ArgsType...);
        using ptr_class_member_functor = _RetType (*)(_BaseClassType *, _ArgsType...);

        using compatible_callback = Callback<_RetType(_ArgsType...), _BaseClassType>;

        struct __internal
        {
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

            CallType mCallType;

            // 1 - add, 2 - remove
            int op;
        };

        Platform::SmartVector<__internal> fncs;
        Platform::SmartVector<__internal> op_fncs;
        bool _runtime_inside_call;

        // mutable -> allow lock on const methods
        mutable std::recursive_mutex mtx;

        // add works as subscribe, only if not exists
        void _add(__internal &&struct_)
        {
            std::lock_guard<decltype(mtx)> lock(mtx);

            auto rc = std::find_if(fncs.begin(), fncs.end(), [&struct_](const __internal &_c)
                                   {
                                       return

                                           // functor
                                           //(struct_._std_function_functor == nullptr) == (_c._std_function_functor == nullptr) &&
                                           struct_._ptr_functor == _c._ptr_functor &&
                                           // object
                                           struct_._ptr_class_member == _c._ptr_class_member &&
                                           struct_._ptr_instance == _c._ptr_instance &&
                                           // check if has any ptr functor or class member...
                                           (struct_._ptr_functor != nullptr || struct_._ptr_class_member != nullptr);

                                       // struct_._ptr_functor == val._ptr_functor &&
                                       // struct_._ptr_class_member == val._ptr_class_member &&
                                       // struct_._ptr_instance == val._ptr_instance;
                                   });
            if (rc != fncs.end())
                return;

            if (_runtime_inside_call)
            {
                op_fncs.push_back(std::move(struct_));
                // 1 - add, 2 - remove
                op_fncs.back().op = 1;
            }
            else
                fncs.push_back(std::move(struct_));
        }
        void _remove(__internal &&struct_)
        {
            std::lock_guard<decltype(mtx)> lock(mtx);

            auto rc = std::find_if(fncs.begin(), fncs.end(), [&struct_](const __internal &_c)
                                   {
                                       return

                                           // functor
                                           //(struct_._std_function_functor == nullptr) == (_c._std_function_functor == nullptr) &&
                                           struct_._ptr_functor == _c._ptr_functor &&
                                           // object
                                           struct_._ptr_class_member == _c._ptr_class_member &&
                                           struct_._ptr_instance == _c._ptr_instance &&
                                           // check if has any ptr functor or class member...
                                           (struct_._ptr_functor != nullptr || struct_._ptr_class_member != nullptr);

                                       // struct_._ptr_functor == val._ptr_functor &&
                                       // struct_._ptr_class_member == val._ptr_class_member &&
                                       // struct_._ptr_instance == val._ptr_instance;
                                   });
            if (rc == fncs.end())
                return;

            if (_runtime_inside_call)
            {
                op_fncs.push_back(std::move(struct_));
                // 1 - add, 2 - remove
                op_fncs.back().op = 2;
            }
            else
                fncs.erase(rc);
        }

    public:
        void operator()(_ArgsType... _arg)
        {
            {
                std::lock_guard<decltype(mtx)> lock(mtx);

                // avoid recursive callback...
                if (_runtime_inside_call)
                    ITKCommon::Internal::event_triggerAbort(__FILE__, __LINE__, "Recursive event call detected");
                _runtime_inside_call = true;
            }
            for (const __internal &_i_struct : fncs)
            {
                //_i_struct._std_function_functor(_arg...);

                switch (_i_struct.mCallType)
                {
                case CallType::ClassMember:
#if USE_C_FUNCTOR_PTR_FOR_MEMBER_FUNCTION == true
                    _i_struct._ptr_class_member(_i_struct._ptr_instance, _arg...);
#else
                    (_i_struct._ptr_instance->*_i_struct._ptr_class_member)(_arg...);
#endif
                    break;
                case CallType::Functor:
                    _i_struct._ptr_functor(_arg...);
                    break;
                case CallType::STDFunction:
                    _i_struct._std_function_functor(_arg...);
                    break;
                default:
                    // assert(false && "Unknown CallType in Event::operator()!");
                    ITKCommon::Internal::event_triggerAbort(__FILE__, __LINE__, "Unknown CallType in Event::operator()!");
                }

                //_i_struct._std_function_functor(std::forward<_ArgsType>(_arg)...);
                // if (_i_struct._std_function_functor != nullptr)
                // 	_i_struct._std_function_functor(std::forward<_ArgsType>(_arg)...);
                // else
                // 	_i_struct._std_function_class_member(_i_struct._ptr_instance, std::forward<_ArgsType>(_arg)...);
            }

            {
                std::lock_guard<decltype(mtx)> lock(mtx);
                _runtime_inside_call = false;

                // 1 - add, 2 - remove, 3 - clear
                for (__internal &_i_struct : op_fncs)
                {
                    if (_i_struct.op == 1)
                        _add(std::move(_i_struct));
                    else if (_i_struct.op == 2)
                        _remove(std::move(_i_struct));
                    else if (_i_struct.op == 3)
                    {
                        fncs.clear();
                        // op_fncs.clear();
                    }
                }

                op_fncs.clear();
            }
        }

        // // it is safer to always execute a non - const event call...
        // void operator()(_ArgsType... _arg) const
        // {
        // 	for (const __internal &_i_struct : fncs)
        // 	{
        // 		_i_struct._std_function_functor(std::forward<_ArgsType>(_arg)...);
        // 		// 		if (_i_struct._std_function_functor != nullptr)
        // 		// 			_i_struct._std_function_functor(std::forward<_ArgsType>(_arg)...);
        // 		// 		else
        // 		// 			_i_struct._std_function_class_member(_i_struct._ptr_instance, std::forward<_ArgsType>(_arg)...);
        // 	}
        // }

        operator bool() const
        {
            return fncs.size() > 0;
        }
        operator bool()
        {
            std::lock_guard<decltype(mtx)> lock(mtx);
            return fncs.size() > 0;
        }

        //////////////////////////
        // COPY CONSTRUCTOR
        // COPY OPERATOR
        //////////////////////////
        Event(self_type &_v)
        {
            std::lock_guard<decltype(_v.mtx)> lock_other(_v.mtx);

            _runtime_inside_call = _v._runtime_inside_call;
            fncs.assign(_v.fncs.begin(), _v.fncs.end());
            op_fncs.assign(_v.op_fncs.begin(), _v.op_fncs.end());
        }
        Event(const self_type &_v)
        {
            _runtime_inside_call = _v._runtime_inside_call;
            fncs.assign(_v.fncs.begin(), _v.fncs.end());
            op_fncs.assign(_v.op_fncs.begin(), _v.op_fncs.end());
        }
        self_type &operator=(self_type &_v)
        {
            std::lock_guard<decltype(mtx)> lock_self(mtx);
            std::lock_guard<decltype(_v.mtx)> lock_other(_v.mtx);

            _runtime_inside_call = _v._runtime_inside_call;
            fncs.assign(_v.fncs.begin(), _v.fncs.end());
            op_fncs.assign(_v.op_fncs.begin(), _v.op_fncs.end());
            return *this;
        }
        self_type &operator=(const self_type &_v)
        {
            std::lock_guard<decltype(mtx)> lock_self(mtx);
            std::lock_guard<decltype(_v.mtx)> lock_other(_v.mtx);

            _runtime_inside_call = _v._runtime_inside_call;
            fncs.assign(_v.fncs.begin(), _v.fncs.end());
            op_fncs.assign(_v.op_fncs.begin(), _v.op_fncs.end());
            return *this;
        }

        // rvalue assignment
        Event(self_type &&_v)
        {
            _runtime_inside_call = _v._runtime_inside_call;
            fncs = std::move(_v.fncs);
            op_fncs = std::move(_v.op_fncs);
        }
        self_type &operator=(self_type &&_v)
        {
            std::lock_guard<decltype(mtx)> lock_self(mtx);
            std::lock_guard<decltype(_v.mtx)> lock_other(_v.mtx);

            _runtime_inside_call = _v._runtime_inside_call;
            fncs = std::move(_v.fncs);
            op_fncs = std::move(_v.op_fncs);

            return *this;
        }

        Event()
        {
            _runtime_inside_call = false;
        }

        self_type &operator=(nullptr_t)
        {
            clear();
            return *this;
        }

        bool operator==(nullptr_t) const
        {
            return !(bool)(*this);
        }

        bool operator!=(nullptr_t) const
        {
            return (bool)(*this);
        }

        bool operator==(nullptr_t)
        {
            return !(bool)(*this);
        }

        bool operator!=(nullptr_t)
        {
            return (bool)(*this);
        }

        //////////////////////////
        // ADD PART
        //////////////////////////

        template <typename _unknown_type_,
                  typename std::enable_if<
                      std::is_assignable<std_function_functor, _unknown_type_>::value>::type * = nullptr>
        void add(const _unknown_type_ &_unknown_instance)
        {

            //
            // Can be std_bind, std_function, lambda
            //

            // can use to test the input parameter
            // std::is_bind_expression<_unknown_type_>::value
            // std::is_same<_unknown_type_, std_function_functor>::value

            // static_assert(std::is_assignable<std_function_functor, _unknown_type_>::value,
            // 	"parameter does not match the Event signature."
            // 	);

            __internal struct_;

            // functor
            struct_._ptr_functor = nullptr;
            struct_._std_function_functor = _unknown_instance;

            // object
            struct_._ptr_instance = nullptr;
            struct_._ptr_class_member = nullptr;
            // struct_._std_function_class_member = nullptr;

            struct_.mCallType = CallType::STDFunction;

            _add(std::move(struct_));
        }

        // template <typename _unknown_type_,
        // 	typename std::enable_if<
        // 		std::is_assignable<std_function_functor, _unknown_type_>::value &&
        // 		std::is_rvalue_reference<_unknown_type_>::value
        // 	>::type* = nullptr
        // >
        // void add(_unknown_type_&&_unknown_instance) {

        // 	//
        // 	// Can be std_bind, std_function, lambda
        // 	//

        // 	// can use to test the input parameter
        // 	// std::is_bind_expression<_unknown_type_>::value
        // 	// std::is_same<_unknown_type_, std_function_functor>::value

        // 	// static_assert(std::is_assignable<std_function_functor, _unknown_type_>::value,
        // 	// 	"parameter does not match the Event signature."
        // 	// 	);

        // 	__internal struct_;

        // 	// functor
        // 	struct_._ptr_functor = nullptr;
        // 	struct_._std_function_functor = std::move(_unknown_instance);

        // 	// object
        // 	struct_._ptr_instance = nullptr;
        // 	struct_._ptr_class_member = nullptr;
        // 	struct_._std_function_class_member = nullptr;

        // 	_add(struct_);
        // }

        void add(const compatible_callback &callback)
        {
            __internal struct_;

            // functor
            struct_._ptr_functor = callback._ptr_functor;
            struct_._std_function_functor = callback._std_function_functor;

            // object
            struct_._ptr_instance = callback._ptr_instance;
            struct_._ptr_class_member = callback._ptr_class_member;
            // struct_._std_function_class_member = nullptr;

            struct_.mCallType = callback.mCallType;

            _add(std::move(struct_));
        }

        void add(compatible_callback &&callback)
        {
            __internal struct_;

            // functor
            struct_._ptr_functor = callback._ptr_functor;
            struct_._std_function_functor = std::move(callback._std_function_functor);

            // object
            struct_._ptr_instance = callback._ptr_instance;
            struct_._ptr_class_member = callback._ptr_class_member;
            // struct_._std_function_class_member = nullptr;

            struct_.mCallType = callback.mCallType;

            _add(std::move(struct_));
        }

        void add(ptr_functor ptr_functor_ref)
        {
            //
            // ptr_functor
            //

            __internal struct_;

            // functor
            struct_._ptr_functor = ptr_functor_ref;
            struct_._std_function_functor = nullptr; // std_function_functor(struct_._ptr_functor);

            // object
            struct_._ptr_instance = nullptr;
            struct_._ptr_class_member = nullptr;
            // struct_._std_function_class_member = nullptr;

            struct_.mCallType = CallType::Functor;

            _add(std::move(struct_));
        }

        void add(const std_function_functor &std_function_instance)
        {
            //
            // std_function_functor
            //

            __internal struct_;

            // functor
            struct_._ptr_functor = nullptr;
            struct_._std_function_functor = std_function_instance;

            // object
            struct_._ptr_instance = nullptr;
            struct_._ptr_class_member = nullptr;
            // struct_._std_function_class_member = nullptr;

            struct_.mCallType = CallType::STDFunction;

            _add(std::move(struct_));
        }

        void add(std_function_functor &&std_function_instance)
        {
            //
            // std_function_functor
            //

            __internal struct_;

            // functor
            struct_._ptr_functor = nullptr;
            struct_._std_function_functor = std::move(std_function_instance);

            // object
            struct_._ptr_instance = nullptr;
            struct_._ptr_class_member = nullptr;
            // struct_._std_function_class_member = nullptr;

            struct_.mCallType = CallType::STDFunction;

            _add(std::move(struct_));
        }

        template <typename _ClassTypeA, typename _ClassTypeB>
        void add(_RetType (_ClassTypeA::*class_member)(_ArgsType...), _ClassTypeB *instance)
        {
            using index_sequence_type = typename STL_Tools::stl_extractor<_RetType(_ArgsType...)>::index_sequence_type;
            specialAdd(class_member, (_ClassTypeA *)instance, index_sequence_type());
        }

        template <typename _ClassTypeA, typename _ClassTypeB>
        void add(_RetType (_ClassTypeA::*class_member)(_ArgsType...) const, _ClassTypeB *instance)
        {
            using index_sequence_type = typename STL_Tools::stl_extractor<_RetType(_ArgsType...)>::index_sequence_type;
            specialAdd((_RetType (_ClassTypeA::*)(_ArgsType...))class_member, (_ClassTypeA *)instance, index_sequence_type());
        }

    private:
        template <typename _ClassType, std::size_t... I>
        void specialAdd(_RetType (_ClassType::*class_member)(_ArgsType...), _ClassType *instance, STL_Tools::index_sequence<I...>)
        {

            //
            // ptr_class_member
            //

            static_assert(std::is_base_of<_BaseClassType, _ClassType>::value,
                          "instance need to derive HandleCallback "
                          "to be used as an event receiver. "
                          "Example 'class Obj:public EventCore::HandleCallback'");

            // static_assert(std::is_same<_RetType(_BaseClassType::*)(_ArgsType...), ptr_class_member>::value,
            // 	"parameter does not match the event signature (ptr_class_member)."
            // 	);

            __internal struct_;

            // functor
            struct_._ptr_functor = nullptr;
            // struct_._std_function_functor = nullptr;

            // object
            struct_._ptr_instance = reinterpret_cast<_BaseClassType *>(instance);
#if USE_C_FUNCTOR_PTR_FOR_MEMBER_FUNCTION == true
            struct_._ptr_class_member = *(ptr_class_member_functor *)&class_member;
#else
            struct_._ptr_class_member = *(ptr_class_member *)&class_member;
#endif
            // struct_._std_function_class_member = struct_._ptr_class_member; // std_function_class_member(struct_._ptr_class_member);

            // {
            //     struct_._std_function_functor = std::bind(struct_._ptr_class_member, struct_._ptr_instance,
            //                                               STL_Tools::placeholder<I + 1>{}...);
            // }
            struct_._std_function_functor = nullptr;


            struct_.mCallType = CallType::ClassMember;

            _add(std::move(struct_));
        }

    public:
        //////////////////////////
        // REMOVE PART
        //////////////////////////

        template <typename _unknown_type_,
                  typename std::enable_if<
                      std::is_assignable<std_function_functor, _unknown_type_>::value>::type * = nullptr>
        void remove(const _unknown_type_ &_unknown_instance)
        {

            static_assert(
                std::is_same<ptr_functor, _unknown_type_>::value,
                "cannot remove an Event element against std::bind, std::function, lambda or any other pointer.");

            // static_assert(false,
            // 	"cannot remove std::bind, std::function or lambda."
            // );
        }

        void remove(const compatible_callback &callback)
        {
            __internal struct_;

            // functor
            struct_._ptr_functor = callback._ptr_functor;
            struct_._std_function_functor = nullptr; // callback._std_function_functor;

            // object
            struct_._ptr_instance = callback._ptr_instance;
            struct_._ptr_class_member = callback._ptr_class_member;
            // struct_._std_function_class_member = nullptr;

            struct_.mCallType = callback.mCallType;

            _remove(std::move(struct_));
        }

        void remove(ptr_functor ptr_functor_ref)
        {
            //
            // ptr_functor
            //

            __internal struct_;

            // functor
            struct_._ptr_functor = ptr_functor_ref;
            struct_._std_function_functor = nullptr; // ptr_functor_ref; // std_function_functor(struct_._ptr_functor);

            // object
            struct_._ptr_instance = nullptr;
            struct_._ptr_class_member = nullptr;
            // struct_._std_function_class_member = nullptr;

            struct_.mCallType = CallType::Functor;

            _remove(std::move(struct_));
        }

        template <typename _ClassTypeA, typename _ClassTypeB>
        void remove(_RetType (_ClassTypeA::*class_member)(_ArgsType...), _ClassTypeB *instance)
        {
            using index_sequence_type = typename STL_Tools::stl_extractor<_RetType(_ArgsType...)>::index_sequence_type;
            specialRemove(class_member, (_ClassTypeA *)instance, index_sequence_type());
        }

        template <typename _ClassTypeA, typename _ClassTypeB>
        void remove(_RetType (_ClassTypeA::*class_member)(_ArgsType...) const, _ClassTypeB *instance)
        {
            using index_sequence_type = typename STL_Tools::stl_extractor<_RetType(_ArgsType...)>::index_sequence_type;
            specialRemove((_RetType (_ClassTypeA::*)(_ArgsType...))class_member, (_ClassTypeA *)instance, index_sequence_type());
        }

    private:
        template <typename _ClassType, std::size_t... I>
        void specialRemove(_RetType (_ClassType::*class_member)(_ArgsType...), _ClassType *instance, STL_Tools::index_sequence<I...>)
        {

            //
            // ptr_class_member
            //
            static_assert(std::is_base_of<_BaseClassType, _ClassType>::value,
                          "instance need to derive HandleCallback "
                          "to be used as an event receiver. "
                          "Example 'class Obj:public EventCore::HandleCallback'");

            // static_assert(std::is_same<_RetType(_BaseClassType::*)(_ArgsType...), ptr_class_member>::value,
            // 	"parameter does not match the event signature (ptr_class_member)."
            // 	);

            __internal struct_;

            // functor
            struct_._ptr_functor = nullptr;
            struct_._std_function_functor = nullptr;

            // object
            struct_._ptr_instance = reinterpret_cast<_BaseClassType *>(instance);
#if USE_C_FUNCTOR_PTR_FOR_MEMBER_FUNCTION == true
            struct_._ptr_class_member = *(ptr_class_member_functor *)&class_member;
#else
            struct_._ptr_class_member = *(ptr_class_member *)&class_member;
#endif
            // struct_._std_function_class_member = struct_._ptr_class_member; // std_function_class_member(struct_._ptr_class_member);

            struct_.mCallType = CallType::ClassMember;

            _remove(std::move(struct_));
        }

    public:
        void clear()
        {
            std::lock_guard<decltype(mtx)> lock(mtx);

            if (_runtime_inside_call)
            {
                op_fncs.push_back(__internal{});
                // 1 - add, 2 - remove, 3 - clear
                op_fncs.back().op = 3;
            }
            else
            {
                //_runtime_inside_call = false;
                fncs.clear();
                op_fncs.clear();
            }
        }

        size_t size()
        {
            std::lock_guard<decltype(mtx)> lock(mtx);
            return fncs.size();
        }
    };

    template <typename _RetType, typename... _ArgsType, typename _BaseClassType>
    bool operator==(nullptr_t, const Event<_RetType(_ArgsType...), _BaseClassType> &_a)
    {
        return !(bool)(_a);
    }
    /*template<typename _RetType, typename ..._ArgsType, typename _BaseClassType>
    bool operator!=(const Callback<_RetType(_ArgsType...), _BaseClassType>& _a, nullptr_t) {
        return static_cast<bool>(_a);
    }*/
    template <typename _RetType, typename... _ArgsType, typename _BaseClassType>
    bool operator!=(nullptr_t, const Event<_RetType(_ArgsType...), _BaseClassType> &_a)
    {
        return (bool)(_a);
    }

    template <typename _RetType, typename... _ArgsType, typename _BaseClassType>
    bool operator==(nullptr_t, Event<_RetType(_ArgsType...), _BaseClassType> &_a)
    {
        return !(bool)(_a);
    }
    /*template<typename _RetType, typename ..._ArgsType, typename _BaseClassType>
    bool operator!=(const Callback<_RetType(_ArgsType...), _BaseClassType>& _a, nullptr_t) {
        return static_cast<bool>(_a);
    }*/
    template <typename _RetType, typename... _ArgsType, typename _BaseClassType>
    bool operator!=(nullptr_t, Event<_RetType(_ArgsType...), _BaseClassType> &_a)
    {
        return (bool)(_a);
    }

}

#include "../ITKCommon/ITKAbort.h"

#if defined(_WIN32)
#pragma warning(pop)
#endif
