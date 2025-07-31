#pragma once

// #include "../platform_common.h"
#include "../../common.h"
// #include "../Mutex.h"
// #include "../AutoLock.h"
// #include "../Semaphore.h"

// #include "../../ITKCommon/ITKAbort.h"
// #include "../../ITKCommon/Memory.h"
#include "../../ITKCommon/STL_Tools.h"
#include <initializer_list>
#include <utility>
#include <new>
#include <iterator>
#include <limits>
#include <algorithm>
#include <type_traits>

namespace Platform
{

    template <typename T, size_t MIN_CAPACITY = 8>
    class SmartVector
    {
    public:
        // STL-compatible type aliases
        using value_type = T;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using reference = T &;
        using const_reference = const T &;
        using pointer = T *;
        using const_pointer = const T *;

    private:
        std::unique_ptr<T[]> cyclic_block_array;
        size_t m_capacity;

        size_t _start;
        size_t _end;
        size_t internal_size;

    public:
        class iterator;
        class const_iterator;
        friend class iterator;
        friend class const_iterator;

        class iterator
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using pointer = T *;
            using reference = T &;

            ITK_INLINE iterator(SmartVector *vec, size_t idx, size_t count) noexcept
                : vec(vec), idx(idx), item_count(count)
            {
                if (item_count == 0)
                {
                    item_count = 0;
                    idx = vec->m_capacity; // end
                }
            }

            ITK_INLINE reference operator*() const noexcept { return vec->cyclic_block_array[idx]; }
            ITK_INLINE pointer operator->() const noexcept { return &vec->cyclic_block_array[idx]; }

            // Prefix increment
            ITK_INLINE iterator &operator++() noexcept
            {
                item_count--;
                if (item_count == 0 || item_count > vec->m_capacity)
                {
                    item_count = 0;
                    idx = vec->m_capacity; // end
                    return *this;
                }
                idx++;
                if (idx >= vec->m_capacity)
                    idx = 0; // wrap around
                return *this;
            }

            // Postfix increment
            ITK_INLINE iterator operator++(int) noexcept
            {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            // Prefix decrement
            ITK_INLINE iterator &operator--() noexcept
            {
                item_count++;
                if (item_count >= vec->internal_size)
                {
                    // Moving before begin, clamp to begin
                    item_count = vec->internal_size;
                    idx = vec->_start;
                    return *this;
                }
                // decrement idx
                if (idx == vec->m_capacity)
                    idx = vec->_end;

                if (idx == 0)
                    idx = vec->m_capacity - 1;
                else
                    idx--;
                return *this;
            }

            // Postfix decrement
            ITK_INLINE iterator operator--(int) noexcept
            {
                iterator tmp = *this;
                --(*this);
                return tmp;
            }

            ITK_INLINE iterator &operator+=(difference_type offset) noexcept
            {
                if (offset == 0)
                    return *this;

                if (offset > 0)
                {
                    item_count -= offset;
                    if (item_count == 0 || item_count > vec->m_capacity)
                    {
                        item_count = 0;
                        idx = vec->m_capacity; // end
                        return *this;
                    }
                    idx += offset;
                    if (idx >= vec->m_capacity)
                        idx -= vec->m_capacity; // wrap around
                }
                else
                {
                    difference_type abs_offset = -offset;
                    item_count += abs_offset;
                    if (item_count >= vec->internal_size)
                    {
                        // Moving before begin, clamp to begin
                        item_count = vec->internal_size;
                        idx = vec->_start;
                        return *this;
                    }
                    if (idx == vec->m_capacity)
                        idx = vec->_end;
                    idx -= abs_offset;
                    if (idx > vec->m_capacity)
                        idx += vec->m_capacity; // wrap around
                }
                return *this;
            }

            ITK_INLINE iterator &operator-=(difference_type offset) noexcept
            {
                return *this += (-offset);
            }

            ITK_INLINE iterator operator+(difference_type offset) const noexcept
            {
                iterator tmp = *this;
                tmp += offset;
                return tmp;
            }

            ITK_INLINE iterator operator-(difference_type offset) const noexcept
            {
                iterator tmp = *this;
                tmp -= offset;
                return tmp;
            }

            ITK_INLINE difference_type operator-(const iterator &other) const noexcept
            {
                if (idx == vec->m_capacity && other.idx == vec->m_capacity)
                    return 0; // both at end
                if (idx == vec->m_capacity)
                    return other.item_count; // this is at end
                if (other.idx == vec->m_capacity)
                    return -(difference_type)item_count; // other is at end

                return (difference_type)other.item_count - (difference_type)item_count;
            }

            ITK_INLINE reference operator[](difference_type n) const noexcept
            {
                iterator tmp = *this + n;
                return *tmp;
            }

            ITK_INLINE constexpr bool operator==(const iterator &other) const noexcept
            {
                return idx == other.idx && vec == other.vec;
            }
            ITK_INLINE constexpr bool operator!=(const iterator &other) const noexcept
            {
                return !(*this == other);
            }

            ITK_INLINE bool operator<(const iterator &other) const noexcept
            {
                if (&vec != &other.vec)
                    return false;
                if (idx == vec->m_capacity)
                    return false; // this is at end
                if (other.idx == vec->m_capacity)
                    return true;                      // other is at end
                return item_count > other.item_count; // lower item_count means later position
            }

            ITK_INLINE bool operator<=(const iterator &other) const noexcept
            {
                return *this < other || *this == other;
            }

            ITK_INLINE bool operator>(const iterator &other) const noexcept
            {
                return !(*this <= other);
            }

            ITK_INLINE bool operator>=(const iterator &other) const noexcept
            {
                return !(*this < other);
            }

        private:
            SmartVector *vec;
            size_t idx;
            size_t item_count;

            friend class SmartVector<T>;
        };

        class const_iterator
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = const T;
            using difference_type = std::ptrdiff_t;
            using pointer = const T *;
            using reference = const T &;

