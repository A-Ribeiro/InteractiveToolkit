#pragma once

#include "../../common.h"
#include "../../ITKCommon/ITKCommon.h"

namespace AlgorithmCore
{

    namespace Sorting
    {

        template <typename _type>
        struct SortIndex
        {
            uint32_t index;  // current index in the array, max of 4294967296 elements in the array
            _type toSort; // hash to sort

            static ITK_INLINE bool comparator(const SortIndex<_type> &i1, const SortIndex<_type> &i2)
            {
                return (i1.toSort < i2.toSort);
            }

            static ITK_INLINE SortIndex<_type> Create(uint32_t index, _type toSort)
            {
                SortIndex<_type> result;
                result.index = index;
                result.toSort = toSort;
                return result;
            }
        };

        // template <>
        // struct SortIndex<uint32_t>
        // {
        //     uint32_t index;  // current index in the array
        //     uint32_t toSort; // hash to sort

        //     static ITK_INLINE bool comparator(const SortIndex<uint32_t> &i1, const SortIndex<uint32_t> &i2)
        //     {
        //         return (i1.toSort < i2.toSort);
        //     }

        //     static ITK_INLINE SortIndex<uint32_t> Create(uint32_t index, uint32_t toSort)
        //     {
        //         SortIndex<uint32_t> result;
        //         result.index = index;
        //         result.toSort = toSort;
        //         return result;
        //     }
        // };

        // template <>
        // struct SortIndex<int32_t>
        // {
        //     uint32_t index; // current index in the array
        //     int32_t toSort; // hash to sort

        //     static ITK_INLINE bool comparator(const SortIndex<int32_t> &i1, const SortIndex<int32_t> &i2)
        //     {
        //         return (i1.toSort < i2.toSort);
        //     }

        //     static ITK_INLINE SortIndex<int32_t> Create(uint32_t index, int32_t toSort)
        //     {
        //         SortIndex<int32_t> result;
        //         result.index = index;
        //         result.toSort = toSort;
        //         return result;
        //     }
        // };

        // template <>
        // struct SortIndex<uint64_t>
        // {
        //     uint32_t index;  // current index in the array
        //     uint64_t toSort; // hash to sort

        //     static ITK_INLINE bool comparator(const SortIndex<uint64_t> &i1, const SortIndex<uint64_t> &i2)
        //     {
        //         return (i1.toSort < i2.toSort);
        //     }

        //     static ITK_INLINE SortIndex<uint64_t> Create(uint32_t index, uint64_t toSort)
        //     {
        //         SortIndex<uint64_t> result;
        //         result.index = index;
        //         result.toSort = toSort;
        //         return result;
        //     }
        // };

        // template <>
        // struct SortIndex<int64_t>
        // {
        //     uint32_t index; // current index in the array
        //     int64_t toSort; // hash to sort

        //     static ITK_INLINE bool comparator(const SortIndex<int64_t> &i1, const SortIndex<int64_t> &i2)
        //     {
        //         return (i1.toSort < i2.toSort);
        //     }

        //     static ITK_INLINE SortIndex<int64_t> Create(uint32_t index, int64_t toSort)
        //     {
        //         SortIndex<int64_t> result;
        //         result.index = index;
        //         result.toSort = toSort;
        //         return result;
        //     }
        // };

        using SortIndexu32 = SortIndex<uint32_t>;
        using SortIndexi32 = SortIndex<int32_t>;

        using SortIndexu64 = SortIndex<uint64_t>;
        using SortIndexi64 = SortIndex<int64_t>;

    }
}