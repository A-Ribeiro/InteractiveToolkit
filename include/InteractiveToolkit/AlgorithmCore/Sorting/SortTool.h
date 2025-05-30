#pragma once

#include "../../common.h"
#include "SortIndex.h"
#include "../../MathCore/uint128.h"

namespace AlgorithmCore
{
    namespace Sorting
    {

        template <typename _type>
        struct RadixCountingSort
        {
        };

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
                U save;
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
                    save = output[target_index];
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
                // uint64_t aux = (uint64_t)v - (uint64_t)min;
                // uint64_t delta = (uint64_t)max - (uint64_t)min;
                // aux = (aux * (uint64_t)UINT32_MAX) / delta;
                // aux += (uint64_t)INT32_MIN;
                // return (int32_t)aux;

                return SortTool<uint32_t>::spread(min ^ UINT32_C(0x80000000), max ^ UINT32_C(0x80000000), v ^ UINT32_C(0x80000000)) ^ UINT32_C(0x80000000);
            }

            // flip a float for sorting
            //  finds SIGN of fp number.
            //  if it's 1 (negative float), it flips all bits
            //  if it's 0 (positive float), it flips the sign only
            static ITK_INLINE int32_t floatToInt(const float &_f)
            {
                uint32_t f = SortTool<uint32_t>::floatToInt(_f) ^ UINT32_C(0x80000000);
                return (int32_t)f;
            }

            // flip a float back (invert FloatFlip)
            //  signed was flipped from above, so:
            //  if sign is 1 (negative), it flips the sign bit back
            //  if sign is 0 (positive), it flips all bits back
            static ITK_INLINE float intToFloat(const int32_t &_f)
            {
                uint32_t f = (uint32_t)_f ^ UINT32_C(0x80000000);
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
                U save;
                for (uint32_t i = 0; i < size; i++)
                {
                    // make a swap

                    uint32_t src_index = index_buffer[i].index;
                    uint32_t target_index = i;

                    if (src_index == target_index)
                        continue;

                    // redirect walk
                    while (src_index < target_index)
                    {
                        // printf("%u %u\n",src_index, target_index);
                        src_index = inplace_redirect[src_index];
                    }

                    // swap
                    save = output[target_index];
                    output[target_index] = output[src_index];
                    output[src_index] = save;

                    // save redirection
                    inplace_redirect[target_index] = src_index;
                }
            }
        };

        template <>
        struct SortTool<uint64_t>
        {
            using sortIndexType = SortIndex<uint64_t>;

            static ITK_INLINE uint64_t spread(uint64_t min, uint64_t max, uint64_t v)
            {
                v = v - min;
                uint64_t delta = max - min;
                uint64_t r_low, r_high;
                MathCore::multiply_uint64_to_uint128(v, UINT64_MAX, &r_high , &r_low);
                return MathCore::divide_uint128_by_uint64(r_high, r_low, delta);

                //MathCore::uint128 aux = v;
                //MathCore::uint128 delta = max - min;
                //const MathCore::uint128 _max_v = UINT64_MAX;
                //aux = (aux * _max_v) / delta;
                //return (uint64_t)aux;
            }

            // flip a float for sorting
            //  finds SIGN of fp number.
            //  if it's 1 (negative float), it flips all bits
            //  if it's 0 (positive float), it flips the sign only
            static ITK_INLINE uint64_t floatToInt(const float &_f)
            {
                // return (uint64_t)SortTool<uint32_t>::floatToInt(_f);
                return doubleToInt((double)_f);
            }

            static ITK_INLINE uint64_t doubleToInt(const double &_f)
            {
                uint64_t f = *(uint64_t *)&_f;
                uint64_t mask = (-int64_t(f >> 63)) | UINT64_C(0x8000000000000000);
                return f ^ mask;
            }

            // flip a float back (invert FloatFlip)
            //  signed was flipped from above, so:
            //  if sign is 1 (negative), it flips the sign bit back
            //  if sign is 0 (positive), it flips all bits back
            static ITK_INLINE float intToFloat(const uint64_t &_f)
            {
                // return SortTool<uint32_t>::intToFloat(_f);
                return (float)intToDouble(_f);
            }

            static ITK_INLINE double intToDouble(const uint64_t &f)
            {
                uint64_t mask = ((f >> 63) - 1) | UINT64_C(0x8000000000000000);
                mask = f ^ mask;
                return *(double *)&mask;
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
                U save;
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
                    save = output[target_index];
                    output[target_index] = output[src_index];
                    output[src_index] = save;

                    // save redirection
                    inplace_redirect[target_index] = src_index;
                }
            }
        };

        template <>
        struct SortTool<int64_t>
        {
            using sortIndexType = SortIndex<int64_t>;

            static ITK_INLINE int64_t spread(int64_t min, int64_t max, int64_t v)
            {
                return SortTool<uint64_t>::spread(min ^ UINT64_C(0x8000000000000000), max ^ UINT64_C(0x8000000000000000), v ^ UINT64_C(0x8000000000000000)) ^ UINT64_C(0x8000000000000000);
            }

            // flip a float for sorting
            //  finds SIGN of fp number.
            //  if it's 1 (negative float), it flips all bits
            //  if it's 0 (positive float), it flips the sign only
            static ITK_INLINE int64_t floatToInt(const float &_f)
            {
                return doubleToInt((double)_f);
            }

            static ITK_INLINE int64_t doubleToInt(const double &_f)
            {
                uint64_t f = SortTool<uint64_t>::doubleToInt(_f) ^ UINT64_C(0x8000000000000000);
                return (int64_t)f;
            }

            // flip a float back (invert FloatFlip)
            //  signed was flipped from above, so:
            //  if sign is 1 (negative), it flips the sign bit back
            //  if sign is 0 (positive), it flips all bits back
            static ITK_INLINE float intToFloat(const int64_t &_f)
            {
                return (float)intToDouble(_f);
            }
            static ITK_INLINE double intToDouble(const int64_t &_f)
            {
                uint64_t f = (uint64_t)_f ^ UINT64_C(0x8000000000000000);
                return SortTool<uint64_t>::intToDouble(f);
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
                U save;
                for (uint32_t i = 0; i < size; i++)
                {
                    // make a swap

                    uint32_t src_index = index_buffer[i].index;
                    uint32_t target_index = i;

                    if (src_index == target_index)
                        continue;

                    // redirect walk
                    while (src_index < target_index)
                    {
                        // printf("%u %u\n",src_index, target_index);
                        src_index = inplace_redirect[src_index];
                    }

                    // swap
                    save = output[target_index];
                    output[target_index] = output[src_index];
                    output[src_index] = save;

                    // save redirection
                    inplace_redirect[target_index] = src_index;
                }
            }
        };

        using SortToolu32 = SortTool<uint32_t>;
        using SortTooli32 = SortTool<int32_t>;

        using SortToolu64 = SortTool<uint64_t>;
        using SortTooli64 = SortTool<int64_t>;

    }
}