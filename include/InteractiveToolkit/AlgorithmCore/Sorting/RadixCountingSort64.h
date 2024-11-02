#pragma once

#include "../../common.h"
#include "SortIndex.h"
#include "SortTool.h"

namespace AlgorithmCore
{

    namespace Sorting
    {

        /// \brief Radix sort using Counting sort inside it for 'uint64_t' type
        ///
        /// Radix sort implementation
        /// _________________________
        ///
        /// The radix sort separates the number by its digit.
        ///
        /// The natural base is 10 (didatic).
        ///
        /// This algorithm uses the 'base_bits' to setup a binary base over the type 'uint64_t'.
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
        /// If the 'tmp_array' is nullptr, then the array is allocated and freed according the 'arrSize' parameter.
        ///
        /// Example:
        ///
        /// \code
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct RadixCountingSort<uint64_t>
        {
            using sortIndexType = SortIndex<uint64_t>;

            static ITK_INLINE void sort(uint64_t *_arr, uint32_t arrSize, uint64_t *tmp_array = nullptr)
            {

#if defined(ITK_AVX2)

                if (arrSize == 0)
                    return;

                // Counting Sort
                __m256i counting[256];
                uint64_t *aux;

                if (tmp_array == nullptr)
                    aux = (uint64_t *)ITKCommon::Memory::malloc(arrSize * sizeof(uint64_t));
                else
                    aux = tmp_array;

                uint64_t *in = _arr;
                uint64_t *out = aux;

                // for (int64_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(counting, 0, sizeof(__m256i) * 256);

                    // count the elements
                    for (uint32_t j = 0; j < arrSize; j++)
                    {
                        uint64_t currItem = in[j];

                        uint8_t index_0 = (currItem) & 0xff;
                        uint8_t index_1 = (currItem >> 8) & 0xff;
                        uint8_t index_2 = (currItem >> 16) & 0xff;
                        uint8_t index_3 = (currItem >> 24) & 0xff;

                        uint8_t index_4 = (currItem >> 32) & 0xff;
                        uint8_t index_5 = (currItem >> 40) & 0xff;
                        uint8_t index_6 = (currItem >> 48) & 0xff;
                        uint8_t index_7 = (currItem >> 56) & 0xff;

                        _mm256_u32_(counting[index_0], 0)++;
                        _mm256_u32_(counting[index_1], 1)++;
                        _mm256_u32_(counting[index_2], 2)++;
                        _mm256_u32_(counting[index_3], 3)++;
                        _mm256_u32_(counting[index_4], 4)++;
                        _mm256_u32_(counting[index_5], 5)++;
                        _mm256_u32_(counting[index_6], 6)++;
                        _mm256_u32_(counting[index_7], 7)++;
                    }

                    // compute offsets
                    __m256i acc = _mm256_set1_epi32(0);

                    for (uint32_t j = 0; j < 256; j++)
                    {
                        __m256i tmp = counting[j];
                        counting[j] = acc;
                        acc = _mm256_add_epi32(acc, tmp);
                    }

                    const __m256i increment_c[] = {
                        _mm256_setr_epi32(1, 0, 0, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 1, 0, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 1, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 1, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 1, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 1, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 1, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 0, 1)};
                    for (int idx = 0, shift = 0; idx < 8; idx += 1, shift += 8)
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            uint64_t currItem = in[j];
                            uint8_t bucket_index = ((currItem >> shift) & 0xff);
                            uint32_t out_index = _mm256_u32_(counting[bucket_index], idx);
                            counting[bucket_index] = _mm256_add_epi32(counting[bucket_index], increment_c[idx]);
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        uint64_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == nullptr)
                    ITKCommon::Memory::free(aux);

#else

                if (arrSize == 0)
                    return;

                // Counting Sort
                uint32_t counting[256][8];
                uint64_t *aux;

                if (tmp_array == nullptr)
                    aux = (uint64_t *)ITKCommon::Memory::malloc(arrSize * sizeof(uint64_t));
                else
                    aux = tmp_array;

                uint64_t *in = _arr;
                uint64_t *out = aux;

                // for (int64_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(&counting[0][0], 0, sizeof(uint32_t) * 256 * 8);

                    // count the elements
                    for (uint32_t j = 0; j < arrSize; j++)
                    {
                        uint64_t currItem = in[j];
                        counting[(currItem) & 0xff][0]++;
                        counting[(currItem >> 8) & 0xff][1]++;
                        counting[(currItem >> 16) & 0xff][2]++;
                        counting[(currItem >> 24) & 0xff][3]++;
                        counting[(currItem >> 32) & 0xff][4]++;
                        counting[(currItem >> 40) & 0xff][5]++;
                        counting[(currItem >> 48) & 0xff][6]++;
                        counting[(currItem >> 56) & 0xff][7]++;
                    }

                    // compute offsets
                    uint32_t acc[8] = {0, 0, 0, 0, 0, 0, 0, 0};

                    for (uint32_t j = 0; j < 256; j++)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            uint32_t tmp = counting[j][k];
                            counting[j][k] = acc[k];
                            acc[k] += tmp;
                        }
                    }

