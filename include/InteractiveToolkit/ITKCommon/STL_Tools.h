#pragma once

#include <algorithm>
#include <vector>
#include <functional>
#include <type_traits>
#include <memory>

#include "../common.h"

namespace STL_Tools
{
    // need a base class... otherwise, cannot extract sub-types
    template <typename _single_element_>
    struct stl_extractor;

    template <typename... _ArgsType>
    struct stl_aux;

    //
    // Functor types extraction
    //
    template <typename _RetType, typename... _ArgsType>
    struct stl_extractor<_RetType(_ArgsType...)>
    {
        typedef std::function<_RetType(_ArgsType...)> std_function_functor;
        typedef _RetType (*ptr_functor)(_ArgsType...);

        // typedef std::tuple<_ArgsType...> std_tuple_arguments;
        typedef stl_aux<_ArgsType...> std_arguments;

        typedef _RetType return_type;
    };

    //
    // Class types extraction
    //
    template <typename _ClassType, typename _RetType, typename... _ArgsType>
    struct stl_extractor<std::tuple<_ClassType, _RetType, stl_aux<_ArgsType...>>>
    {
        typedef std::function<_RetType(_ClassType *, _ArgsType...)> std_function_class_member;
        typedef _RetType (_ClassType::*ptr_class_member)(_ArgsType...);
    };

    template <size_t... _ints_>
    struct index_sequence
    {
        using type = index_sequence;
        using value_type = size_t;
        static constexpr std::size_t size() noexcept { return sizeof...(_ints_); }
    };

    template <size_t _index_>
    struct make_index_sequence
    {
    };

    template <>
    struct make_index_sequence<0> : index_sequence<>
    {
    };

#define __template_make_index_sequence(i, ...)                  \
    template <>                                                 \
    struct make_index_sequence<i> : index_sequence<__VA_ARGS__> \
    {                                                           \
    }
    __template_make_index_sequence(1, 0);
    __template_make_index_sequence(2, 0, 1);
    __template_make_index_sequence(3, 0, 1, 2);
    __template_make_index_sequence(4, 0, 1, 2, 3);
    __template_make_index_sequence(5, 0, 1, 2, 3, 4);
    __template_make_index_sequence(6, 0, 1, 2, 3, 4, 5);
    __template_make_index_sequence(7, 0, 1, 2, 3, 4, 5, 6);
    __template_make_index_sequence(8, 0, 1, 2, 3, 4, 5, 6, 7);
    __template_make_index_sequence(9, 0, 1, 2, 3, 4, 5, 6, 7, 8);
    __template_make_index_sequence(10, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    __template_make_index_sequence(11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    __template_make_index_sequence(12, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11);
    __template_make_index_sequence(13, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12);
    __template_make_index_sequence(14, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13);
    __template_make_index_sequence(15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14);
    __template_make_index_sequence(16, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
    __template_make_index_sequence(17, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
    __template_make_index_sequence(18, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17);
    __template_make_index_sequence(19, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18);
    __template_make_index_sequence(20, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19);

#undef __template_make_index_sequence

    template <size_t _aux_>
    struct index_to_placeholder
    {
    };
#define __template_index_to_placeholder(i)              \
    template <>                                         \
    struct index_to_placeholder<i>                      \
    {                                                   \
        using type = decltype(std::placeholders::_##i); \
    }

    __template_index_to_placeholder(1);
    __template_index_to_placeholder(2);
    __template_index_to_placeholder(3);
    __template_index_to_placeholder(4);
    __template_index_to_placeholder(5);
    __template_index_to_placeholder(6);
    __template_index_to_placeholder(7);
    __template_index_to_placeholder(8);
    __template_index_to_placeholder(9);
    __template_index_to_placeholder(10); // MacOS clang limit

#if defined(_WIN32) || defined(__linux__) //|| defined(__APPLE__)
// __template_index_to_placeholder(11);
// __template_index_to_placeholder(12);
// __template_index_to_placeholder(13);
// __template_index_to_placeholder(14);
// __template_index_to_placeholder(15);
// __template_index_to_placeholder(16);
// __template_index_to_placeholder(17);
// __template_index_to_placeholder(18);
// __template_index_to_placeholder(19);
// __template_index_to_placeholder(20); // Visual studio limit
#endif

    // __template_index_to_placeholder(21);
    // __template_index_to_placeholder(22);
    // __template_index_to_placeholder(23);
    // __template_index_to_placeholder(24);
    // __template_index_to_placeholder(25);
    // __template_index_to_placeholder(26);
    // __template_index_to_placeholder(27);
    // __template_index_to_placeholder(28); // Linux gcc limit

#undef __template_index_to_placeholder

    template <int _id_>
    using placeholder = typename index_to_placeholder<_id_>::type;

    template <typename _base_type, typename... _param_args>
    ITK_INLINE std::unique_ptr<_base_type> make_unique(_param_args &&...args)
    {
        return std::unique_ptr<_base_type>(new _base_type(std::forward<_param_args>(args)...));
    }

    template <template <typename, typename> class ContainerT, typename ValueT, typename AllocatorT>
    class Reversal_Impl
    {
    public:
        using _ListT = ContainerT<ValueT, AllocatorT>;
        _ListT &_obj;

        Reversal_Impl(_ListT &obj) : _obj(obj) {}
        typename _ListT::reverse_iterator begin() { return _obj.rbegin(); }
        typename _ListT::reverse_iterator end() { return _obj.rend(); }
    };

    template <template <typename, typename> class ContainerT, typename ValueT, typename AllocatorT>
    class Reversal_Impl_Const
    {
    public:
        using _ListT = const ContainerT<ValueT, AllocatorT>;
        _ListT &_obj;

        Reversal_Impl_Const(_ListT &obj) : _obj(obj) {}
        typename _ListT::const_reverse_iterator begin() const { return _obj.rbegin(); }
        typename _ListT::const_reverse_iterator end() const { return _obj.rend(); }
    };

    template <template <typename, typename> class ContainerT, typename ValueT, typename AllocatorT>
    static inline Reversal_Impl<ContainerT, ValueT, AllocatorT> Reversal(ContainerT<ValueT, AllocatorT> &obj)
    {
        return Reversal_Impl<ContainerT, ValueT, AllocatorT>(obj);
    }

    template <template <typename, typename> class ContainerT, typename ValueT, typename AllocatorT>
    static inline Reversal_Impl_Const<ContainerT, ValueT, AllocatorT> Reversal(const ContainerT<ValueT, AllocatorT> &obj)
    {
        return Reversal_Impl_Const<ContainerT, ValueT, AllocatorT>(obj);
    }

}