            ITK_INLINE const_iterator(const SmartVector *vec, size_t idx, size_t count) noexcept
                : vec(vec), idx(idx), item_count(count)
            {
                if (item_count == 0)
                {
                    item_count = 0;
                    idx = vec->m_capacity; // end
                }
            }

            ITK_INLINE reference operator*() const noexcept { return vec->cyclic_block_array[idx]; }
            ITK_INLINE pointer operator->() const noexcept { return &vec->cyclic_block_array[idx]; }

            // Prefix increment
            ITK_INLINE const_iterator &operator++() noexcept
            {
                item_count--;
                if (item_count == 0 || item_count > vec->m_capacity)
                {
                    item_count = 0;
                    idx = vec->m_capacity; // end
                    return *this;
                }
                idx++;
                if (idx >= vec->m_capacity)
                    idx = 0; // wrap around
                return *this;
            }

            // Postfix increment
            ITK_INLINE const_iterator operator++(int) noexcept
            {
                const_iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            // Prefix decrement
            ITK_INLINE const_iterator &operator--() noexcept
            {
                item_count++;
                if (item_count >= vec->internal_size)
                {
                    // Moving before begin, clamp to begin
                    item_count = vec->internal_size;
                    idx = vec->_start;
                    return *this;
                }
                // decrement idx
                if (idx == vec->m_capacity)
                    idx = vec->_end;

                if (idx == 0)
                    idx = vec->m_capacity - 1;
                else
                    idx--;
                return *this;
            }

            // Postfix decrement
            ITK_INLINE const_iterator operator--(int) noexcept
            {
                const_iterator tmp = *this;
                --(*this);
                return tmp;
            }

            ITK_INLINE const_iterator &operator+=(difference_type offset) noexcept
            {
                if (offset == 0)
                    return *this;

                if (offset > 0)
                {
                    item_count -= offset;
                    if (item_count == 0 || item_count > vec->m_capacity)
                    {
                        item_count = 0;
                        idx = vec->m_capacity; // end
                        return *this;
                    }
                    idx += offset;
                    if (idx >= vec->m_capacity)
                        idx -= vec->m_capacity; // wrap around
                }
                else
                {
                    difference_type abs_offset = -offset;
                    item_count += abs_offset;
                    if (item_count >= vec->internal_size)
                    {
                        // Moving before begin, clamp to begin
                        item_count = vec->internal_size;
                        idx = vec->_start;
                        return *this;
                    }
                    if (idx == vec->m_capacity)
                        idx = vec->_end;
                    idx -= abs_offset;
                    if (idx > vec->m_capacity)
                        idx += vec->m_capacity; // wrap around
                }
                return *this;
            }

            ITK_INLINE const_iterator &operator-=(difference_type offset) noexcept
            {
                return *this += (-offset);
            }

            ITK_INLINE const_iterator operator+(difference_type offset) const noexcept
            {
                const_iterator tmp = *this;
                tmp += offset;
                return tmp;
            }

            ITK_INLINE const_iterator operator-(difference_type offset) const noexcept
            {
                const_iterator tmp = *this;
                tmp -= offset;
                return tmp;
            }

            ITK_INLINE difference_type operator-(const const_iterator &other) const noexcept
            {
                if (idx == vec->m_capacity && other.idx == vec->m_capacity)
                    return 0; // both at end
                if (idx == vec->m_capacity)
                    return other.item_count; // this is at end
                if (other.idx == vec->m_capacity)
                    return -(difference_type)item_count; // other is at end

                return (difference_type)other.item_count - (difference_type)item_count;
            }

            ITK_INLINE reference operator[](difference_type n) const noexcept
            {
                const_iterator tmp = *this + n;
                return *tmp;
            }

            ITK_INLINE constexpr bool operator==(const const_iterator &other) const noexcept
            {
                return idx == other.idx && vec == other.vec;
            }
            ITK_INLINE constexpr bool operator!=(const const_iterator &other) const noexcept
            {
                return !(*this == other);
            }

            ITK_INLINE bool operator<(const const_iterator &other) const noexcept
            {
                if (&vec != &other.vec)
                    return false;
                if (idx == vec->m_capacity)
                    return false; // this is at end
                if (other.idx == vec->m_capacity)
                    return true;                      // other is at end
                return item_count > other.item_count; // lower item_count means later position
            }

            ITK_INLINE bool operator<=(const const_iterator &other) const noexcept
            {
                return *this < other || *this == other;
            }

            ITK_INLINE bool operator>(const const_iterator &other) const noexcept
            {
                return !(*this <= other);
            }

            ITK_INLINE bool operator>=(const const_iterator &other) const noexcept
            {
                return !(*this < other);
            }

        private:
            const SmartVector *vec;
            size_t idx;
            size_t item_count;

            friend class SmartVector<T>;
        };

        ITK_INLINE iterator begin() noexcept { return iterator(this, (internal_size) ? _start : m_capacity, internal_size); }
        ITK_INLINE iterator end() noexcept { return iterator(this, m_capacity, 0); }
        ITK_INLINE const_iterator begin() const noexcept { return const_iterator(this, (internal_size) ? _start : m_capacity, internal_size); }
        ITK_INLINE const_iterator end() const noexcept { return const_iterator(this, m_capacity, 0); }
        ITK_INLINE const_iterator cbegin() const noexcept { return begin(); }
        ITK_INLINE const_iterator cend() const noexcept { return end(); }

        // Additional STL-compatible iterator type aliases
        using iterator_type = iterator;
        using const_iterator_type = const_iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;

        // Reverse iterator support
        ITK_INLINE reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
        ITK_INLINE reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
        ITK_INLINE const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
        ITK_INLINE const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
        ITK_INLINE const_reverse_iterator crbegin() const noexcept { return rbegin(); }
        ITK_INLINE const_reverse_iterator crend() const noexcept { return rend(); }

