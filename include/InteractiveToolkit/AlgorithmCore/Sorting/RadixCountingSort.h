#pragma once

#include "../../common.h"
#include "SortIndex.h"
#include "SortTool.h"

namespace AlgorithmCore
{

    namespace Sorting
    {
        template <typename _type>
        struct RadixCountingSort
        {
        };

        /// \brief Radix sort using Counting sort inside it for 'uint32_t' type
        ///
        /// Radix sort implementation
        /// _________________________
        ///
        /// The radix sort separates the number by its digit.
        ///
        /// The natural base is 10 (didatic).
        ///
        /// This algorithm uses the 'base_bits' to setup a binary base over the type 'uint32_t'.
        ///
        /// This binary base allow us to use the shift and bitwise operators to perform better on CPUs.
        ///
        /// Counting sort implementation
        /// _________________________
        ///
        /// The counting sort is used to sort the individual digits resulting from the radix.
        ///
        /// This implementation allow the pre-allocation of the temporary buffer outside of the function.
        ///
        /// If the 'tmp_array' is NULL, then the array is allocated and freed according the 'arrSize' parameter.
        ///
        /// Example:
        ///
        /// \code
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct RadixCountingSort<uint32_t>
        {
            using sortIndexType = SortIndex<uint32_t>;

            static ITK_INLINE void sort(uint32_t *_arr, uint32_t arrSize, uint32_t *tmp_array = NULL)
            {

#if defined(ITK_SSE2)

                if (arrSize == 0)
                    return;

                // Counting Sort
                __m128i counting[256];
                uint32_t *aux;

                if (tmp_array == NULL)
                    aux = (uint32_t *)ITKCommon::Memory::malloc(arrSize * sizeof(uint32_t));
                else
                    aux = tmp_array;

                uint32_t *in = _arr;
                uint32_t *out = aux;

                // for (int32_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(counting, 0, sizeof(__m128i) * 256);

                    const __m128i _c_0 = _mm_setr_epi32(1, 0, 0, 0);
                    const __m128i _c_1 = _mm_setr_epi32(0, 1, 0, 0);
                    const __m128i _c_2 = _mm_setr_epi32(0, 0, 1, 0);
                    const __m128i _c_3 = _mm_setr_epi32(0, 0, 0, 1);

                    // count the elements
                    for (uint32_t j = 0; j < arrSize; j++)
                    {

                        uint32_t currItem = in[j];
                        uint32_t index_0 = (currItem)&0xff;
                        uint32_t index_1 = (currItem >> 8) & 0xff;
                        uint32_t index_2 = (currItem >> 16) & 0xff;
                        uint32_t index_3 = (currItem >> 24) & 0xff;

                        _mm_u32_(counting[index_0], 0)++;
                        _mm_u32_(counting[index_1], 1)++;
                        _mm_u32_(counting[index_2], 2)++;
                        _mm_u32_(counting[index_3], 3)++;

                        /*counting[index_0] = _mm_add_epi32(counting[index_0], _c_0);
                        counting[index_1] = _mm_add_epi32(counting[index_1], _c_1);
                        counting[index_2] = _mm_add_epi32(counting[index_2], _c_2);
                        counting[index_3] = _mm_add_epi32(counting[index_3], _c_3);*/
                    }

                    // compute offsets
                    __m128i acc = _mm_set1_epi32(0);

                    for (uint32_t j = 0; j < 256; j++)
                    {
                        __m128i tmp = counting[j];
                        counting[j] = acc;
                        acc = _mm_add_epi32(acc, tmp);
                    }

                    // shift = 0
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            uint32_t currItem = in[j];
                            uint32_t bucket_index = ((currItem)&0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 0);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_0);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        uint32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 8
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            uint32_t currItem = in[j];
                            uint32_t bucket_index = ((currItem >> 8) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 1);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_1);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        uint32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 16
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            uint32_t currItem = in[j];
                            uint32_t bucket_index = ((currItem >> 16) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 2);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_2);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        uint32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 24
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            uint32_t currItem = in[j];
                            uint32_t bucket_index = ((currItem >> 24) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 3);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_3);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        uint32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == NULL)
                    ITKCommon::Memory::free(aux);

