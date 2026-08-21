#pragma once

#include "../../common.h"

namespace AlgorithmCore
{
    namespace Sorting
    {

        /// \brief Primary template for integer type guessing in radix sort.
        ///
        /// Provides type traits used by the radix sort algorithm to determine
        /// the appropriate unsigned sort type, byte count, and value conversion
        /// for signed and floating-point types.
        ///
        /// \author Alessandro Ribeiro
        ///
        /// \tparam T The type to guess integer properties for.
        ///
        template <typename T>
        struct IntGuessing
        {
        };

        /// \brief Specialization of IntGuessing for int8_t.
        ///
        /// Maps int8_t to uint8_t for sorting, XORs with 0x80 to preserve
        /// signed ordering, and performs a memcpy for the final copy step.
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct IntGuessing<int8_t>
        {
            /// \brief The unsigned type used for sorting.
            ///
            typedef uint8_t sort_type;
            /// \brief Number of bytes in the type.
            ///
            static constexpr int bytes = 1;
            /// \brief Convert a signed int8_t value to its unsigned sort representation.
            ///
            /// XORs the value with 0x80 to preserve signed ordering when treated as unsigned.
            ///
            /// \param v The int8_t value to convert.
            /// \return The unsigned sort representation.
            ///
            static inline uint8_t get_sort_uint_value(const int8_t &v) noexcept { return (uint8_t)v ^ UINT8_C(0x80); }
            /// \brief Perform the final memory copy for the sorted result.
            ///
            /// \param dst Destination buffer.
            /// \param src Source buffer.
            /// \param size Number of bytes to copy.
            ///
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept { memcpy(dst, src, size); }
        };

        /// \brief Specialization of IntGuessing for uint8_t.
        ///
        /// Maps uint8_t to itself for sorting. No sign adjustment is needed
        /// since the type is already unsigned.
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct IntGuessing<uint8_t>
        {
            /// \brief The unsigned type used for sorting.
            ///
            typedef uint8_t sort_type;
            /// \brief Number of bytes in the type.
            ///
            static constexpr int bytes = 1;
            /// \brief Return the value as-is for sorting.
            ///
            /// \param v The uint8_t value.
            /// \return The same value.
            ///
            static inline uint8_t get_sort_uint_value(const uint8_t &v) noexcept { return v; }
            /// \brief Perform the final memory copy for the sorted result.
            ///
            /// \param dst Destination buffer.
            /// \param src Source buffer.
            /// \param size Number of bytes to copy.
            ///
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept { memcpy(dst, src, size); }
        };