    private:
        void expand_capacity(size_t new_size)
        {
            if (new_size > m_capacity)
            {
                size_t new_capacity = m_capacity;
                if (new_capacity == 0)
                    new_capacity = MIN_CAPACITY;
                while (new_size > new_capacity)
                    new_capacity <<= 1;

                auto new_cyclic_block_array = STL_Tools::make_unique<T[]>(new_capacity);

                if (internal_size > 0)
                {
                    if (_start < _end)
                    {
                        // Normal case, no wrap around
                        for (size_t i = 0; i < internal_size; i++)
                            new_cyclic_block_array[i] = std::move(cyclic_block_array[_start + i]);
                    }
                    else
                    {
                        // Wrap around case
                        size_t first_part_size = m_capacity - _start;
                        for (size_t i = 0; i < first_part_size; i++)
                            new_cyclic_block_array[i] = std::move(cyclic_block_array[_start + i]);
                        for (size_t i = 0; i < _end; i++)
                            new_cyclic_block_array[first_part_size + i] = std::move(cyclic_block_array[i]);
                    }
                }

                cyclic_block_array = std::move(new_cyclic_block_array);
                m_capacity = new_capacity;
                _start = 0;
                _end = internal_size;
            }
        }

        // returns the relative_position in the cyclic block array
        ITK_INLINE size_t internal_insert_reserve_empty(size_t abs_pos, size_t increase_count) noexcept
        {
            size_t new_size = internal_size + increase_count;
            expand_capacity(new_size);

            size_t pos = abs_pos + _start;
            if (pos >= m_capacity)
                pos -= m_capacity;

            if (pos == _end)
            {
                // insert at the end
                // push_back(v);
                _end += increase_count;
                if (_end >= m_capacity)
                    _end -= m_capacity;
                internal_size = new_size;
                return pos;
            }
            else if (pos == _start)
            {
                // insert at the start
                // push_front(v);
                _start -= increase_count;
                if (_start >= m_capacity)
                    _start += m_capacity;
                internal_size = new_size;

                // as start change its reference, we need to adjust the internal position
                pos = abs_pos + _start;
                if (pos >= m_capacity)
                    pos -= m_capacity;

                return pos;
            }

            int64_t distance_to_start = pos - _start;
            int64_t distance_to_end = pos - _end;
            if (distance_to_start < 0)
                distance_to_start += m_capacity;
            if (distance_to_end > 0)
                distance_to_end -= m_capacity;
            distance_to_end = -distance_to_end; // make it positive

            if (distance_to_end <= distance_to_start)
            {
                // printf("inserting from end\n");
                // near end, will make less moves inserting from the end
                // move content using end as reference until the position

                size_t _last_insert = _end + increase_count - 1;
                if (_last_insert >= m_capacity)
                    _last_insert -= m_capacity;

                size_t _before_element = _end;
                size_t count = 0;

                size_t pos_stop_iteration = pos + increase_count - 1;
                if (pos_stop_iteration >= m_capacity)
                    pos_stop_iteration -= m_capacity;

                while (count < internal_size)
                {
                    if (_last_insert == pos_stop_iteration)
                    {
                        // cyclic_block_array[_last_insert] = v;
                        break;
                    }
                    _before_element--;
                    if (_before_element >= m_capacity)
                        _before_element = m_capacity - 1;

                    cyclic_block_array[_last_insert] = std::move(cyclic_block_array[_before_element]);
                    // printf("\n  copying -> %i \n", cyclic_block_array[_before_element]);

                    //_last_insert = _before_element;
                    _last_insert--;
                    if (_last_insert >= m_capacity)
                        _last_insert = m_capacity - 1;
                    count++;
                }
                _end += increase_count;
                if (_end >= m_capacity)
                    _end -= m_capacity;
            }
            else
            {
                // printf("inserting from start\n");
                // near start, will make less moves inserting from the start
                // move content using start as reference from the position to the end
                size_t _before_element = _start - increase_count;
                // _before_element--;
                if (_before_element >= m_capacity)
                    _before_element += m_capacity;
                size_t _last_insert = _start;
                size_t count = 0;
                while (count < internal_size)
                {
                    if (_last_insert == pos)
                    {
                        // cyclic_block_array[_before_element] = v;
                        break;
                    }
                    cyclic_block_array[_before_element] = std::move(cyclic_block_array[_last_insert]);
                    // printf("\n  copying -> %i \n", cyclic_block_array[_before_element]);

                    //_before_element = _last_insert;
                    _before_element++;
                    if (_before_element >= m_capacity)
                        _before_element = 0;
                    _last_insert++;
                    if (_last_insert >= m_capacity)
                        _last_insert = 0;
                    count++;
                }
                _start -= increase_count;
                if (_start >= m_capacity)
                    _start += m_capacity;

                // as start change its reference, we need to adjust the internal position
                pos = abs_pos + _start;
                if (pos >= m_capacity)
                    pos -= m_capacity;
            }

            internal_size = new_size;

            return pos;
        }

        // ITK_INLINE void internal_insert_old(size_t pos, const T &v) noexcept
        // {
        //     size_t new_size = internal_size + 1;
        //     size_t start_beforeExpansion = _start;
        //     expand_capacity(new_size);
        //     if (start_beforeExpansion != _start)
        //     {
        //         // the cyclic block array was expanded, so we need to adjust the position
        //         pos += (_start - start_beforeExpansion);
        //     }

        //     // this point _end will never be equal to _start,
        //     // so we can use it as a reference point,
        //     // unless the vector is empty, then _start == _end == 0

        //     if (pos == _end)
        //     {
        //         // insert at the end
        //         push_back(v);
        //         return;
        //     }
        //     else if (pos == _start)
        //     {
        //         // insert at the start
        //         push_front(v);
        //         return;
        //     }