                    for (int idx = 0, shift = 0; idx < 8; idx += 1, shift += 8)
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            uint64_t currItem = in[j];
                            uint8_t bucket_index = ((currItem >> shift) & 0xff);
                            uint32_t out_index = counting[bucket_index][idx];
                            counting[bucket_index][idx]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        uint64_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == nullptr)
                    ITKCommon::Memory::free(aux);

#endif
            }

            static ITK_INLINE void sortIndex(sortIndexType *_arr, uint32_t arrSize, sortIndexType *tmp_array = nullptr)
            {
#if defined(ITK_AVX2)

                if (arrSize == 0)
                    return;

                // Counting Sort
                __m256i counting[256];
                sortIndexType *aux;

                if (tmp_array == nullptr)
                    aux = (sortIndexType *)ITKCommon::Memory::malloc(arrSize * sizeof(sortIndexType));
                else
                    aux = tmp_array;

                sortIndexType *in = _arr;
                sortIndexType *out = aux;

                // for (int64_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(counting, 0, sizeof(__m256i) * 256);

                    // count the elements
                    for (uint32_t j = 0; j < arrSize; j++)
                    {
                        uint64_t currItem = in[j].toSort;

                        uint64_t index_0 = (currItem) & 0xff;
                        uint64_t index_1 = (currItem >> 8) & 0xff;
                        uint64_t index_2 = (currItem >> 16) & 0xff;
                        uint64_t index_3 = (currItem >> 24) & 0xff;

                        uint8_t index_4 = (currItem >> 32) & 0xff;
                        uint8_t index_5 = (currItem >> 40) & 0xff;
                        uint8_t index_6 = (currItem >> 48) & 0xff;
                        uint8_t index_7 = (currItem >> 56) & 0xff;

                        _mm256_u32_(counting[index_0], 0)++;
                        _mm256_u32_(counting[index_1], 1)++;
                        _mm256_u32_(counting[index_2], 2)++;
                        _mm256_u32_(counting[index_3], 3)++;
                        _mm256_u32_(counting[index_4], 4)++;
                        _mm256_u32_(counting[index_5], 5)++;
                        _mm256_u32_(counting[index_6], 6)++;
                        _mm256_u32_(counting[index_7], 7)++;
                    }

                    // compute offsets
                    __m256i acc = _mm256_set1_epi32(0);

                    for (uint32_t j = 0; j < 256; j++)
                    {
                        __m256i tmp = counting[j];
                        counting[j] = acc;
                        acc = _mm256_add_epi32(acc, tmp);
                    }

                    const __m256i increment_c[] = {
                        _mm256_setr_epi32(1, 0, 0, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 1, 0, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 1, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 1, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 1, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 1, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 1, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 0, 1)};
                    for (int idx = 0, shift = 0; idx < 8; idx += 1, shift += 8)
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint8_t bucket_index = ((currItem.toSort >> shift) & 0xff);
                            uint32_t out_index = _mm256_u32_(counting[bucket_index], idx);
                            counting[bucket_index] = _mm256_add_epi64(counting[bucket_index], increment_c[idx]);
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == nullptr)
                    ITKCommon::Memory::free(aux);

