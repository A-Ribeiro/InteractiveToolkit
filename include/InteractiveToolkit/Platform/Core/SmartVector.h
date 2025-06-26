#pragma once

// #include "../platform_common.h"
#include "../../common.h"
#include "../Mutex.h"
#include "../AutoLock.h"
#include "../Semaphore.h"

#include "../../ITKCommon/ITKAbort.h"
#include "../../ITKCommon/Memory.h"

namespace Platform
{

    template <typename T, int MIN_CAPACITY = 8>
    class SmartVector
    {

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
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using pointer = T *;
            using reference = T &;

            ITK_INLINE iterator(SmartVector &vec, size_t idx, size_t count) noexcept
                : vec(vec), idx(idx), item_count(count)
            {
            }

            ITK_INLINE reference operator*() const noexcept { return vec.cyclic_block_array[idx]; }
            ITK_INLINE pointer operator->() const noexcept { return &vec.cyclic_block_array[idx]; }

            // Prefix increment
            ITK_INLINE iterator operator++() noexcept
            {
                iterator tmp = *this;
                (*this)++;
                return tmp;
            }

            // Postfix increment
            ITK_INLINE iterator &operator++(int) noexcept
            {
                item_count--;
                if (item_count == 0 || item_count > vec.m_capacity)
                {
                    idx = vec.m_capacity; // end
                    return *this;
                }
                idx++;
                if (idx >= vec.m_capacity)
                    idx = 0; // wrap around
                return *this;
            }

            ITK_INLINE iterator &operator+=(int64_t offset) noexcept
            {
                item_count -= offset;
                if (item_count == 0 || item_count > vec.m_capacity)
                {
                    item_count = 0;
                    idx = vec.m_capacity; // end
                    return *this;
                }
                idx += offset;
                if (idx >= vec.m_capacity)
                    idx -= vec.m_capacity; // wrap around
                return *this;
            }

            ITK_INLINE iterator operator+(int64_t offset) const noexcept
            {
                iterator tmp = *this;
                tmp += offset;
                return tmp;
            }

            ITK_INLINE constexpr bool operator==(const iterator &other) const
            {
                return idx == other.idx && &vec == &other.vec;
            }
            ITK_INLINE constexpr bool operator!=(const iterator &other) const
            {
                return !(*this == other);
            }

        private:
            SmartVector &vec;
            size_t idx;
            size_t item_count;

            friend class SmartVector<T>;
        };

        class const_iterator
        {
        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = const T;
            using pointer = const T *;
            using reference = const T &;

            ITK_INLINE const_iterator(const SmartVector &vec, uint32_t idx, size_t count)
                : vec(vec), idx(idx), item_count(count) {}

            ITK_INLINE reference operator*() const { return vec.cyclic_block_array[idx]; }
            ITK_INLINE pointer operator->() const { return &vec.cyclic_block_array[idx]; }

            // Prefix increment
            ITK_INLINE const_iterator operator++()
            {
                const_iterator tmp = *this;
                (*this)++;
                return tmp;
            }

            // Postfix increment
            ITK_INLINE const_iterator &operator++(int)
            {
                item_count--;
                if (item_count == 0 || item_count > vec.m_capacity)
                {
                    item_count = 0;
                    idx = vec.m_capacity; // end
                    return *this;
                }
                idx++;
                if (idx >= vec.m_capacity)
                    idx = 0; // wrap around
                return *this;
            }

            ITK_INLINE const_iterator &operator+=(int64_t offset) noexcept
            {
                item_count -= offset;
                if (item_count == 0 || item_count > vec.m_capacity)
                {
                    item_count = 0;
                    idx = vec.m_capacity; // end
                    return *this;
                }
                idx += offset;
                if (idx >= vec.m_capacity)
                    idx -= vec.m_capacity; // wrap around
                return *this;
            }

            ITK_INLINE const_iterator operator+(int64_t offset) const noexcept
            {
                const_iterator tmp = *this;
                tmp += offset;
                return tmp;
            }

            ITK_INLINE constexpr bool operator==(const const_iterator &other) const
            {
                return idx == other.idx && &vec == &other.vec;
            }
            ITK_INLINE constexpr bool operator!=(const const_iterator &other) const
            {
                return !(*this == other);
            }

        private:
            const SmartVector &vec;
            size_t idx;
            size_t item_count;

            friend class SmartVector<T>;
        };