        //     int64_t distance_to_start = pos - _start;
        //     int64_t distance_to_end = pos - _end;
        //     if (distance_to_start < 0)
        //         distance_to_start += m_capacity;
        //     if (distance_to_end > 0)
        //         distance_to_end -= m_capacity;
        //     distance_to_end = -distance_to_end; // make it positive

        //     // printf("distance_to_start: %zi, distance_to_end: %zi, pos: %zu, _start: %zu, _end: %zu arr_size: %zu\n",
        //     //        distance_to_start, distance_to_end, pos, _start, _end, capacity);

        //     if (distance_to_end <= distance_to_start)
        //     {
        //         // printf("inserting from end\n");
        //         // near end, will make less moves inserting from the end
        //         // move content using end as reference until the position
        //         size_t _last_insert = _end;
        //         size_t _before_element = _last_insert;
        //         size_t count = 0;
        //         while (count < internal_size)
        //         {
        //             if (_last_insert == pos)
        //             {
        //                 cyclic_block_array[_last_insert] = v;
        //                 break;
        //             }
        //             _before_element--;
        //             if (_before_element >= m_capacity)
        //                 _before_element = m_capacity - 1;

        //             cyclic_block_array[_last_insert] = std::move(cyclic_block_array[_before_element]);

        //             _last_insert = _before_element;
        //             count++;
        //         }
        //         _end++;
        //         if (_end >= m_capacity)
        //             _end = 0;
        //     }
        //     else
        //     {
        //         // printf("inserting from start\n");
        //         // near start, will make less moves inserting from the start
        //         // move content using start as reference from the position to the end
        //         size_t _before_element = _start;
        //         _before_element--;
        //         if (_before_element >= m_capacity)
        //             _before_element = m_capacity - 1;
        //         size_t _last_insert = _start;
        //         size_t count = 0;
        //         while (count < internal_size)
        //         {
        //             if (_last_insert == pos)
        //             {
        //                 cyclic_block_array[_before_element] = v;
        //                 break;
        //             }
        //             cyclic_block_array[_before_element] = std::move(cyclic_block_array[_last_insert]);
        //             _before_element = _last_insert;
        //             _last_insert++;
        //             if (_last_insert >= m_capacity)
        //                 _last_insert = 0;
        //             count++;
        //         }
        //         _start--;
        //         if (_start >= m_capacity)
        //             _start = m_capacity - 1;
        //     }

        //     internal_size = new_size;
        // }

        ITK_INLINE void internal_erase_v2(size_t abs_pos, size_t erase_count, bool force_moves_from_end) noexcept
        {
            size_t pos = _start + abs_pos;
            if (pos >= m_capacity)
                pos -= m_capacity;

            size_t abs_target_pos = abs_pos + erase_count;
            if (abs_target_pos >= internal_size)
                abs_target_pos = internal_size;

            size_t abs_real_count = abs_target_pos - abs_pos;

            size_t target_pos = _start + abs_target_pos;
            if (target_pos >= m_capacity)
                target_pos -= m_capacity;

            if (internal_size == 0)
                return;
            else if (pos == _start)
            {
                if (force_moves_from_end)
                {
                    size_t _start_write = pos;
                    size_t elements_count = internal_size - abs_target_pos;

                    for (size_t i = 0; i < elements_count; i++)
                    {
                        cyclic_block_array[_start_write] = std::move(cyclic_block_array[target_pos]);

                        _start_write++;
                        if (_start_write >= m_capacity)
                            _start_write = 0;

                        target_pos++;
                        if (target_pos >= m_capacity)
                            target_pos = 0;
                    }
                    for (size_t i = 0; i < abs_real_count; i++)
                        pop_back();
                }
                else
                {
                    for (size_t i = 0; i < abs_real_count; i++)
                        pop_front();
                }
                return;
            }
            else
            {
                if (abs_target_pos == internal_size)
                {
                    for (size_t i = 0; i < abs_real_count; i++)
                        pop_back();
                    return;
                }
            }

            // 0|----<pos>xxxxxxx<target_pos>----|internal_size
            size_t moves_from_start = abs_pos;
            size_t moves_from_end = internal_size - abs_target_pos;

            if (force_moves_from_end || moves_from_end <= moves_from_start || true)
            {
                size_t _start_write = pos;
                for (size_t i = 0; i < moves_from_end; i++)
                {
                    cyclic_block_array[_start_write] = std::move(cyclic_block_array[target_pos]);

                    _start_write++;
                    if (_start_write >= m_capacity)
                        _start_write = 0;

                    target_pos++;
                    if (target_pos >= m_capacity)
                        target_pos = 0;
                }

                for (size_t i = 0; i < abs_real_count; i++)
                    pop_back();
            }
            else
            {
                // 0|----<pos>xxxxxxx<target_pos>----|internal_size
                size_t _start_write = target_pos;
                for (size_t i = 0; i < moves_from_start; i++)
                {
                    pos--;
                    if (pos >= m_capacity)
                        pos = m_capacity - 1;

                    _start_write--;
                    if (_start_write >= m_capacity)
                        _start_write = m_capacity - 1;

                    cyclic_block_array[_start_write] = std::move(cyclic_block_array[pos]);
                }

                for (size_t i = 0; i < abs_real_count; i++)
                    pop_front();
            }
        }