#else

                if (arrSize == 0)
                    return;

                // Counting Sort
                uint32_t counting[256][8];
                sortIndexType *aux;

                if (tmp_array == nullptr)
                    aux = (sortIndexType *)ITKCommon::Memory::malloc(arrSize * sizeof(sortIndexType));
                else
                    aux = tmp_array;

                sortIndexType *in = _arr;
                sortIndexType *out = aux;

                // for (int64_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(&counting[0][0], 0, sizeof(uint32_t) * 256 * 8);

                    // count the elements
                    for (uint32_t j = 0; j < arrSize; j++)
                    {
                        uint64_t currItem = in[j].toSort;
                        counting[(currItem) & 0xff][0]++;
                        counting[(currItem >> 8) & 0xff][1]++;
                        counting[(currItem >> 16) & 0xff][2]++;
                        counting[(currItem >> 24) & 0xff][3]++;
                        counting[(currItem >> 32) & 0xff][4]++;
                        counting[(currItem >> 40) & 0xff][5]++;
                        counting[(currItem >> 48) & 0xff][6]++;
                        counting[(currItem >> 56) & 0xff][7]++;
                    }

                    // compute offsets
                    uint64_t acc[8] = {0, 0, 0, 0, 0, 0, 0, 0};

                    for (uint32_t j = 0; j < 256; j++)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            uint32_t tmp = counting[j][k];
                            counting[j][k] = acc[k];
                            acc[k] += tmp;
                        }
                    }

                    for (int idx = 0, shift = 0; idx < 8; idx += 1, shift += 8)
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint8_t bucket_index = ((currItem.toSort >> shift) & 0xff);
                            uint32_t out_index = counting[bucket_index][idx];
                            counting[bucket_index][idx]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == nullptr)
                    ITKCommon::Memory::free(aux);

#endif
            }
        };

        /// \brief Radix sort using Counting sort inside it for 'int64_t' type
        ///
        /// Radix sort implementation
        /// _________________________
        ///
        /// The radix sort separates the number by its digit.
        ///
        /// The natural base is 10 (didatic).
        ///
        /// This algorithm uses the 'base_bits' to setup a binary base over the type 'int64_t'.
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
        /// If the 'tmp_array' is nullptr, then the array is allocated and freed according the 'arrSize' parameter.
        ///
        /// Example:
        ///
        /// \code
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct RadixCountingSort<int64_t>
        {
            using sortIndexType = SortIndex<int64_t>;

            static ITK_INLINE void sort(int64_t *_arr, uint32_t arrSize, int64_t *tmp_array = nullptr)
            {
#if defined(ITK_AVX2)

                if (arrSize == 0)
                    return;

                // Counting Sort
                __m256i counting[256];
                int64_t *aux;

                if (tmp_array == nullptr)
                    aux = (int64_t *)ITKCommon::Memory::malloc(arrSize * sizeof(int64_t));
                else
                    aux = tmp_array;

                int64_t *in = _arr;
                int64_t *out = aux;

                // for (int64_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(counting, 0, sizeof(__m256i) * 256);

                    // count the elements
                    const uint64_t _minus_128 = (uint64_t)(uint8_t)-128;
                    for (uint32_t j = 0; j < arrSize; j++)
                    {

                        uint64_t currItem = (uint64_t)in[j];
                        uint8_t index_0 = (currItem) & 0xff;
                        uint8_t index_1 = (currItem >> 8) & 0xff;
                        uint8_t index_2 = (currItem >> 16) & 0xff;
                        uint8_t index_3 = (currItem >> 24) & 0xff;

                        uint8_t index_4 = (currItem >> 32) & 0xff;
                        uint8_t index_5 = (currItem >> 40) & 0xff;
                        uint8_t index_6 = (currItem >> 48) & 0xff;
                        uint8_t index_7 = ((currItem >> 56) ^ _minus_128) & 0xff;

                        _mm256_u32_(counting[index_0], 0)++;
                        _mm256_u32_(counting[index_1], 1)++;
                        _mm256_u32_(counting[index_2], 2)++;
                        _mm256_u32_(counting[index_3], 3)++;
                        _mm256_u32_(counting[index_4], 4)++;
                        _mm256_u32_(counting[index_5], 5)++;
                        _mm256_u32_(counting[index_6], 6)++;
                        _mm256_u32_(counting[index_7], 7)++;
                    }

                    // compute offsets
                    __m256i acc = _mm256_set1_epi32(0);

                    for (uint32_t j = 0; j < 256; j++)
                    {
                        __m256i tmp = counting[j];
                        counting[j] = acc;
                        acc = _mm256_add_epi32(acc, tmp);
                    }

                    const __m256i increment_c[] = {
                        _mm256_setr_epi32(1, 0, 0, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 1, 0, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 1, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 1, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 1, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 1, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 1, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 0, 1)};
                    for (int idx = 0, shift = 0; idx < 7; idx += 1, shift += 8)
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            int64_t currItem = in[j];
                            uint8_t bucket_index = (((uint64_t)currItem >> shift) & 0xff);
                            uint32_t out_index = _mm256_u32_(counting[bucket_index], idx);
                            counting[bucket_index] = _mm256_add_epi32(counting[bucket_index], increment_c[idx]);
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int64_t *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    const int idx = 7;
                    const int shift = 56;
                    {
                        // place elements in the output array
                        for (uint64_t j = 0; j < arrSize; j++)
                        {
                            int64_t currItem = in[j];
                            uint8_t bucket_index = ((((int64_t)currItem >> shift) ^ _minus_128) & 0xff);
                            uint32_t out_index = _mm256_u32_(counting[bucket_index], idx);
                            counting[bucket_index] = _mm256_add_epi32(counting[bucket_index], increment_c[idx]);
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int64_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == nullptr)
                    ITKCommon::Memory::free(aux);

#else

                if (arrSize == 0)
                    return;

                // Counting Sort
                uint32_t counting[256][8];
                int64_t *aux;

                if (tmp_array == nullptr)
                    aux = (int64_t *)ITKCommon::Memory::malloc(arrSize * sizeof(int64_t));
                else
                    aux = tmp_array;

                int64_t *in = _arr;
                int64_t *out = aux;

                // for (int64_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(&counting[0][0], 0, sizeof(uint32_t) * 256 * 8);

                    // count the elements
                    const uint64_t _minus_128 = (uint64_t)(uint8_t)-128;
                    for (uint32_t j = 0; j < arrSize; j++)
                    {
                        uint64_t currItem = (uint64_t)in[j];
                        counting[(currItem) & 0xff][0]++;
                        counting[(currItem >> 8) & 0xff][1]++;
                        counting[(currItem >> 16) & 0xff][2]++;
                        counting[(currItem >> 24) & 0xff][3]++;
                        counting[(currItem >> 32) & 0xff][4]++;
                        counting[(currItem >> 40) & 0xff][5]++;
                        counting[(currItem >> 48) & 0xff][6]++;
                        counting[((currItem >> 56) ^ _minus_128) & 0xff][7]++;
                    }

                    // compute offsets
                    uint32_t acc[8] = {0, 0, 0, 0, 0, 0, 0, 0};

                    for (uint32_t j = 0; j < 256; j++)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            uint32_t tmp = counting[j][k];
                            counting[j][k] = acc[k];
                            acc[k] += tmp;
                        }
                    }

                    for (int idx = 0, shift = 0; idx < 7; idx += 1, shift += 8)
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            int64_t currItem = in[j];
                            uint8_t bucket_index = (((uint64_t)currItem >> shift) & 0xff);
                            uint32_t out_index = counting[bucket_index][idx];
                            counting[bucket_index][idx]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int64_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                    
                    const int idx = 7;
                    const int shift = 56;
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            int64_t currItem = in[j];
                            uint8_t bucket_index = ((((uint64_t)currItem >> shift) ^ _minus_128) & 0xff);
                            uint32_t out_index = counting[bucket_index][idx];
                            counting[bucket_index][idx]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        int64_t *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == nullptr)
                    ITKCommon::Memory::free(aux);

