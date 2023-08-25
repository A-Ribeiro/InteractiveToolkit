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
        };

        template <>
        struct SortIndex<uint32_t>
        {
            uint32_t index;  // current index in the array
            uint32_t toSort; // hash to sort

            static ITK_INLINE bool comparator(const SortIndex<uint32_t> &i1, const SortIndex<uint32_t> &i2)
            {
                return (i1.toSort < i2.toSort);
            }

            static ITK_INLINE SortIndex<uint32_t> Create(uint32_t index, uint32_t toSort)
            {
                SortIndex<uint32_t> result;
                result.index = index;
                result.toSort = toSort;
                return result;
            }
        };

        template <>
        struct SortIndex<int32_t>
        {
            uint32_t index; // current index in the array
            int32_t toSort; // hash to sort

            static ITK_INLINE bool comparator(const SortIndex<int32_t> &i1, const SortIndex<int32_t> &i2)
            {
                return (i1.toSort < i2.toSort);
            }

            static ITK_INLINE SortIndex<int32_t> Create(uint32_t index, int32_t toSort)
            {
                SortIndex<int32_t> result;
                result.index = index;
                result.toSort = toSort;
                return result;
            }
        };

        using SortIndexu = SortIndex<uint32_t>;
        using SortIndexi = SortIndex<int32_t>;

    }
}