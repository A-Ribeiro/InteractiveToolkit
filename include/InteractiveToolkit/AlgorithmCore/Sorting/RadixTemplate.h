#pragma once

#include "../../common.h"

namespace AlgorithmCore
{
    namespace Sorting
    {

        template <typename T>
        struct IntGuessing
        {
        };

        template <>
        struct IntGuessing<int8_t>
        {
            typedef uint8_t sort_type;
            static constexpr int bytes = 1;
            static inline uint8_t get_sort_uint_value(const int8_t &v) noexcept { return (uint8_t)v ^ UINT8_C(0x80); }
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept { memcpy(dst, src, size); }
        };

        template <>
        struct IntGuessing<uint8_t>
        {
            typedef uint8_t sort_type;
            static constexpr int bytes = 1;
            static inline uint8_t get_sort_uint_value(const uint8_t &v) noexcept { return v; }
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept { memcpy(dst, src, size); }
        };

        template <>
        struct IntGuessing<int16_t>
        {
            typedef uint16_t sort_type;
            static constexpr int bytes = 2;
            static inline uint16_t get_sort_uint_value(const int16_t &v) noexcept { return (uint16_t)v ^ UINT16_C(0x8000); }
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        template <>
        struct IntGuessing<uint16_t>
        {
            typedef uint16_t sort_type;
            static constexpr int bytes = 2;
            static inline uint16_t get_sort_uint_value(const uint16_t &v) noexcept { return v; }
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        template <>
        struct IntGuessing<int32_t>
        {
            typedef uint32_t sort_type;
            static constexpr int bytes = 4;
            static inline uint32_t get_sort_uint_value(const int32_t &v) noexcept { return (uint32_t)v ^ UINT32_C(0x80000000); }
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        template <>
        struct IntGuessing<uint32_t>
        {
            typedef uint32_t sort_type;
            static constexpr int bytes = 4;
            static inline uint32_t get_sort_uint_value(const uint32_t &v) noexcept { return v; }
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        template <>
        struct IntGuessing<float>
        {
            typedef uint32_t sort_type;
            static constexpr int bytes = 4;
            static inline uint32_t get_sort_uint_value(const float &v) noexcept
            {
                const uint32_t &data_value = *static_cast<uint32_t *>((void *)&v);
                uint32_t mask = (-int32_t(data_value >> 31)) | UINT32_C(0x80000000);
                return data_value ^ mask;
            }
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        template <>
        struct IntGuessing<int64_t>
        {
            typedef uint64_t sort_type;
            static constexpr int bytes = 8;
            static inline uint64_t get_sort_uint_value(const int64_t &v) noexcept { return (uint64_t)v ^ UINT64_C(0x8000000000000000); }
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        template <>
        struct IntGuessing<uint64_t>
        {
            typedef uint64_t sort_type;
            static constexpr int bytes = 8;
            static inline uint64_t get_sort_uint_value(const uint64_t &v) noexcept { return v; }
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        template <>
        struct IntGuessing<double>
        {
            typedef uint64_t sort_type;
            static constexpr int bytes = 8;
            static inline uint64_t get_sort_uint_value(const double &v) noexcept
            {
                const uint64_t &data_value = *static_cast<uint64_t *>((void *)&v);
                uint64_t mask = (-int64_t(data_value >> 63)) | UINT64_C(0x8000000000000000);
                return data_value ^ mask;
            }
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        template <typename T, typename = void>
        struct ToSortExtractor : std::false_type
        {
            typedef T sort_input_type;
            static const sort_input_type &read_data(const sort_input_type &v) noexcept { return v; }
        };

        template <typename T>
        struct ToSortExtractor<T, decltype(&T::toSort, void())> : std::true_type
        {
            typedef decltype(T::toSort) sort_input_type;
            static const sort_input_type &read_data(const T &v) noexcept { return v.toSort; }
        };

        template <typename T>
        static inline void radix_sort(T *_arr, uint32_t arrSize, T *tmp_array = nullptr)
        {
            using to_sort_extractor = ToSortExtractor<T>;
            using int_guessing = IntGuessing<typename to_sort_extractor::sort_input_type>;
            using sort_type = typename int_guessing::sort_type;

            if (arrSize == 0)
                return;

            // Counting Sort
            uint32_t counting[256][int_guessing::bytes] = {}; // initialize cleaned

            std::unique_ptr<T[]> aux;
            if (!tmp_array)
                aux = STL_Tools::make_unique<T[]>(arrSize);
            T *in = _arr;
            T *out = (aux) ? aux.get() : tmp_array;

            // count the elements
            for (uint32_t j = 0; j < arrSize; j++)
            {
                sort_type sort_index = int_guessing::get_sort_uint_value(to_sort_extractor::read_data(in[j]));
                for (uint32_t k = 0; k < int_guessing::bytes; k++)
                {
                    // uint32_t bucket_index = (uint32_t)sort_index & 0xff;
                    // 0 .. 255 valid range
                    counting[(uint8_t)sort_index][k]++;
                    sort_index = sort_index >> 8;
                }
            }

            // compute offsets
            uint32_t acc[int_guessing::bytes] = {}; // initialize cleaned
            uint32_t tmp[int_guessing::bytes];
            for (uint32_t j = 0; j < 256; j++)
            {
                for (uint32_t k = 0; k < int_guessing::bytes; k++)
                {
                    tmp[k] = counting[j][k];
                    counting[j][k] = acc[k];
                    acc[k] += tmp[k];
                }
            }

            for (uint32_t i = 0; i < int_guessing::bytes; i++)
            {
                uint32_t shift = i << 3; // i * 8
                // place elements in the output array
                for (uint32_t j = 0; j < arrSize; j++)
                {
                    const T &currItem = in[j];
                    sort_type sort_index = int_guessing::get_sort_uint_value(to_sort_extractor::read_data(currItem));
                    sort_index = sort_index >> shift;
                    // uint32_t bucket_index = (uint32_t)sort_index & 0xff;
                    // 0 .. 255 valid range
                    uint8_t bucket_index = (uint8_t)sort_index;
                    uint32_t out_index = counting[bucket_index][i];
                    counting[bucket_index][i]++;
                    out[out_index] = currItem;
                }

                // swap out, in
                std::swap(in, out);
            }

            int_guessing::final_memcpy(_arr, in, sizeof(T) * arrSize);
        }
        
    }
}