#else

                if (arrSize == 0)
                    return;

                // Counting Sort
                uint32_t counting[256][4];
                uint32_t *aux;

                if (tmp_array == NULL)
                    aux = (uint32_t *)ITKCommon::Memory::malloc(arrSize * sizeof(uint32_t));
                else
                    aux = tmp_array;

                uint32_t *in = _arr;
                uint32_t *out = aux;

                // for (int32_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(&counting[0][0], 0, sizeof(uint32_t) * 256 * 4);

                    // count the elements
                    for (uint32_t j = 0; j < arrSize; j++)
                    {
                        uint32_t currItem = in[j];
                        counting[(currItem)&0xff][0]++;
                        counting[(currItem >> 8) & 0xff][1]++;
                        counting[(currItem >> 16) & 0xff][2]++;
                        counting[(currItem >> 24) & 0xff][3]++;
                    }

                    // compute offsets
                    // uint32_t acc[4];
                    // memcpy(acc, counting, sizeof(uint32_t) << 2);
                    // memset(counting, 0, sizeof(uint32_t) << 2);

                    //uint32_t acc[4] = {counting[0][0], counting[0][1], counting[0][2], counting[0][3]};

                    // counting[0][0] = 0;
                    // counting[0][1] = 0;
                    // counting[0][2] = 0;
                    // counting[0][3] = 0;

                    uint32_t acc[4] = {0, 0, 0, 0};

                    // uint32_t tmp[4];

                    for (uint32_t j = 0; j < 256; j++)
                    {

                        // memcpy(tmp, counting[j], sizeof(uint32_t) << 2);
                        // memcpy(counting[j], acc, sizeof(uint32_t) << 2);

                        uint32_t tmp[4] = {counting[j][0], counting[j][1], counting[j][2], counting[j][3]};

                        counting[j][0] = acc[0];
                        counting[j][1] = acc[1];
                        counting[j][2] = acc[2];
                        counting[j][3] = acc[3];

                        acc[0] += tmp[0];
                        acc[1] += tmp[1];
                        acc[2] += tmp[2];
                        acc[3] += tmp[3];
                    }

                    // for (uint32_t shift = 0; shift < 32; shift += 8)
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            uint32_t currItem = in[j];
                            uint32_t bucket_index = ((currItem >> 0) & 0xff);
                            uint32_t out_index = counting[bucket_index][0];
                            counting[bucket_index][0]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        uint32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            uint32_t currItem = in[j];
                            uint32_t bucket_index = ((currItem >> 8) & 0xff);
                            uint32_t out_index = counting[bucket_index][1];
                            counting[bucket_index][1]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        uint32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            uint32_t currItem = in[j];
                            uint32_t bucket_index = ((currItem >> 16) & 0xff);
                            uint32_t out_index = counting[bucket_index][2];
                            counting[bucket_index][2]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        uint32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            uint32_t currItem = in[j];
                            uint32_t bucket_index = ((currItem >> 24) & 0xff);
                            uint32_t out_index = counting[bucket_index][3];
                            counting[bucket_index][3]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        uint32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == NULL)
                    ITKCommon::Memory::free(aux);