    public:
        ITK_INLINE SmartVector(int initial_size = 0) noexcept
        {
            m_capacity = 0;
            _start = 0;
            _end = 0;
            internal_size = 0;
            resize(initial_size);
        }
        ITK_INLINE SmartVector(const SmartVector &other) noexcept
        {
            m_capacity = 0;
            if (other.internal_size > m_capacity || cyclic_block_array == nullptr)
            {
                m_capacity = other.internal_size;
                if (m_capacity < MIN_CAPACITY)
                    m_capacity = MIN_CAPACITY;
                while (other.internal_size > m_capacity)
                    m_capacity <<= 1;
                if (m_capacity > 0)
                    cyclic_block_array = STL_Tools::make_unique<T[]>(m_capacity);
            }

            internal_size = other.internal_size;

            _start = 0;
            _end = internal_size;

            if (internal_size > 0)
            {
                if (other._start < other._end)
                {
                    // Normal case, no wrap around
                    for (size_t i = 0; i < internal_size; i++)
                        cyclic_block_array[i] = other.cyclic_block_array[other._start + i];
                }
                else
                {
                    // Wrap around case
                    size_t first_part_size = other.m_capacity - other._start;
                    for (size_t i = 0; i < first_part_size; i++)
                        cyclic_block_array[i] = other.cyclic_block_array[other._start + i];
                    for (size_t i = 0; i < other._end; i++)
                        cyclic_block_array[first_part_size + i] = other.cyclic_block_array[i];
                }
            }
        }
        ITK_INLINE SmartVector(SmartVector &&other) noexcept
        {
            m_capacity = other.m_capacity;
            _start = other._start;
            _end = other._end;
            internal_size = other.internal_size;
            cyclic_block_array = std::move(other.cyclic_block_array);
            other.m_capacity = 0;
            other._start = 0;
            other._end = 0;
            other.internal_size = 0;
        }
        ITK_INLINE SmartVector(std::initializer_list<T> ilist) noexcept
        {
            m_capacity = 0;
            _start = 0;
            _end = 0;
            internal_size = 0;
            resize(ilist.size());
            size_t i = 0;
            for (const auto &item : ilist)
            {
                (*this)[i++] = item;
            }
        }
        ITK_INLINE SmartVector &operator=(const SmartVector &other) noexcept
        {
            if (this != &other)
            {
                if (other.internal_size > m_capacity || cyclic_block_array == nullptr)
                {
                    m_capacity = other.internal_size;
                    if (m_capacity < MIN_CAPACITY)
                        m_capacity = MIN_CAPACITY;
                    while (other.internal_size > m_capacity)
                        m_capacity <<= 1;
                    if (m_capacity > 0)
                        cyclic_block_array = STL_Tools::make_unique<T[]>(m_capacity);
                }

                internal_size = other.internal_size;

                _start = 0;
                _end = internal_size;

                if (internal_size > 0)
                {
                    if (other._start < other._end)
                    {
                        // Normal case, no wrap around
                        for (size_t i = 0; i < internal_size; i++)
                            cyclic_block_array[i] = other.cyclic_block_array[other._start + i];
                    }
                    else
                    {
                        // Wrap around case
                        size_t first_part_size = other.m_capacity - other._start;
                        for (size_t i = 0; i < first_part_size; i++)
                            cyclic_block_array[i] = other.cyclic_block_array[other._start + i];
                        for (size_t i = 0; i < other._end; i++)
                            cyclic_block_array[first_part_size + i] = other.cyclic_block_array[i];
                    }
                }
            }
            return *this;
        }
        ITK_INLINE SmartVector &operator=(SmartVector &&other) noexcept
        {
            if (this != &other)
            {
                m_capacity = other.m_capacity;
                _start = other._start;
                _end = other._end;
                internal_size = other.internal_size;
                cyclic_block_array = std::move(other.cyclic_block_array);
                other.m_capacity = 0;
                other._start = 0;
                other._end = 0;
                other.internal_size = 0;
            }
            return *this;
        }

        ITK_INLINE T &operator[](size_t index) noexcept
        {
            index += _start;
            if (index >= m_capacity)
                index -= m_capacity;
            return cyclic_block_array[index];
        }

        ITK_INLINE const T &operator[](size_t index) const noexcept
        {
            index += _start;
            if (index >= m_capacity)
                index -= m_capacity;
            return cyclic_block_array[index];
        }

        ITK_INLINE size_t size() const noexcept
        {
            return internal_size;
        }

        ITK_INLINE size_t capacity() const noexcept
        {
            return m_capacity;
        }

        ITK_INLINE bool empty() const noexcept
        {
            return internal_size == 0;
        }

        ITK_INLINE size_t max_size() const noexcept
        {
            return (std::numeric_limits<size_t>::max)() / sizeof(T);
        }

        ITK_INLINE void reserve(size_t new_capacity) noexcept
        {
            if (new_capacity > m_capacity)
                expand_capacity(new_capacity);
        }

        ITK_INLINE void shrink_to_fit() noexcept
        {
            if (internal_size < m_capacity)
            {
                size_t new_capacity = internal_size;
                if (new_capacity < MIN_CAPACITY)
                    new_capacity = MIN_CAPACITY;

                if (new_capacity < m_capacity)
                {
                    auto new_cyclic_block_array = STL_Tools::make_unique<T[]>(new_capacity);

                    if (internal_size > 0)
                    {
                        if (_start < _end)
                        {
                            // Normal case, no wrap around
                            for (size_t i = 0; i < internal_size; i++)
                                new_cyclic_block_array[i] = std::move(cyclic_block_array[_start + i]);
                        }
                        else
                        {
                            // Wrap around case
                            size_t first_part_size = m_capacity - _start;
                            for (size_t i = 0; i < first_part_size; i++)
                                new_cyclic_block_array[i] = std::move(cyclic_block_array[_start + i]);
                            for (size_t i = 0; i < _end; i++)
                                new_cyclic_block_array[first_part_size + i] = std::move(cyclic_block_array[i]);
                        }
                    }

                    cyclic_block_array = std::move(new_cyclic_block_array);
                    m_capacity = new_capacity;
                    _start = 0;
                    _end = internal_size;
                }
            }
        }

        ITK_INLINE void resize(size_t new_size) noexcept
        {
            resize(new_size, T{});
        }

