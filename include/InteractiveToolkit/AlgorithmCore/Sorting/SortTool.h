#pragma once

#include "../../common.h"
#include "SortIndex.h"

namespace AlgorithmCore
{
    namespace Sorting
    {

        template <typename _type>
        struct SortTool
        {
        };

        template <>
        struct SortTool<uint32_t>
        {
            using sortIndexType = SortIndex<uint32_t>;

            static ITK_INLINE uint32_t spread(uint32_t min, uint32_t max, uint32_t v)
            {
                v = v - min;
                uint64_t aux = v;
                uint64_t delta = max - min;
                aux = (aux * (uint64_t)UINT32_MAX) / delta;
                return (uint32_t)aux;
            }

            // flip a float for sorting
            //  finds SIGN of fp number.
            //  if it's 1 (negative float), it flips all bits
            //  if it's 0 (positive float), it flips the sign only
            static ITK_INLINE uint32_t floatToInt(const float &_f)
            {
                uint32_t f = *(uint32_t *)&_f;
                uint32_t mask = (-int32_t(f >> 31)) | 0x80000000;
                return f ^ mask;
            }

            // flip a float back (invert FloatFlip)
            //  signed was flipped from above, so:
            //  if sign is 1 (negative), it flips the sign bit back
            //  if sign is 0 (positive), it flips all bits back
            static ITK_INLINE float intToFloat(const uint32_t &f)
            {
                uint32_t mask = ((f >> 31) - 1) | 0x80000000;
                mask = f ^ mask;
                return *(float *)&mask;
            }

            /// \brief Inplace replace
            ///
            /// Do the output buffer sort internal replace.
            /// No need to create a double buffer approach.
            ///
            /// Example:
            ///
            /// \code
            /// std::vector<int32_t> original_buffer();
            ///
            /// std::vector<Sorting::IndexInt32> sort_index_buffer(original_buffer.size());
            /// for (int i = 0; i < (int)sort_index_buffer.size(); i++)
            /// {
            ///     sort_index_buffer[i].index = i;
            ///     sort_index_buffer[i].toSort = original_buffer[i];
            /// }
            /// Sorting::radix_counting_sort_signed_index(&sort_index_buffer[0], sort_index_buffer.size());
            ///
            /// // Replace the original buffer with the sorted version
            /// Sorting::inplace_replace(&sort_index_buffer[0], &original_buffer[0], original_buffer.size());
            ///
            /// \endcode
            ///
            /// \author Alessandro Ribeiro
            ///
            template <typename U>
            static ITK_INLINE void inplaceReplace(sortIndexType *index_buffer, U *output, uint32_t size)
            {
                std::vector<uint32_t> inplace_redirect(size);
                for (uint32_t i = 0; i < size; i++)
                {
                    // make a swap

                    uint32_t src_index = index_buffer[i].index;
                    uint32_t target_index = i;

                    if (src_index == target_index)
                        continue;

                    // redirect walk
                    while (src_index < target_index)
                        src_index = inplace_redirect[src_index];

                    // swap
                    U save = output[target_index];
                    output[target_index] = output[src_index];
                    output[src_index] = save;

                    // save redirection
                    inplace_redirect[target_index] = src_index;
                }
            }
        };

        template <>
        struct SortTool<int32_t>
        {
            using sortIndexType = SortIndex<int32_t>;

            static ITK_INLINE int32_t spread(int32_t min, int32_t max, int32_t v)
            {
                uint64_t aux = (uint64_t)v - (uint64_t)min;
                uint64_t delta = (uint64_t)max - (uint64_t)min;
                aux = (aux * (uint64_t)UINT32_MAX) / delta;
                aux += (uint64_t)INT32_MIN;
                return (int32_t)aux;
            }

            // flip a float for sorting
            //  finds SIGN of fp number.
            //  if it's 1 (negative float), it flips all bits
            //  if it's 0 (positive float), it flips the sign only
            static ITK_INLINE int32_t floatToInt(const float &_f)
            {
                int32_t f = (int32_t)SortTool<uint32_t>::floatToInt(_f) + INT32_MIN;
                return (int32_t)f;
            }

            // flip a float back (invert FloatFlip)
            //  signed was flipped from above, so:
            //  if sign is 1 (negative), it flips the sign bit back
            //  if sign is 0 (positive), it flips all bits back
            static ITK_INLINE float intToFloat(const int32_t &_f)
            {
                uint32_t f = (uint32_t)((uint32_t)_f - (uint32_t)INT32_MIN);
                return SortTool<uint32_t>::intToFloat(f);
            }

            /// \brief Inplace replace
            ///
            /// Do the output buffer sort internal replace.
            /// No need to create a double buffer approach.
            ///
            /// Example:
            ///
            /// \code
            /// std::vector<int32_t> original_buffer();
            ///
            /// std::vector<Sorting::IndexInt32> sort_index_buffer(original_buffer.size());
            /// for (int i = 0; i < (int)sort_index_buffer.size(); i++)
            /// {
            ///     sort_index_buffer[i].index = i;
            ///     sort_index_buffer[i].toSort = original_buffer[i];
            /// }
            /// Sorting::radix_counting_sort_signed_index(&sort_index_buffer[0], sort_index_buffer.size());
            ///
            /// // Replace the original buffer with the sorted version
            /// Sorting::inplace_replace(&sort_index_buffer[0], &original_buffer[0], original_buffer.size());
            ///
            /// \endcode
            ///
            /// \author Alessandro Ribeiro
            ///
            template <typename U>
            static ITK_INLINE void inplaceReplace(sortIndexType *index_buffer, U *output, uint32_t size)
            {
                std::vector<uint32_t> inplace_redirect(size);
                for (uint32_t i = 0; i < size; i++)
                {
                    // make a swap

                    uint32_t src_index = index_buffer[i].index;
                    uint32_t target_index = i;

                    if (src_index == target_index)
                        continue;

                    // redirect walk
                    while (src_index < target_index){
                        //printf("%u %u\n",src_index, target_index);
                        src_index = inplace_redirect[src_index];
                    }

                    // swap
                    U save = output[target_index];
                    output[target_index] = output[src_index];
                    output[src_index] = save;

                    // save redirection
                    inplace_redirect[target_index] = src_index;
                }
            }
        };

        using SortToolu = SortTool<uint32_t>;
        using SortTooli = SortTool<int32_t>;

    }
}