        /// \brief Specialization of IntGuessing for int16_t.
        ///
        /// Maps int16_t to uint16_t for sorting, XORs with 0x8000 to preserve
        /// signed ordering, and performs no final copy (in-place swap is sufficient).
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct IntGuessing<int16_t>
        {
            /// \brief The unsigned type used for sorting.
            ///
            typedef uint16_t sort_type;
            /// \brief Number of bytes in the type.
            ///
            static constexpr int bytes = 2;
            /// \brief Convert a signed int16_t value to its unsigned sort representation.
            ///
            /// XORs the value with 0x8000 to preserve signed ordering when treated as unsigned.
            ///
            /// \param v The int16_t value to convert.
            /// \return The unsigned sort representation.
            ///
            static inline uint16_t get_sort_uint_value(const int16_t &v) noexcept { return (uint16_t)v ^ UINT16_C(0x8000); }
            /// \brief No final copy needed for int16_t.
            ///
            /// \param dst Destination buffer.
            /// \param src Source buffer.
            /// \param size Number of bytes to copy.
            ///
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        /// \brief Specialization of IntGuessing for uint16_t.
        ///
        /// Maps uint16_t to itself for sorting. No sign adjustment is needed
        /// since the type is already unsigned.
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct IntGuessing<uint16_t>
        {
            /// \brief The unsigned type used for sorting.
            ///
            typedef uint16_t sort_type;
            /// \brief Number of bytes in the type.
            ///
            static constexpr int bytes = 2;
            /// \brief Return the value as-is for sorting.
            ///
            /// \param v The uint16_t value.
            /// \return The same value.
            ///
            static inline uint16_t get_sort_uint_value(const uint16_t &v) noexcept { return v; }
            /// \brief No final copy needed for uint16_t.
            ///
            /// \param dst Destination buffer.
            /// \param src Source buffer.
            /// \param size Number of bytes to copy.
            ///
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        /// \brief Specialization of IntGuessing for int32_t.
        ///
        /// Maps int32_t to uint32_t for sorting, XORs with 0x80000000 to preserve
        /// signed ordering, and performs no final copy (in-place swap is sufficient).
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct IntGuessing<int32_t>
        {
            /// \brief The unsigned type used for sorting.
            ///
            typedef uint32_t sort_type;
            /// \brief Number of bytes in the type.
            ///
            static constexpr int bytes = 4;
            /// \brief Convert a signed int32_t value to its unsigned sort representation.
            ///
            /// XORs the value with 0x80000000 to preserve signed ordering when treated as unsigned.
            ///
            /// \param v The int32_t value to convert.
            /// \return The unsigned sort representation.
            ///
            static inline uint32_t get_sort_uint_value(const int32_t &v) noexcept { return (uint32_t)v ^ UINT32_C(0x80000000); }
            /// \brief No final copy needed for int32_t.
            ///
            /// \param dst Destination buffer.
            /// \param src Source buffer.
            /// \param size Number of bytes to copy.
            ///
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        /// \brief Specialization of IntGuessing for uint32_t.
        ///
        /// Maps uint32_t to itself for sorting. No sign adjustment is needed
        /// since the type is already unsigned.
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct IntGuessing<uint32_t>
        {
            /// \brief The unsigned type used for sorting.
            ///
            typedef uint32_t sort_type;
            /// \brief Number of bytes in the type.
            ///
            static constexpr int bytes = 4;
            /// \brief Return the value as-is for sorting.
            ///
            /// \param v The uint32_t value.
            /// \return The same value.
            ///
            static inline uint32_t get_sort_uint_value(const uint32_t &v) noexcept { return v; }
            /// \brief No final copy needed for uint32_t.
            ///
            /// \param dst Destination buffer.
            /// \param src Source buffer.
            /// \param size Number of bytes to copy.
            ///
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        /// \brief Specialization of IntGuessing for float.
        ///
        /// Maps float to uint32_t for sorting. Uses IEEE 754 bit manipulation
        /// to preserve signed ordering: negative floats are inverted so that
        /// lexicographic unsigned comparison yields correct signed ordering.
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct IntGuessing<float>
        {
            /// \brief The unsigned type used for sorting.
            ///
            typedef uint32_t sort_type;
            /// \brief Number of bytes in the type.
            ///
            static constexpr int bytes = 4;
            /// \brief Convert a float value to its unsigned sort representation.
            ///
            /// Reinterprets the float bits as uint32_t and applies a sign-aware
            /// mask so that negative values are inverted, preserving correct ordering.
            ///
            /// \param v The float value to convert.
            /// \return The unsigned sort representation.
            ///
            static inline uint32_t get_sort_uint_value(const float &v) noexcept
            {
                const uint32_t &data_value = *static_cast<uint32_t *>((void *)&v);
                uint32_t mask = (-int32_t(data_value >> 31)) | UINT32_C(0x80000000);
                return data_value ^ mask;
            }
            /// \brief No final copy needed for float.
            ///
            /// \param dst Destination buffer.
            /// \param src Source buffer.
            /// \param size Number of bytes to copy.
            ///
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        /// \brief Specialization of IntGuessing for int64_t.
        ///
        /// Maps int64_t to uint64_t for sorting, XORs with 0x8000000000000000
        /// to preserve signed ordering, and performs no final copy.
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct IntGuessing<int64_t>
        {
            /// \brief The unsigned type used for sorting.
            ///
            typedef uint64_t sort_type;
            /// \brief Number of bytes in the type.
            ///
            static constexpr int bytes = 8;
            /// \brief Convert a signed int64_t value to its unsigned sort representation.
            ///
            /// XORs the value with 0x8000000000000000 to preserve signed ordering when treated as unsigned.
            ///
            /// \param v The int64_t value to convert.
            /// \return The unsigned sort representation.
            ///
            static inline uint64_t get_sort_uint_value(const int64_t &v) noexcept { return (uint64_t)v ^ UINT64_C(0x8000000000000000); }
            /// \brief No final copy needed for int64_t.
            ///
            /// \param dst Destination buffer.
            /// \param src Source buffer.
            /// \param size Number of bytes to copy.
            ///
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        /// \brief Specialization of IntGuessing for uint64_t.
        ///
        /// Maps uint64_t to itself for sorting. No sign adjustment is needed
        /// since the type is already unsigned.
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct IntGuessing<uint64_t>
        {
            /// \brief The unsigned type used for sorting.
            ///
            typedef uint64_t sort_type;
            /// \brief Number of bytes in the type.
            ///
            static constexpr int bytes = 8;
            /// \brief Return the value as-is for sorting.
            ///
            /// \param v The uint64_t value.
            /// \return The same value.
            ///
            static inline uint64_t get_sort_uint_value(const uint64_t &v) noexcept { return v; }
            /// \brief No final copy needed for uint64_t.
            ///
            /// \param dst Destination buffer.
            /// \param src Source buffer.
            /// \param size Number of bytes to copy.
            ///
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        /// \brief Specialization of IntGuessing for double.
        ///
        /// Maps double to uint64_t for sorting. Uses IEEE 754 bit manipulation
        /// to preserve signed ordering: negative doubles are inverted so that
        /// lexicographic unsigned comparison yields correct signed ordering.
        ///
        /// \author Alessandro Ribeiro
        ///
        template <>
        struct IntGuessing<double>
        {
            /// \brief The unsigned type used for sorting.
            ///
            typedef uint64_t sort_type;
            /// \brief Number of bytes in the type.
            ///
            static constexpr int bytes = 8;
            /// \brief Convert a double value to its unsigned sort representation.
            ///
            /// Reinterprets the double bits as uint64_t and applies a sign-aware
            /// mask so that negative values are inverted, preserving correct ordering.
            ///
            /// \param v The double value to convert.
            /// \return The unsigned sort representation.
            ///
            static inline uint64_t get_sort_uint_value(const double &v) noexcept
            {
                const uint64_t &data_value = *static_cast<uint64_t *>((void *)&v);
                uint64_t mask = (-int64_t(data_value >> 63)) | UINT64_C(0x8000000000000000);
                return data_value ^ mask;
            }
            /// \brief No final copy needed for double.
            ///
            /// \param dst Destination buffer.
            /// \param src Source buffer.
            /// \param size Number of bytes to copy.
            ///
            static inline void final_memcpy(void *dst, void *src, size_t size) noexcept {}
        };

        /// \brief Primary template for extracting the sort key from a type.
        ///
        /// By default, assumes the type itself is the sort key (std::false_type).
        /// Used to determine whether a type has a dedicated `toSort` member.
        ///
        /// \author Alessandro Ribeiro
        ///
        /// \tparam T The type to extract sort data from.
        ///
        template <typename T, typename = void>
        struct ToSortExtractor : std::false_type
        {
            /// \brief The type used as the sort input.
            ///
            typedef T sort_input_type;
            /// \brief Read the sort key from the given value.
            ///
            /// \param v The value to read from.
            /// \return A reference to the sort key.
            ///
            static const sort_input_type &read_data(const sort_input_type &v) noexcept { return v; }
        };

        /// \brief Specialization of ToSortExtractor for types with a `toSort` member.
        ///
        /// Detected via SFINAE: if the type has a member named `toSort`, this
        /// specialization extracts that member as the sort key (std::true_type).
        ///
        /// \author Alessandro Ribeiro
        ///
        /// \tparam T The type containing a `toSort` member.
        ///
        template <typename T>
        struct ToSortExtractor<T, decltype(&T::toSort, void())> : std::true_type
        {
            /// \brief The type of the sort input (the type of the `toSort` member).
            ///
            typedef decltype(T::toSort) sort_input_type;
            /// \brief Read the sort key from the given value.
            ///
            /// \param v The value to read from.
            /// \return A reference to the `toSort` member.
            ///
            static const sort_input_type &read_data(const T &v) noexcept { return v.toSort; }
        };

        /// \brief Perform an in-place radix sort on an array of type T.
        ///
        /// Sorts the array using a least-significant-digit (LSD) radix sort
        /// with counting sort as the stable subroutine. Supports signed integers,
        /// unsigned integers, floats, and doubles via the IntGuessing trait.
        /// Types with a `toSort` member will use that member as the sort key.
        ///
        /// The algorithm processes each byte of the sort key from least significant
        /// to most significant, using a 256-bucket counting sort per byte.
        ///
        /// \author Alessandro Ribeiro
        ///
        /// \tparam T The type of elements to sort.
        /// \param _arr Pointer to the array to sort.
        /// \param arrSize Number of elements in the array.
        /// \param tmp_array Optional external temporary buffer. If null, an internal
        ///        buffer is allocated automatically.
        ///
        /// \code
        /// int32_t arr[] = {5, 2, 9, 1, 5, 6};
        /// AlgorithmCore::Sorting::radix_sort(arr, 6);
        /// // arr is now {1, 2, 5, 5, 6, 9}
        ///
        /// float farr[] = {-1.0f, 3.14f, 0.0f, -2.5f};
        /// AlgorithmCore::Sorting::radix_sort(farr, 4);
        /// // farr is now {-2.5f, -1.0f, 0.0f, 3.14f}
        /// \endcode
        ///
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
            //uint32_t tmp[int_guessing::bytes];
            for (uint32_t j = 0; j < 256; j++)
            {
                for (uint32_t k = 0; k < int_guessing::bytes; k++)
                {
                    uint32_t tmp = counting[j][k];
                    counting[j][k] = acc[k];
                    acc[k] += tmp;
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