        ITK_INLINE void resize(size_t new_size, const T &value) noexcept
        {
            if (new_size > internal_size)
            {
                expand_capacity(new_size);
                // Fill new elements with the specified value
                size_t old_size = internal_size;
                internal_size = new_size;
                _end = _start + internal_size;
                if (_end >= m_capacity)
                    _end -= m_capacity;

                // Initialize new elements
                for (size_t i = old_size; i < new_size; ++i)
                    (*this)[i] = value;
            }
            else if (new_size < internal_size)
            {
                // Clear elements that will be removed
                for (size_t i = new_size; i < internal_size; ++i)
                    (*this)[i] = T{};

                internal_size = new_size;
                _end = _start + internal_size;
                if (_end >= m_capacity)
                    _end -= m_capacity;
            }
            // If new_size == internal_size, do nothing
        }

        ITK_INLINE void clear() noexcept
        {
            for (auto &item : *this)
                item = T(); // clear the value
            internal_size = 0;
            _start = _end = 0;
        }

        ITK_INLINE void push_back(const T &v) noexcept
        {
            size_t new_size = internal_size + 1;
            expand_capacity(new_size);

            cyclic_block_array[_end] = v;
            _end++;
            if (_end >= m_capacity)
                _end = 0;
            internal_size = new_size;
        }

        ITK_INLINE void push_back(T &&v) noexcept
        {
            size_t new_size = internal_size + 1;
            expand_capacity(new_size);

            cyclic_block_array[_end] = std::move(v);
            _end++;
            if (_end >= m_capacity)
                _end = 0;
            internal_size = new_size;
        }

        ITK_INLINE void push_front(const T &v) noexcept
        {
            size_t new_size = internal_size + 1;
            expand_capacity(new_size);

            _start--;
            if (_start >= m_capacity)
                _start = m_capacity - 1;

            cyclic_block_array[_start] = v;
            internal_size = new_size;
        }

        ITK_INLINE void push_front(T &&v) noexcept
        {
            size_t new_size = internal_size + 1;
            expand_capacity(new_size);

            _start--;
            if (_start >= m_capacity)
                _start = m_capacity - 1;

            cyclic_block_array[_start] = std::move(v);
            internal_size = new_size;
        }

        // Emplace methods for efficient in-place construction
        template <class... Args>
        ITK_INLINE T &emplace_back(Args &&...args) noexcept
        {
            size_t new_size = internal_size + 1;
            expand_capacity(new_size);

            new (&cyclic_block_array[_end]) T(std::forward<Args>(args)...);

            _end++;
            if (_end >= m_capacity)
                _end = 0;
            internal_size = new_size;

            // Return reference to the newly constructed element
            size_t last_idx = _end - 1;
            if (last_idx >= m_capacity)
                last_idx = m_capacity - 1;
            return cyclic_block_array[last_idx];
        }

        template <class... Args>
        ITK_INLINE T &emplace_front(Args &&...args) noexcept
        {
            size_t new_size = internal_size + 1;
            expand_capacity(new_size);

            _start--;
            if (_start >= m_capacity)
                _start = m_capacity - 1;

            new (&cyclic_block_array[_start]) T(std::forward<Args>(args)...);

            internal_size = new_size;

            return cyclic_block_array[_start];
        }

        template <class... Args>
        ITK_INLINE iterator emplace(iterator pos, Args &&...args) noexcept
        {
            size_t index = internal_size - pos.item_count;
            if (index >= internal_size)
            {
                emplace_back(std::forward<Args>(args)...);
                return iterator(this, (_start + internal_size - 1) % m_capacity, 1);
            }
            else if (index == 0)
            {
                emplace_front(std::forward<Args>(args)...);
                return iterator(this, _start, internal_size);
            }

            // size_t internal_pos = _start + index;
            // if (internal_pos >= m_capacity)
            //     internal_pos -= m_capacity;

            // internal_insert(internal_pos, temp);
            size_t internal_pos = internal_insert_reserve_empty(index, 1);
            new (&cyclic_block_array[internal_pos]) T(std::forward<Args>(args)...);

            return iterator(this, internal_pos, internal_size - index);
        }

        template <class... Args>
        ITK_INLINE iterator emplace(const_iterator pos, Args &&...args) noexcept
        {
            size_t index = internal_size - pos.item_count;
            if (index >= internal_size)
            {
                emplace_back(std::forward<Args>(args)...);
                return iterator(this, (_start + internal_size - 1) % m_capacity, 1);
            }
            else if (index == 0)
            {
                emplace_front(std::forward<Args>(args)...);
                return iterator(this, _start, internal_size);
            }

            // size_t internal_pos = _start + index;
            // if (internal_pos >= m_capacity)
            //     internal_pos -= m_capacity;

            // For simplicity, use existing insert mechanism with temporary object
            // T temp(std::forward<Args>(args)...);
            // internal_insert(internal_pos, temp);
            size_t internal_pos = internal_insert_reserve_empty(index, 1);
            new (&cyclic_block_array[internal_pos]) T(std::forward<Args>(args)...);

            return iterator(this, internal_pos, internal_size - index);
        }

        ITK_INLINE void pop_front() noexcept
        {
            if (internal_size == 0)
                return;
            cyclic_block_array[_start] = T(); // clear the value
            _start++;
            if (_start >= m_capacity)
                _start = 0;
            internal_size--;
        }

        ITK_INLINE void pop_back() noexcept
        {
            if (internal_size == 0)
                return;
            _end--;
            if (_end >= m_capacity)
                _end = m_capacity - 1;
            cyclic_block_array[_end] = T(); // clear the value
            internal_size--;
        }
        ITK_INLINE const T &front() const noexcept
        {
            return cyclic_block_array[_start];
        }
        ITK_INLINE T &front() noexcept
        {
            return cyclic_block_array[_start];
        }

        ITK_INLINE const T &back() const noexcept
        {
            size_t idx = _end - 1;
            if (idx >= m_capacity)
                idx = m_capacity - 1;
            return cyclic_block_array[idx];
        }
        ITK_INLINE T &back() noexcept
        {
            size_t idx = _end - 1;
            if (idx >= m_capacity)
                idx = m_capacity - 1;
            return cyclic_block_array[idx];
        }

