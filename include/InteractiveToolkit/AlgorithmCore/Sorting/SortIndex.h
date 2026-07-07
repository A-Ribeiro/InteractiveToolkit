#pragma once

#include "../../common.h"
#include "../../ITKCommon/ITKCommon.h"

// #include <type_traits>

namespace AlgorithmCore
{

    namespace Sorting
    {

        template <typename _type, typename Enable = void>
        struct SortIndex;

        template <typename _type>
        struct SortIndex<_type, typename std::enable_if<(sizeof(_type) == 8), void>::type>
        {
            _type toSort;   // hash to sort
            uint32_t index; // current index in the array, max of 4294967296 elements in the array
        private:
            uint32_t _padding_for_gpu_compute = 0; // padding for GPU compute, to make the struct 16 bytes aligned
        public:

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

        template <typename _type>
        struct SortIndex<_type, typename std::enable_if<(sizeof(_type) == 4), void>::type>
        {
            _type toSort;   // hash to sort
            uint32_t index; // current index in the array, max of 4294967296 elements in the array

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

        using SortIndexu32 = SortIndex<uint32_t>;
        using SortIndexi32 = SortIndex<int32_t>;

        using SortIndexu64 = SortIndex<uint64_t>;
        using SortIndexi64 = SortIndex<int64_t>;

    }
}