        ITK_INLINE iterator begin() noexcept { return iterator(*this, _start, internal_size); }
        ITK_INLINE iterator end() noexcept { return iterator(*this, m_capacity, 0); }
        ITK_INLINE const_iterator begin() const noexcept { return const_iterator(*this, _start, internal_size); }
        ITK_INLINE const_iterator end() const noexcept { return const_iterator(*this, m_capacity, 0); }
        ITK_INLINE const_iterator cbegin() const noexcept { return begin(); }
        ITK_INLINE const_iterator cend() const noexcept { return end(); }

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
                            new_cyclic_block_array[i] = cyclic_block_array[_start + i];
                    }
                    else
                    {
                        // Wrap around case
                        size_t first_part_size = m_capacity - _start;
                        for (size_t i = 0; i < first_part_size; i++)
                            new_cyclic_block_array[i] = cyclic_block_array[_start + i];
                        for (size_t i = 0; i < _end; i++)
                            new_cyclic_block_array[first_part_size + i] = cyclic_block_array[i];
                    }
                }

                cyclic_block_array = std::move(new_cyclic_block_array);
                m_capacity = new_capacity;
                _start = 0;
                _end = internal_size;
            }
        }

        ITK_INLINE void internal_insert(size_t pos, const T &v) noexcept
        {
            size_t new_size = internal_size + 1;
            size_t start_beforeExpansion = _start;
            expand_capacity(new_size);
            if (start_beforeExpansion != _start)
            {
                // the cyclic block array was expanded, so we need to adjust the position
                pos += (_start - start_beforeExpansion);
            }

            // this point _end will never be equal to _start,
            // so we can use it as a reference point,
            // unless the vector is empty, then _start == _end == 0

            if (pos == _end)
            {
                // insert at the end
                push_back(v);
                return;
            }
            else if (pos == _start)
            {
                // insert at the start
                push_front(v);
                return;
            }

            int64_t distance_to_start = pos - _start;
            int64_t distance_to_end = pos - _end;
            if (distance_to_start < 0)
                distance_to_start += m_capacity;
            if (distance_to_end > 0)
                distance_to_end -= m_capacity;
            distance_to_end = -distance_to_end; // make it positive

            // printf("distance_to_start: %zi, distance_to_end: %zi, pos: %zu, _start: %zu, _end: %zu arr_size: %zu\n",
            //        distance_to_start, distance_to_end, pos, _start, _end, capacity);

            if (distance_to_end <= distance_to_start)
            {
                // printf("inserting from end\n");
                // near end, will make less moves inserting from the end
                // move content using end as reference until the position
                size_t _last_insert = _end;
                size_t _before_element = _last_insert;
                size_t count = 0;
                while (count < internal_size)
                {
                    if (_last_insert == pos)
                    {
                        cyclic_block_array[_last_insert] = v;
                        break;
                    }
                    _before_element--;
                    if (_before_element >= m_capacity)
                        _before_element = m_capacity - 1;

                    cyclic_block_array[_last_insert] = cyclic_block_array[_before_element];

                    _last_insert = _before_element;
                    count++;
                }
                _end++;
                if (_end >= m_capacity)
                    _end = 0;
            }
            else
            {
                // printf("inserting from start\n");
                // near start, will make less moves inserting from the start
                // move content using start as reference from the position to the end
                size_t _before_element = _start;
                _before_element--;
                if (_before_element >= m_capacity)
                    _before_element = m_capacity - 1;
                size_t _last_insert = _start;
                size_t count = 0;
                while (count < internal_size)
                {
                    if (_last_insert == pos)
                    {
                        cyclic_block_array[_before_element] = v;
                        break;
                    }
                    cyclic_block_array[_before_element] = cyclic_block_array[_last_insert];
                    _before_element = _last_insert;
                    _last_insert++;
                    if (_last_insert >= m_capacity)
                        _last_insert = 0;
                    count++;
                }
                _start--;
                if (_start >= m_capacity)
                    _start = m_capacity - 1;
            }

            internal_size = new_size;
        }

        ITK_INLINE void internal_erase(size_t pos, bool force_moves_from_end) noexcept
        {
            if (internal_size == 0)
                return;
            else if (pos == _start)
            {
                pop_front();
                return;
            }
            else
            {
                size_t last_element = _end - 1;
                if (last_element >= m_capacity)
                    last_element = m_capacity - 1;

                if (pos == last_element)
                {
                    pop_back();
                    return;
                }
            }

            int64_t distance_to_start = pos - _start;
            int64_t distance_to_end = pos - _end;
            if (distance_to_start < 0)
                distance_to_start += m_capacity;
            if (distance_to_end > 0)
                distance_to_end -= m_capacity;
            distance_to_end = -distance_to_end; // make it positive

            if (force_moves_from_end || distance_to_end <= distance_to_start)
            {
                // near end, will make less moves removing from the end

                // printf("removing from end\n");

                size_t _next = pos;
                _next++;
                if (_next >= m_capacity)
                    _next = 0;

                size_t count = 0;
                while (count < internal_size && pos != _end && _next != _end)
                {
                    cyclic_block_array[pos] = cyclic_block_array[_next];
                    pos = _next;

                    _next++;
                    if (_next >= m_capacity)
                        _next = 0;
                    count++;
                }

                _end--;
                if (_end >= m_capacity)
                    _end = m_capacity - 1;
            }
            else
            {
                // start end, will make less moves removing from the start

                // printf("removing from start\n");
                size_t _prev = pos;
                _prev--;
                if (_prev >= m_capacity)
                    _prev = m_capacity - 1;

                size_t count = 0;
                while (count < internal_size && pos != _start)
                {
                    cyclic_block_array[pos] = cyclic_block_array[_prev];
                    pos = _prev;
                    _prev--;
                    if (_prev >= m_capacity)
                        _prev = m_capacity - 1;
                    count++;
                }

                _start++;
                if (_start >= m_capacity)
                    _start = 0;
            }

            internal_size--;
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
                    for (size_t i = 0; i < _end; i++)
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
                        for (size_t i = 0; i < _end; i++)
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

        ITK_INLINE void resize(size_t new_size) noexcept
        {
            expand_capacity(new_size);
            internal_size = new_size;
            _end = _start + internal_size;
            if (_end >= m_capacity)
                _end -= m_capacity;
        }

        ITK_INLINE void clear() noexcept
        {
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

        ITK_INLINE void pop_front() noexcept
        {
            if (internal_size == 0)
                return;
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
            internal_size--;
        }
        ITK_INLINE const T &front() const noexcept
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

            size_t internal_pos = _start + pos;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            internal_insert(internal_pos, v);
        }

        ITK_INLINE void erase(size_t pos, bool force_moves_from_end = false) noexcept
        {
            if (pos >= internal_size)
                return;
            else if (pos == internal_size - 1)
            {
                pop_back();
                return;
            }
            else if (pos == 0)
            {
                pop_front();
                return;
            }

            size_t internal_pos = _start + pos;
            if (internal_pos >= m_capacity)
                internal_pos -= m_capacity;

            internal_erase(internal_pos, force_moves_from_end);
        }
    };

    // enum class QueueOrder : uint8_t
    // {
    //     None,
    //     Ascending,
    //     Descending
    // };

    // template <typename T>
    // class ObjectQueue
    // {
    //     Platform::Mutex mutex;
    //     Platform::Semaphore semaphore;
    //     bool blocking;

    //     QueueOrder order;

    //     // std::list<T> list;

    //     std::vector<T> cyclic_block_array;
    //     size_t _start;
    //     size_t _end;
    //     size_t internal_size;

    // public:
    //     // deleted copy constructor and assign operator, to avoid copy...
    //     ObjectQueue(const ObjectQueue &v) = delete;
    //     ObjectQueue &operator=(const ObjectQueue &v) = delete;

    //     ObjectQueue(bool blocking = true) : semaphore(0), cyclic_block_array(10)
    //     {
    //         this->blocking = blocking;
    //         order = QueueOrder::None;
    //         _start = 0;
    //         _end = 0;
    //         internal_size = 0;
    //     }

    //     QueueOrder getOrder()
    //     {
    //         return order;
    //     }

    //     T removeInOrder(const T &v, bool *removed = nullptr, bool ignoreSignal = false)
    //     {
    //         if (removed != nullptr)
    //             *removed = false;

    //         ITK_ABORT(
    //             order != QueueOrder::Ascending && internal_size != 0,
    //             "Trying to dequeue an in-order element of a non-ordered queue.\n");

    //         bool blockinAquireSuccess = false;
    //         if (blocking)
    //         {
    //             blockinAquireSuccess = semaphore.blockingAcquire();
    //             if (semaphore.isSignaled() && !ignoreSignal)
    //                 return T();
    //         }

    //         Platform::AutoLock autoLock(&mutex);
    //         if (internal_size == 0)
    //         {
    //             // not found the data to remove...
    //             // incr semaphore for another thread
    //             if (blockinAquireSuccess)
    //                 semaphore.release();
    //             return T();
    //         }
    //         else
    //         {
    //             {
    //                 const T &begin = front();
    //                 if (begin == v)
    //                 {
    //                     T result = pop_front();
    //                     if (blockinAquireSuccess)
    //                         semaphore.release();
    //                     return result;
    //                 }
    //                 else if (v < begin)
    //                 {
    //                     // not found the data to remove...
    //                     // incr semaphore for another thread
    //                     if (blockinAquireSuccess)
    //                         semaphore.release();
    //                     return T();
    //                 }

    //                 const T &end = back();
    //                 if (end == v)
    //                 {
    //                     T result = pop_back();
    //                     if (blockinAquireSuccess)
    //                         semaphore.release();
    //                     return result;
    //                 }
    //                 else if (v > end)
    //                 {
    //                     // not found the data to remove...
    //                     // incr semaphore for another thread
    //                     if (blockinAquireSuccess)
    //                         semaphore.release();
    //                     return T();
    //                 }
    //             }

    //             // binary search
    //             int size = internal_size;
    //             size_t aux;
    //             size_t it = _start;
    //             while (true)
    //             {
    //                 int half_size = size / 2;

    //                 if (half_size == 0)
    //                 {
    //                     if (!(cyclic_block_array[it] == v))
    //                     {
    //                         it++;
    //                         if (it >= capacity)
    //                             it = 0;
    //                     }
    //                     const T &result = cyclic_block_array[it];
    //                     if (result == v)
    //                     {
    //                         erase(it);
    //                         if (removed != nullptr)
    //                             *removed = true;
    //                         if (blockinAquireSuccess)
    //                             semaphore.release();
    //                         return result;
    //                     }

    //                     if (blockinAquireSuccess)
    //                         semaphore.release();
    //                     return T();
    //                 }

    //                 aux = it;
    //                 // std::advance(aux, half_size);
    //                 aux += half_size;
    //                 if (aux >= capacity)
    //                     aux -= capacity;

    //                 if (v < cyclic_block_array[aux])
    //                 {
    //                     size = half_size;
    //                 }
    //                 else if (v > cyclic_block_array[aux])
    //                 {
    //                     it = aux;
    //                     size = size - half_size;
    //                 }
    //                 else
    //                 {
    //                     if (cyclic_block_array[aux] == v)
    //                     {
    //                         T result = cyclic_block_array[aux];
    //                         erase(aux);
    //                         if (removed != nullptr)
    //                             *removed = true;
    //                         if (blockinAquireSuccess)
    //                             semaphore.release();
    //                         return result;
    //                     }
    //                 }
    //             }
    //         }

    //         // not found the data to remove...
    //         // incr semaphore for another thread
    //         if (blockinAquireSuccess)
    //             semaphore.release();
    //         return T();
    //     }

    //     void enqueueInOrder(const T &v)
    //     {
    //         mutex.lock();

    //         ITK_ABORT(
    //             order != QueueOrder::Ascending && internal_size != 0,
    //             "Trying to enqueue element in a non-ordered queue.\n");

    //         order = QueueOrder::Ascending;

    //         if (internal_size == 0)
    //         {
    //             push_back(v);
    //         }
    //         else
    //         {
    //             const auto &_front = this->front();
    //             bool compareResult_it_less_equal = (v < _front) ? true : (!(v > _front) ? true : false);
    //             if (compareResult_it_less_equal)
    //             {
    //                 push_front(v);

    //                 mutex.unlock();
    //                 if (blocking)
    //                     semaphore.release();
    //                 return;
    //             }
    //             const auto &_back = this->back();
    //             bool compareResult_it_greated_last = (v > _back) ? true : (!(v < _back) ? true : false);
    //             if (compareResult_it_greated_last)
    //             {
    //                 push_back(v);

    //                 mutex.unlock();
    //                 if (blocking)
    //                     semaphore.release();
    //                 return;
    //             }

    //             // binary search
    //             int size = internal_size;
    //             size_t aux;
    //             size_t it = _start;
    //             while (true)
    //             {
    //                 int half_size = size / 2;

    //                 if (half_size == 0)
    //                 {
    //                     it++;
    //                     if (it >= capacity)
    //                         it = 0;
    //                     insert(it, v);
    //                     break;
    //                 }

    //                 aux = it;
    //                 // std::advance(aux, half_size);
    //                 aux += half_size;
    //                 if (aux >= capacity)
    //                     aux -= capacity;

    //                 if (v < cyclic_block_array[aux])
    //                 {
    //                     size = half_size;
    //                 }
    //                 else if (v > cyclic_block_array[aux])
    //                 {
    //                     it = aux;
    //                     size = size - half_size;
    //                 }
    //                 else
    //                 {
    //                     insert(aux, v);
    //                     break;
    //                 }
    //             }
    //         }

    //         mutex.unlock();
    //         if (blocking)
    //             semaphore.release();
    //     }

    //     void enqueueInOrderReverse(const T &v)
    //     {
    //         mutex.lock();

    //         ITK_ABORT(
    //             order != QueueOrder::Descending && internal_size != 0,
    //             "Trying to enqueue element in a non-ordered queue.\n");

    //         order = QueueOrder::Descending;

    //         if (internal_size == 0)
    //         {
    //             push_back(v);
    //         }
    //         else
    //         {
    //             const auto &_front = this->front();
    //             bool compareResult_it_less_equal = (v > _front) ? true : (!(v < _front) ? true : false);
    //             if (compareResult_it_less_equal)
    //             {
    //                 push_front(v);

    //                 mutex.unlock();
    //                 if (blocking)
    //                     semaphore.release();
    //                 return;
    //             }
    //             const auto &_back = this->back();
    //             bool compareResult_it_greated_last = (v < _back) ? true : (!(v > _back) ? true : false);
    //             if (compareResult_it_greated_last)
    //             {
    //                 push_back(v);

    //                 mutex.unlock();
    //                 if (blocking)
    //                     semaphore.release();
    //                 return;
    //             }

    //             // binary search
    //             int size = internal_size;
    //             size_t aux;
    //             size_t it = _start;
    //             while (true)
    //             {
    //                 int half_size = size / 2;

    //                 if (half_size == 0)
    //                 {
    //                     it++;
    //                     if (it >= capacity)
    //                         it = 0;
    //                     insert(it, v);
    //                     break;
    //                 }

    //                 aux = it;
    //                 // std::advance(aux, half_size);
    //                 aux += half_size;
    //                 if (aux >= capacity)
    //                     aux -= capacity;

    //                 if (v > cyclic_block_array[aux])
    //                 {
    //                     size = half_size;
    //                 }
    //                 else if (v < cyclic_block_array[aux])
    //                 {
    //                     it = aux;
    //                     size = size - half_size;
    //                 }
    //                 else
    //                 {
    //                     insert(aux, v);
    //                     break;
    //                 }
    //             }
    //         }

    //         mutex.unlock();
    //         if (blocking)
    //             semaphore.release();
    //     }

    //     void enqueue(const T &v)
    //     {
    //         mutex.lock();

    //         order = QueueOrder::None;

    //         push_back(v);
    //         mutex.unlock();

    //         if (blocking)
    //             semaphore.release();
    //     }

    //     uint32_t size()
    //     {
    //         Platform::AutoLock autoLock(&mutex);
    //         return (uint32_t)internal_size;
    //     }

    //     T peek()
    //     {
    //         Platform::AutoLock autoLock(&mutex);
    //         // if (list.size() > 0)
    //         //     return list.front();
    //         if (internal_size == 0)
    //             return T();
    //         return cyclic_block_array[_start];
    //     }

    //     T dequeue(bool *isSignaled = nullptr, bool ignoreSignal = false)
    //     {

    //         if (blocking)
    //         {
    //             if (!semaphore.blockingAcquire() && !ignoreSignal)
    //             {
    //                 if (isSignaled != nullptr)
    //                     *isSignaled = true;
    //                 return T();
    //             }

    //             mutex.lock();
    //             T result = pop_front();
    //             mutex.unlock();

    //             if (isSignaled != nullptr)
    //                 *isSignaled = false;
    //             return result;
    //         }

    //         mutex.lock();
    //         T result = pop_front();
    //         mutex.unlock();

    //         if (isSignaled != nullptr)
    //             *isSignaled = false;

    //         return result;
    //     }

    //     T rdequeue(bool *isSignaled = nullptr, bool ignoreSignal = false)
    //     {

    //         if (blocking)
    //         {
    //             if (!semaphore.blockingAcquire() && !ignoreSignal)
    //             {
    //                 if (isSignaled != nullptr)
    //                     *isSignaled = true;
    //                 return T();
    //             }

    //             mutex.lock();
    //             T result = pop_back();
    //             mutex.unlock();

    //             if (isSignaled != nullptr)
    //                 *isSignaled = false;
    //             return result;
    //         }

    //         mutex.lock();
    //         T result = pop_back();
    //         mutex.unlock();

    //         if (isSignaled != nullptr)
    //             *isSignaled = false;
    //         return result;
    //     }

    //     bool isSignaledFromCurrentThread()
    //     {
    //         return semaphore.isSignaled();
    //     }
    // };

}