        ITK_INLINE void swap(SmartVector &other) noexcept
        {
            std::swap(m_capacity, other.m_capacity);
            std::swap(_start, other._start);
            std::swap(_end, other._end);
            std::swap(internal_size, other.internal_size);
            cyclic_block_array.swap(other.cyclic_block_array);
        }

        // STL-compatible comparison operators
        ITK_INLINE bool operator==(const SmartVector &other) const noexcept
        {
            if (internal_size != other.internal_size)
                return false;

            for (size_t i = 0; i < internal_size; ++i)
            {
                if ((*this)[i] != other[i])
                    return false;
            }
            return true;
        }

        ITK_INLINE bool operator!=(const SmartVector &other) const noexcept
        {
            return !(*this == other);
        }

        ITK_INLINE bool operator<(const SmartVector &other) const noexcept
        {
            return std::lexicographical_compare(begin(), end(), other.begin(), other.end());
        }

        ITK_INLINE bool operator<=(const SmartVector &other) const noexcept
        {
            return !(other < *this);
        }

        ITK_INLINE bool operator>(const SmartVector &other) const noexcept
        {
            return other < *this;
        }

        ITK_INLINE bool operator>=(const SmartVector &other) const noexcept
        {
            return !(*this < other);
        }

        ITK_INLINE void insert(size_t pos, const T &v) noexcept
        {
            if (pos >= internal_size)
            {
                push_back(v);
                return;
            }
            else if (pos == 0)
            {
                push_front(v);
                return;
            }

            size_t internal_pos = internal_insert_reserve_empty(pos, 1);
            cyclic_block_array[internal_pos] = v;
        }

        // ITK_INLINE void insert(const iterator &it, const T &v) noexcept
        // {
        //     insert(internal_size - it.item_count, v);
        // }
        // ITK_INLINE void insert(const const_iterator &it, const T &v) noexcept
        // {
        //     insert(internal_size - it.item_count, v);
        // }

        // Standard library compatible insert overloads
        ITK_INLINE iterator insert(const iterator &pos, const T &value) noexcept
        {
            size_t index = internal_size - pos.item_count;
            insert(index, value);
            size_t internal_pos = _start + index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;
            return iterator(this, internal_pos, internal_size - index);
        }

        ITK_INLINE iterator insert(const const_iterator &pos, const T &value) noexcept
        {
            size_t index = internal_size - pos.item_count;
            insert(index, value);
            size_t internal_pos = _start + index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;
            return iterator(this, internal_pos, internal_size - index);
        }

        ITK_INLINE iterator insert(const iterator &pos, size_t count, const T &value) noexcept
        {
            size_t index = internal_size - pos.item_count;

            size_t internal_pos = internal_insert_reserve_empty(index, count);
            for (size_t i = 0; i < count; ++i)
            {
                cyclic_block_array[internal_pos] = value;
                internal_pos++;
                if (internal_pos >= m_capacity)
                    internal_pos = 0;
            }

            internal_pos = _start + index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            return iterator(this, internal_pos, internal_size - index);
        }

        ITK_INLINE iterator insert(const const_iterator &pos, size_t count, const T &value) noexcept
        {
            size_t index = internal_size - pos.item_count;

            size_t internal_pos = internal_insert_reserve_empty(index, count);
            for (size_t i = 0; i < count; ++i)
            {
                cyclic_block_array[internal_pos] = value;
                internal_pos++;
                if (internal_pos >= m_capacity)
                    internal_pos = 0;
            }

            internal_pos = _start + index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            return iterator(this, internal_pos, internal_size - index);
        }

        template <class InputIt>
        ITK_INLINE typename std::enable_if<!std::is_integral<InputIt>::value, iterator>::type
        insert(const iterator &pos, const InputIt &firstp, const InputIt &last) noexcept
        {
            auto first = firstp;

            size_t index = internal_size - pos.item_count;

            size_t count = std::distance(first, last);
            size_t internal_pos = internal_insert_reserve_empty(index, count);

            for (size_t i = 0; i < count; ++i)
            {
                cyclic_block_array[internal_pos] = *first;
                ++first;
                internal_pos++;
                if (internal_pos >= m_capacity)
                    internal_pos = 0;
            }

            internal_pos = _start + index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            return iterator(this, internal_pos, internal_size - index);
        }

        template <class InputIt>
        ITK_INLINE typename std::enable_if<!std::is_integral<InputIt>::value, iterator>::type
        insert(const const_iterator &pos, const InputIt &firstp, const InputIt &last) noexcept
        {
            auto first = firstp;

            size_t index = internal_size - pos.item_count;

            size_t count = std::distance(first, last);
            size_t internal_pos = internal_insert_reserve_empty(index, count);

            for (size_t i = 0; i < count; ++i)
            {
                cyclic_block_array[internal_pos] = *first;
                ++first;
                internal_pos++;
                if (internal_pos >= m_capacity)
                    internal_pos = 0;
            }

            internal_pos = _start + index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            return iterator(this, internal_pos, internal_size - index);
        }

        template <class InputIt>
        ITK_INLINE iterator insert(const const_iterator &pos, const InputIt &firstp, const InputIt &last) noexcept
        {
            auto first = firstp;

            size_t index = internal_size - pos.item_count;

            size_t count = std::distance(first, last);
            size_t internal_pos = internal_insert_reserve_empty(index, count);

            for (size_t i = 0; i < count; ++i)
            {
                cyclic_block_array[internal_pos] = *first;
                ++first;
                internal_pos++;
                if (internal_pos >= m_capacity)
                    internal_pos = 0;
            }

            internal_pos = _start + index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            return iterator(this, internal_pos, internal_size - index);
        }