#endif
            }

            static ITK_INLINE void sortIndex(sortIndexType *_arr, uint32_t arrSize, sortIndexType *tmp_array = NULL)
            {
#if defined(ITK_SSE2)

                if (arrSize == 0)
                    return;

                // Counting Sort
                __m128i counting[256];
                sortIndexType *aux;

                if (tmp_array == NULL)
                    aux = (sortIndexType *)ITKCommon::Memory::malloc(arrSize * sizeof(sortIndexType));
                else
                    aux = tmp_array;

                sortIndexType *in = _arr;
                sortIndexType *out = aux;

                // for (int32_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(counting, 0, sizeof(__m128i) * 256);

                    const __m128i _c_0 = _mm_setr_epi32(1, 0, 0, 0);
                    const __m128i _c_1 = _mm_setr_epi32(0, 1, 0, 0);
                    const __m128i _c_2 = _mm_setr_epi32(0, 0, 1, 0);
                    const __m128i _c_3 = _mm_setr_epi32(0, 0, 0, 1);

                    // count the elements
                    for (uint32_t j = 0; j < arrSize; j++)
                    {

                        uint32_t currItem = in[j].toSort;
                        uint32_t index_0 = (currItem)&0xff;
                        uint32_t index_1 = (currItem >> 8) & 0xff;
                        uint32_t index_2 = (currItem >> 16) & 0xff;
                        uint32_t index_3 = (currItem >> 24) & 0xff;

                        _mm_u32_(counting[index_0], 0)++;
                        _mm_u32_(counting[index_1], 1)++;
                        _mm_u32_(counting[index_2], 2)++;
                        _mm_u32_(counting[index_3], 3)++;

                        /*counting[index_0] = _mm_add_epi32(counting[index_0], _c_0);
                        counting[index_1] = _mm_add_epi32(counting[index_1], _c_1);
                        counting[index_2] = _mm_add_epi32(counting[index_2], _c_2);
                        counting[index_3] = _mm_add_epi32(counting[index_3], _c_3);*/
                    }

                    // compute offsets
                    __m128i acc = _mm_set1_epi32(0);

                    for (uint32_t j = 0; j < 256; j++)
                    {
                        __m128i tmp = counting[j];
                        counting[j] = acc;
                        acc = _mm_add_epi32(acc, tmp);
                    }

                    // shift = 0
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = ((currItem.toSort) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 0);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_0);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 8
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = ((currItem.toSort >> 8) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 1);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_1);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 16
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = ((currItem.toSort >> 16) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 2);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_2);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 24
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = ((currItem.toSort >> 24) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 3);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_3);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == NULL)
                    ITKCommon::Memory::free(aux);

#else

                if (arrSize == 0)
                    return;

                // Counting Sort
                uint32_t counting[256][4];
                sortIndexType *aux;

                if (tmp_array == NULL)
                    aux = (sortIndexType *)ITKCommon::Memory::malloc(arrSize * sizeof(sortIndexType));
                else
                    aux = tmp_array;

                sortIndexType *in = _arr;
                sortIndexType *out = aux;

                // for (int32_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(&counting[0][0], 0, sizeof(uint32_t) * 256 * 4);

                    // count the elements
                    for (uint32_t j = 0; j < arrSize; j++)
                    {
                        uint32_t currItem = in[j].toSort;
                        counting[(currItem)&0xff][0]++;
                        counting[(currItem >> 8) & 0xff][1]++;
                        counting[(currItem >> 16) & 0xff][2]++;
                        counting[(currItem >> 24) & 0xff][3]++;
                    }

                    // compute offsets
                    // uint32_t acc[4];
                    // memcpy(acc, counting, sizeof(uint32_t) << 2);
                    // memset(counting, 0, sizeof(uint32_t) << 2);

                    //uint32_t acc[4] = {counting[0][0], counting[0][1], counting[0][2], counting[0][3]};

                    // counting[0][0] = 0;
                    // counting[0][1] = 0;
                    // counting[0][2] = 0;
                    // counting[0][3] = 0;

                    uint32_t acc[4] = {0, 0, 0, 0};

                    // uint32_t tmp[4];

                    for (uint32_t j = 0; j < 256; j++)
                    {

                        // memcpy(tmp, counting[j], sizeof(uint32_t) << 2);
                        // memcpy(counting[j], acc, sizeof(uint32_t) << 2);

                        uint32_t tmp[4] = {counting[j][0], counting[j][1], counting[j][2], counting[j][3]};

                        counting[j][0] = acc[0];
                        counting[j][1] = acc[1];
                        counting[j][2] = acc[2];
                        counting[j][3] = acc[3];

                        acc[0] += tmp[0];
                        acc[1] += tmp[1];
                        acc[2] += tmp[2];
                        acc[3] += tmp[3];
                    }

                    // for (uint32_t shift = 0; shift < 32; shift += 8)
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = ((currItem.toSort >> 0) & 0xff);
                            uint32_t out_index = counting[bucket_index][0];
                            counting[bucket_index][0]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = ((currItem.toSort >> 8) & 0xff);
                            uint32_t out_index = counting[bucket_index][1];
                            counting[bucket_index][1]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = ((currItem.toSort >> 16) & 0xff);
                            uint32_t out_index = counting[bucket_index][2];
                            counting[bucket_index][2]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = ((currItem.toSort >> 24) & 0xff);
                            uint32_t out_index = counting[bucket_index][3];
                            counting[bucket_index][3]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == NULL)
                    ITKCommon::Memory::free(aux);