#endif
            }

            static ITK_INLINE void sortIndex(sortIndexType *_arr, uint32_t arrSize, sortIndexType *tmp_array = nullptr)
            {
#if defined(ITK_AVX2)

                if (arrSize == 0)
                    return;

                // Counting Sort
                __m256i counting[256];
                sortIndexType *aux;

                if (tmp_array == nullptr)
                    aux = (sortIndexType *)ITKCommon::Memory::malloc(arrSize * sizeof(sortIndexType));
                else
                    aux = tmp_array;

                sortIndexType *in = _arr;
                sortIndexType *out = aux;

                // for (int64_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(counting, 0, sizeof(__m256i) * 256);

                    // count the elements
                    const uint64_t _minus_128 = (uint64_t)(uint8_t)-128;
                    for (uint64_t j = 0; j < arrSize; j++)
                    {

                        uint64_t currItem = (uint64_t)in[j].toSort;
                        uint8_t index_0 = (currItem) & 0xff;
                        uint8_t index_1 = (currItem >> 8) & 0xff;
                        uint8_t index_2 = (currItem >> 16) & 0xff;
                        uint8_t index_3 = (currItem >> 24) & 0xff;

                        uint8_t index_4 = (currItem >> 32) & 0xff;
                        uint8_t index_5 = (currItem >> 40) & 0xff;
                        uint8_t index_6 = (currItem >> 48) & 0xff;
                        uint8_t index_7 = ((currItem >> 56) ^ _minus_128) & 0xff;

                        _mm256_u32_(counting[index_0], 0)++;
                        _mm256_u32_(counting[index_1], 1)++;
                        _mm256_u32_(counting[index_2], 2)++;
                        _mm256_u32_(counting[index_3], 3)++;
                        _mm256_u32_(counting[index_4], 4)++;
                        _mm256_u32_(counting[index_5], 5)++;
                        _mm256_u32_(counting[index_6], 6)++;
                        _mm256_u32_(counting[index_7], 7)++;
                    }

                    // compute offsets
                    __m256i acc = _mm256_set1_epi32(0);

                    for (uint32_t j = 0; j < 256; j++)
                    {
                        __m256i tmp = counting[j];
                        counting[j] = acc;
                        acc = _mm256_add_epi64(acc, tmp);
                    }

                    const __m256i increment_c[] = {
                        _mm256_setr_epi32(1, 0, 0, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 1, 0, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 1, 0, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 1, 0, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 1, 0, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 1, 0, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 1, 0),
                        _mm256_setr_epi32(0, 0, 0, 0, 0, 0, 0, 1)};
                    for (int idx = 0, shift = 0; idx < 7; idx += 1, shift += 8)
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint8_t bucket_index = (((uint64_t)currItem.toSort >> shift) & 0xff);
                            uint64_t out_index = _mm256_u32_(counting[bucket_index], idx);
                            counting[bucket_index] = _mm256_add_epi32(counting[bucket_index], increment_c[idx]);
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    const int idx = 7;
                    const int shift = 56;
                    {
                        // place elements in the output array
                        for (uint32_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint8_t bucket_index = ((((int64_t)currItem.toSort >> shift) ^ _minus_128) & 0xff);
                            uint64_t out_index = _mm256_u32_(counting[bucket_index], idx);
                            counting[bucket_index] = _mm256_add_epi32(counting[bucket_index], increment_c[idx]);
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == nullptr)
                    ITKCommon::Memory::free(aux);

#else

                if (arrSize == 0)
                    return;

                // Counting Sort
                uint32_t counting[256][8];
                sortIndexType *aux;

                if (tmp_array == nullptr)
                    aux = (sortIndexType *)ITKCommon::Memory::malloc(arrSize * sizeof(sortIndexType));
                else
                    aux = tmp_array;

                sortIndexType *in = _arr;
                sortIndexType *out = aux;

                // for (int64_t i = 0; i < radix_num; i++)
                {

                    // Cleaning counters
                    memset(&counting[0][0], 0, sizeof(uint32_t) * 256 * 8);

                    // count the elements
                    const uint64_t _minus_128 = (uint64_t)(uint8_t)-128;
                    for (uint64_t j = 0; j < arrSize; j++)
                    {
                        uint64_t currItem = (uint64_t)in[j].toSort;
                        counting[(currItem) & 0xff][0]++;
                        counting[(currItem >> 8) & 0xff][1]++;
                        counting[(currItem >> 16) & 0xff][2]++;
                        counting[(currItem >> 24) & 0xff][3]++;
                        counting[(currItem >> 32) & 0xff][4]++;
                        counting[(currItem >> 40) & 0xff][5]++;
                        counting[(currItem >> 48) & 0xff][6]++;
                        counting[((currItem >> 56) ^ _minus_128) & 0xff][7]++;
                    }

                    // compute offsets
                    uint32_t acc[8] = {0, 0, 0, 0, 0, 0, 0, 0};

                    for (uint32_t j = 0; j < 256; j++)
                    {
                        for (int k = 0; k < 8; k++)
                        {
                            uint32_t tmp = counting[j][k];
                            counting[j][k] = acc[k];
                            acc[k] += tmp;
                        }
                    }

                    for (int idx = 0, shift = 0; idx < 7; idx += 1, shift += 8)
                    {
                        // place elements in the output array
                        for (uint64_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint8_t bucket_index = (((uint64_t)currItem.toSort >> shift) & 0xff);
                            uint32_t out_index = counting[bucket_index][idx];
                            counting[bucket_index][idx]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }

                    const int idx = 7;
                    const int shift = 56;
                    {
                        // place elements in the output array
                        for (uint64_t j = 0; j < arrSize; j++)
                        {
                            const sortIndexType &currItem = in[j];
                            uint8_t bucket_index = ((((uint64_t)currItem.toSort >> shift) ^ _minus_128) & 0xff);
                            uint32_t out_index = counting[bucket_index][idx];
                            counting[bucket_index][idx]++;
                            out[out_index] = currItem;
                        }

                        // swap out, in
                        sortIndexType *tmp = in;
                        in = out;
                        out = tmp;
                    }
                }

                if (tmp_array == nullptr)
                    ITKCommon::Memory::free(aux);

#endif
            }
        };

    }
}