        ITK_INLINE iterator insert(const iterator &pos, const std::initializer_list<T> &ilist) noexcept
        {
            size_t index = internal_size - pos.item_count;

            size_t count = ilist.size();
            size_t internal_pos = internal_insert_reserve_empty(index, count);

            auto first = ilist.begin();
            for (size_t i = 0; i < count; ++i)
            {
                cyclic_block_array[internal_pos] = *first;
                ++first;
                internal_pos++;
                if (internal_pos >= m_capacity)
                    internal_pos = 0;
            }

            internal_pos = _start + index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            return iterator(this, internal_pos, internal_size - index);
        }

        ITK_INLINE iterator insert(const const_iterator &pos, const std::initializer_list<T> &ilist) noexcept
        {
            size_t index = internal_size - pos.item_count;

            size_t count = ilist.size();
            size_t internal_pos = internal_insert_reserve_empty(index, count);

            auto first = ilist.begin();
            for (size_t i = 0; i < count; ++i)
            {
                cyclic_block_array[internal_pos] = *first;
                ++first;
                internal_pos++;
                if (internal_pos >= m_capacity)
                    internal_pos = 0;
            }

            internal_pos = _start + index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            return iterator(this, internal_pos, internal_size - index);
        }

        ITK_INLINE void erase(size_t pos, size_t erase_count = 1, bool force_moves_from_end = false) noexcept
        {
            if (pos >= internal_size)
                return;
            // else if (pos == internal_size - 1 && erase_count == 1)
            // {
            //     pop_back();
            //     return;
            // }
            // else if (pos == 0)
            // {
            //     for (size_t i = 0; i < erase_count && internal_size > 0; ++i)
            //         pop_front();
            //     return;
            // }

            internal_erase_v2(pos, erase_count, force_moves_from_end);
        }

        // ITK_INLINE void erase(const iterator &it, bool force_moves_from_end = false) noexcept
        // {
        //     erase(internal_size - it.item_count, force_moves_from_end);
        // }
        // ITK_INLINE void erase(const const_iterator &it, bool force_moves_from_end = false) noexcept
        // {
        //     erase(internal_size - it.item_count, force_moves_from_end);
        // }

        // Standard library compatible erase overloads
        ITK_INLINE iterator erase(const iterator &pos) noexcept
        {
            size_t index = internal_size - pos.item_count;
            if (index >= internal_size)
                return end();

            erase(index, 1);

            // Return iterator to the element after the erased one
            if (index >= internal_size)
                return end();

            size_t internal_pos = _start + index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            return iterator(this, internal_pos, internal_size - index);
        }

        ITK_INLINE iterator erase(const const_iterator &pos) noexcept
        {
            size_t index = internal_size - pos.item_count;
            if (index >= internal_size)
                return end();

            erase(index, 1);

            // Return iterator to the element after the erased one
            if (index >= internal_size)
                return end();

            size_t internal_pos = _start + index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            return iterator(this, internal_pos, internal_size - index);
        }

        ITK_INLINE iterator erase(const iterator &first, const iterator &last) noexcept
        {
            if (first == last)
                return last;

            size_t first_index = internal_size - first.item_count;
            size_t last_index = internal_size - last.item_count;

            if (first_index >= internal_size)
                return end();
            if (last_index > internal_size)
                last_index = internal_size;

            // Erase from back to front to maintain indices
            // for (size_t i = last_index; i > first_index; --i)
            //     erase(i - 1);
            erase(first_index, last_index - first_index);

            // Return iterator to the element after the last erased one
            if (first_index >= internal_size)
                return end();

            size_t internal_pos = _start + first_index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            return iterator(this, internal_pos, internal_size - first_index);
        }

        ITK_INLINE iterator erase(const const_iterator &first, const const_iterator &last) noexcept
        {
            if (first == last)
                return iterator(this, last.idx, last.item_count);

            size_t first_index = internal_size - first.item_count;
            size_t last_index = internal_size - last.item_count;

            if (first_index >= internal_size)
                return end();
            if (last_index > internal_size)
                last_index = internal_size;

            // Erase from back to front to maintain indices
            // for (size_t i = last_index; i > first_index; --i)
            //     erase(i - 1);
            erase(first_index, last_index - first_index);

            // Return iterator to the element after the last erased one
            if (first_index >= internal_size)
                return end();

            size_t internal_pos = _start + first_index;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            return iterator(this, internal_pos, internal_size - first_index);
        }

        // assign methods for std::assign compatibility
        ITK_INLINE void assign(size_t count, const T &value) noexcept
        {
            // clear();
            resize(count);
            for (size_t i = 0; i < count; ++i)
                (*this)[i] = value;
        }

        template <class InputIt>
        ITK_INLINE void assign(const InputIt &firstp, const InputIt &last) noexcept
        {
            size_t count = std::distance(firstp, last);
            resize(count);
            // clear();
            //  for (auto it = first; it != last; ++it)
            //      push_back(*it);

            auto first = firstp;
            size_t idx = this->_start;
            for (size_t i = 0; i < count; ++i)
            {
                cyclic_block_array[idx] = *first;
                ++first;
                idx++;
                if (idx >= m_capacity)
                    idx = 0;
            }
        }

        ITK_INLINE void assign(const std::initializer_list<T> &ilist) noexcept
        {
            size_t count = ilist.size();
            resize(count);
            // clear();
            //  for (const auto &item : ilist)
            //      push_back(item);
            auto first = ilist.begin();
            size_t idx = this->_start;
            for (size_t i = 0; i < count; ++i)
            {
                cyclic_block_array[idx] = *first;
                ++first;
                idx++;
                if (idx >= m_capacity)
                    idx = 0;
            }
        }
    };

    // Global swap function for STL compatibility
    template <typename T, size_t MIN_CAPACITY>
    ITK_INLINE void swap(SmartVector<T, MIN_CAPACITY> &lhs, SmartVector<T, MIN_CAPACITY> &rhs) noexcept
    {
        lhs.swap(rhs);
    }

}