#endif
            }
        };

        /// \brief Radix sort using Counting sort inside it for 'int32_t' type
        ///
        /// Radix sort implementation
        /// _________________________
        ///
        /// The radix sort separates the number by its digit.
        ///
        /// The natural base is 10 (didatic).
        ///
        /// This algorithm uses the 'base_bits' to setup a binary base over the type 'int32_t'.
        ///
        /// This binary base allow us to use the shift and bitwise operators to perform better on CPUs.
        ///
        /// Counting sort implementation
        /// _________________________
        ///
        /// The counting sort is used to sort the individual digits resulting from the radix.
        ///
        /// This implementation allow the pre-allocation of the temporary buffer outside of the function.
        ///
        /// If the 'tmp_array' is NULL, then the array is allocated and freed according the 'arrSize' parameter.
        ///
        /// Example:
        ///
        /// \code
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct RadixCountingSort<int32_t>
        {
            using sortIndexType = SortIndex<int32_t>;

            static ITK_INLINE void sort(int32_t *_arr, uint32_t arrSize, int32_t *tmp_array = NULL)
            {
#if defined(ITK_SSE2)

                if (arrSize == 0)
                    return;

                // Counting Sort
                __m128i counting[256];
                int32_t *aux;

                if (tmp_array == NULL)
                    aux = (int32_t *)ITKCommon::Memory::malloc(arrSize * sizeof(int32_t));
                else
                    aux = tmp_array;

                int32_t *in = _arr;
                int32_t *out = aux;

                // for (int32_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(counting, 0, sizeof(__m128i) * 256);

                    const __m128i _c_0 = _mm_setr_epi32(1, 0, 0, 0);
                    const __m128i _c_1 = _mm_setr_epi32(0, 1, 0, 0);
                    const __m128i _c_2 = _mm_setr_epi32(0, 0, 1, 0);
                    const __m128i _c_3 = _mm_setr_epi32(0, 0, 0, 1);

                    // count the elements
                    const uint32_t _minus_128 = (uint32_t)(uint8_t)-128;
                    for (uint32_t j = 0; j < arrSize; j++)
                    {

                        uint32_t currItem = (uint32_t)in[j];
                        uint32_t index_0 = (currItem)&0xff;
                        uint32_t index_1 = (currItem >> 8) & 0xff;
                        uint32_t index_2 = (currItem >> 16) & 0xff;
                        uint32_t index_3 = ((currItem >> 24) ^ _minus_128) & 0xff;

                        _mm_u32_(counting[index_0], 0)++;
                        _mm_u32_(counting[index_1], 1)++;
                        _mm_u32_(counting[index_2], 2)++;
                        _mm_u32_(counting[index_3], 3)++;

                        /*counting[index_0] = _mm_add_epi32(counting[index_0], _c_0);
                        counting[index_1] = _mm_add_epi32(counting[index_1], _c_1);
                        counting[index_2] = _mm_add_epi32(counting[index_2], _c_2);
                        counting[index_3] = _mm_add_epi32(counting[index_3], _c_3);*/
                    }

                    // compute offsets
                    __m128i acc = _mm_set1_epi32(0);

                    for (uint32_t j = 0; j < 256; j++)
                    {
                        __m128i tmp = counting[j];
                        counting[j] = acc;
                        acc = _mm_add_epi32(acc, tmp);
                    }

                    // shift = 0
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            int32_t currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 0);
                            
                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_0);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 8
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            int32_t currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem >> 8) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 1);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_1);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 16
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            int32_t currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem >> 16) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 2);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_2);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 24
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            int32_t currItem = in[j];
                            uint32_t bucket_index = ((((int32_t)currItem >> 24) ^ _minus_128) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 3);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_3);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == NULL)
                    ITKCommon::Memory::free(aux);

#else

                if (arrSize == 0)
                    return;

                // Counting Sort
                uint32_t counting[256][4];
                int32_t *aux;

                if (tmp_array == NULL)
                    aux = (int32_t *)ITKCommon::Memory::malloc(arrSize * sizeof(int32_t));
                else
                    aux = tmp_array;

                int32_t *in = _arr;
                int32_t *out = aux;

                // for (int32_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(&counting[0][0], 0, sizeof(uint32_t) * 256 * 4);

                    // count the elements
                    const uint32_t _minus_128 = (uint32_t)(uint8_t)-128;
                    for (uint32_t j = 0; j < arrSize; j++)
                    {
                        uint32_t currItem = (uint32_t)in[j];
                        counting[(currItem)&0xff][0]++;
                        counting[(currItem >> 8) & 0xff][1]++;
                        counting[(currItem >> 16) & 0xff][2]++;
                        counting[((currItem >> 24) ^ _minus_128) & 0xff][3]++;
                    }

                    // compute offsets
                    // uint32_t acc[4];
                    // memcpy(acc, counting, sizeof(uint32_t) << 2);
                    // memset(counting, 0, sizeof(uint32_t) << 2);

                    //uint32_t acc[4] = {counting[0][0], counting[0][1], counting[0][2], counting[0][3]};

                    // counting[0][0] = 0;
                    // counting[0][1] = 0;
                    // counting[0][2] = 0;
                    // counting[0][3] = 0;

                    uint32_t acc[4] = {0, 0, 0, 0};

                    // uint32_t tmp[4];

                    for (uint32_t j = 0; j < 256; j++)
                    {

                        // memcpy(tmp, counting[j], sizeof(uint32_t) << 2);
                        // memcpy(counting[j], acc, sizeof(uint32_t) << 2);

                        uint32_t tmp[4] = {counting[j][0], counting[j][1], counting[j][2], counting[j][3]};

                        counting[j][0] = acc[0];
                        counting[j][1] = acc[1];
                        counting[j][2] = acc[2];
                        counting[j][3] = acc[3];

                        acc[0] += tmp[0];
                        acc[1] += tmp[1];
                        acc[2] += tmp[2];
                        acc[3] += tmp[3];
                    }

                    // for (uint32_t shift = 0; shift < 32; shift += 8)
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            int32_t currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem >> 0) & 0xff);
                            uint32_t out_index = counting[bucket_index][0];
                            counting[bucket_index][0]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            int32_t currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem >> 8) & 0xff);
                            uint32_t out_index = counting[bucket_index][1];
                            counting[bucket_index][1]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            int32_t currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem >> 16) & 0xff);
                            uint32_t out_index = counting[bucket_index][2];
                            counting[bucket_index][2]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            int32_t currItem = in[j];
                            uint32_t bucket_index = ((((uint32_t)currItem >> 24) ^ _minus_128) & 0xff);
                            uint32_t out_index = counting[bucket_index][3];
                            counting[bucket_index][3]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int32_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == NULL)
                    ITKCommon::Memory::free(aux);

#endif
            }

            static ITK_INLINE void sortIndex(sortIndexType *_arr, uint32_t arrSize, sortIndexType *tmp_array = NULL)
            {
#if defined(ITK_SSE2)

                if (arrSize == 0)
                    return;

                // Counting Sort
                __m128i counting[256];
                sortIndexType *aux;

                if (tmp_array == NULL)
                    aux = (sortIndexType *)ITKCommon::Memory::malloc(arrSize * sizeof(sortIndexType));
                else
                    aux = tmp_array;

                sortIndexType *in = _arr;
                sortIndexType *out = aux;

                // for (int32_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(counting, 0, sizeof(__m128i) * 256);

                    const __m128i _c_0 = _mm_setr_epi32(1, 0, 0, 0);
                    const __m128i _c_1 = _mm_setr_epi32(0, 1, 0, 0);
                    const __m128i _c_2 = _mm_setr_epi32(0, 0, 1, 0);
                    const __m128i _c_3 = _mm_setr_epi32(0, 0, 0, 1);

                    // count the elements
                    const uint32_t _minus_128 = (uint32_t)(uint8_t)-128;
                    for (uint32_t j = 0; j < arrSize; j++)
                    {

                        uint32_t currItem = (uint32_t)in[j].toSort;
                        uint32_t index_0 = (currItem)&0xff;
                        uint32_t index_1 = (currItem >> 8) & 0xff;
                        uint32_t index_2 = (currItem >> 16) & 0xff;
                        uint32_t index_3 = ((currItem >> 24) ^ _minus_128) & 0xff;

                        _mm_u32_(counting[index_0], 0)++;
                        _mm_u32_(counting[index_1], 1)++;
                        _mm_u32_(counting[index_2], 2)++;
                        _mm_u32_(counting[index_3], 3)++;

                        /*counting[index_0] = _mm_add_epu32(counting[index_0], _c_0);
                        counting[index_1] = _mm_add_epu32(counting[index_1], _c_1);
                        counting[index_2] = _mm_add_epu32(counting[index_2], _c_2);
                        counting[index_3] = _mm_add_epu32(counting[index_3], _c_3);*/
                    }

                    // compute offsets
                    __m128i acc = _mm_set1_epi32(0);
                    
                    for (uint32_t j = 0; j < 256; j++)
                    {
                        __m128i tmp = counting[j];
                        counting[j] = acc;
                        acc = _mm_add_epi32(acc, tmp);
                    }

                    // shift = 0
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem.toSort) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 0);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_0);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 8
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem.toSort >> 8) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 1);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_1);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 16
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem.toSort >> 16) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 2);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_2);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    // shift = 24
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = ((((int32_t)currItem.toSort >> 24) ^ _minus_128) & 0xff);

                            uint32_t out_index = _mm_u32_(counting[bucket_index], 3);

                            counting[bucket_index] = _mm_add_epi32(counting[bucket_index], _c_3);

                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == NULL)
                    ITKCommon::Memory::free(aux);

#else

                if (arrSize == 0)
                    return;

                // Counting Sort
                uint32_t counting[256][4];
                sortIndexType *aux;

                if (tmp_array == NULL)
                    aux = (sortIndexType *)ITKCommon::Memory::malloc(arrSize * sizeof(sortIndexType));
                else
                    aux = tmp_array;

                sortIndexType *in = _arr;
                sortIndexType *out = aux;

                // for (int32_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(&counting[0][0], 0, sizeof(uint32_t) * 256 * 4);

                    // count the elements
                    const uint32_t _minus_128 = (uint32_t)(uint8_t)-128;
                    for (uint32_t j = 0; j < arrSize; j++)
                    {
                        uint32_t currItem = (uint32_t)in[j].toSort;
                        counting[(currItem)&0xff][0]++;
                        counting[(currItem >> 8) & 0xff][1]++;
                        counting[(currItem >> 16) & 0xff][2]++;
                        counting[((currItem >> 24) ^ _minus_128) & 0xff][3]++;
                    }

                    // compute offsets
                    // uint32_t acc[4];
                    // memcpy(acc, counting, sizeof(uint32_t) << 2);
                    // memset(counting, 0, sizeof(uint32_t) << 2);

                    //uint32_t acc[4] = {counting[0][0], counting[0][1], counting[0][2], counting[0][3]};

                    // counting[0][0] = 0;
                    // counting[0][1] = 0;
                    // counting[0][2] = 0;
                    // counting[0][3] = 0;

                    uint32_t acc[4] = {0, 0, 0, 0};

                    // uint32_t tmp[4];

                    for (uint32_t j = 0; j < 256; j++)
                    {

                        // memcpy(tmp, counting[j], sizeof(uint32_t) << 2);
                        // memcpy(counting[j], acc, sizeof(uint32_t) << 2);

                        uint32_t tmp[4] = {counting[j][0], counting[j][1], counting[j][2], counting[j][3]};

                        counting[j][0] = acc[0];
                        counting[j][1] = acc[1];
                        counting[j][2] = acc[2];
                        counting[j][3] = acc[3];

                        acc[0] += tmp[0];
                        acc[1] += tmp[1];
                        acc[2] += tmp[2];
                        acc[3] += tmp[3];
                    }

                    // for (uint32_t shift = 0; shift < 32; shift += 8)
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem.toSort >> 0) & 0xff);
                            uint32_t out_index = counting[bucket_index][0];
                            counting[bucket_index][0]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem.toSort >> 8) & 0xff);
                            uint32_t out_index = counting[bucket_index][1];
                            counting[bucket_index][1]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = (((uint32_t)currItem.toSort >> 16) & 0xff);
                            uint32_t out_index = counting[bucket_index][2];
                            counting[bucket_index][2]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint32_t bucket_index = ((((uint32_t)currItem.toSort >> 24) ^ _minus_128) & 0xff);
                            uint32_t out_index = counting[bucket_index][3];
                            counting[bucket_index][3]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == NULL)
                    ITKCommon::Memory::free(aux);

#endif
            }
        };

        using RadixCountingSortu = RadixCountingSort<uint32_t>;
        using RadixCountingSorti = RadixCountingSort<int32_t>;

    }
}