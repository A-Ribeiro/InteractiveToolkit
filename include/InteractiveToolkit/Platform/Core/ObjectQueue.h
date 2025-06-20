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

    enum class QueueOrder : uint8_t
    {
        None,
        Ascending,
        Descending
    };

    template <typename T>
    class ObjectQueue
    {
        Platform::Mutex mutex;
        Platform::Semaphore semaphore;
        bool blocking;

        QueueOrder order;

        // std::list<T> list;

        std::vector<T> cyclic_block_array;
        size_t _start;
        size_t _end;
        size_t internal_size;

        ITK_INLINE void checkExpansion(size_t new_size)
        {
            if (new_size > cyclic_block_array.size())
            {
                // resize the cyclic block array
                std::vector<T> new_cyclic_block_array(cyclic_block_array.size() * 2);
                for (size_t i = 0; i < internal_size; i++)
                {
                    new_cyclic_block_array[i] = cyclic_block_array[_start];
                    _start++;
                    if (_start >= cyclic_block_array.size())
                        _start = 0;
                }
                cyclic_block_array = std::move(new_cyclic_block_array);
                _start = 0;
                _end = internal_size;
            }
        }

        ITK_INLINE void pushback(const T &v)
        {
            size_t new_size = internal_size + 1;
            checkExpansion(new_size);

            cyclic_block_array[_end] = v;
            _end++;
            if (_end >= cyclic_block_array.size())
                _end = 0;
            internal_size = new_size;
        }

        ITK_INLINE void pushfront(const T &v)
        {
            size_t new_size = internal_size + 1;
            checkExpansion(new_size);

            _start--;
            if (_start >= cyclic_block_array.size())
                _start = cyclic_block_array.size() - 1;

            cyclic_block_array[_start] = v;
            internal_size = new_size;
        }

        ITK_INLINE T popfront()
        {
            if (internal_size == 0)
                return T();

            T result = cyclic_block_array[_start];
            _start++;
            if (_start >= cyclic_block_array.size())
                _start = 0;

            internal_size--;
            return result;
        }

        ITK_INLINE T popback()
        {
            if (internal_size == 0)
                return T();

            _end--;
            if (_end >= cyclic_block_array.size())
                _end = cyclic_block_array.size() - 1;

            T result = cyclic_block_array[_end];
            internal_size--;
            return result;
        }

        ITK_INLINE void erase(size_t pos)
        {
            if (internal_size == 0)
                return;
            else if (pos == _start)
            {
                popfront();
                return;
            }
            else
            {
                size_t last_element = _end - 1;
                if (last_element >= cyclic_block_array.size())
                    last_element = cyclic_block_array.size() - 1;

                if (pos == last_element)
                {
                    popback();
                    return;
                }
            }

            int64_t distance_to_start = pos - _start;
            int64_t distance_to_end = pos - _end;
            if (distance_to_start < 0)
                distance_to_start += cyclic_block_array.size();
            if (distance_to_end > 0)
                distance_to_end -= cyclic_block_array.size();
            distance_to_end = -distance_to_end; // make it positive

            if (distance_to_end <= distance_to_start)
            {
                // near end, will make less moves removing from the end

                // printf("removing from end\n");

                size_t _next = pos;
                _next++;
                if (_next >= cyclic_block_array.size())
                    _next = 0;

                size_t count = 0;
                while (count < internal_size && pos != _end && _next != _end)
                {
                    cyclic_block_array[pos] = cyclic_block_array[_next];
                    pos = _next;

                    _next++;
                    if (_next >= cyclic_block_array.size())
                        _next = 0;
                    count++;
                }

                _end--;
                if (_end >= cyclic_block_array.size())
                    _end = cyclic_block_array.size() - 1;
            }
            else
            {
                // start end, will make less moves removing from the start

                // printf("removing from start\n");
                size_t _prev = pos;
                _prev--;
                if (_prev >= cyclic_block_array.size())
                    _prev = cyclic_block_array.size() - 1;

                size_t count = 0;
                while (count < internal_size && pos != _start)
                {
                    cyclic_block_array[pos] = cyclic_block_array[_prev];
                    pos = _prev;
                    _prev--;
                    if (_prev >= cyclic_block_array.size())
                        _prev = cyclic_block_array.size() - 1;
                    count++;
                }

                _start++;
                if (_start >= cyclic_block_array.size())
                    _start = 0;
            }

            internal_size--;
        }

        ITK_INLINE void insert(size_t pos, const T &v)
        {
            size_t new_size = internal_size + 1;
            size_t start_beforeExpansion = _start;
            checkExpansion(new_size);
            if (start_beforeExpansion != _start)
            {
                // the cyclic block array was expanded, so we need to adjust the position
                pos += (_start - start_beforeExpansion);
            }

            if (pos == _end)
            {
                // insert at the end
                pushback(v);
                return;
            }
            else if (pos == _start)
            {
                // insert at the start
                pushfront(v);
                return;
            }

            int64_t distance_to_start = pos - _start;
            int64_t distance_to_end = pos - _end;
            if (distance_to_start < 0)
                distance_to_start += cyclic_block_array.size();
            if (distance_to_end > 0)
                distance_to_end -= cyclic_block_array.size();
            distance_to_end = -distance_to_end; // make it positive

            // printf("distance_to_start: %zi, distance_to_end: %zi, pos: %zu, _start: %zu, _end: %zu arr_size: %zu\n",
            //        distance_to_start, distance_to_end, pos, _start, _end, cyclic_block_array.size());

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
                    if (_before_element >= cyclic_block_array.size())
                        _before_element = cyclic_block_array.size() - 1;

                    cyclic_block_array[_last_insert] = cyclic_block_array[_before_element];

                    _last_insert = _before_element;
                    count++;
                }
                _end++;
                if (_end >= cyclic_block_array.size())
                    _end = 0;
            }
            else
            {
                // printf("inserting from start\n");
                // near start, will make less moves inserting from the start
                // move content using start as reference from the position to the end
                size_t _before_element = _start;
                _before_element--;
                if (_before_element >= cyclic_block_array.size())
                    _before_element = cyclic_block_array.size() - 1;
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
                    if (_last_insert >= cyclic_block_array.size())
                        _last_insert = 0;
                    count++;
                }
                _start--;
                if (_start >= cyclic_block_array.size())
                    _start = cyclic_block_array.size() - 1;
            }

            internal_size = new_size;
        }

        ITK_INLINE const T &front() const
        {
            return cyclic_block_array[_start];
        }

        ITK_INLINE const T &back() const
        {
            size_t idx = _end - 1;
            if (idx >= cyclic_block_array.size())
                idx = cyclic_block_array.size() - 1;
            return cyclic_block_array[idx];
        }

    public:
        // deleted copy constructor and assign operator, to avoid copy...
        ObjectQueue(const ObjectQueue &v) = delete;
        ObjectQueue &operator=(const ObjectQueue &v) = delete;

        ObjectQueue(bool blocking = true) : semaphore(0), cyclic_block_array(10)
        {
            this->blocking = blocking;
            order = QueueOrder::None;
            _start = 0;
            _end = 0;
            internal_size = 0;
        }

        QueueOrder getOrder()
        {
            return order;
        }

        T removeInOrder(const T &v, bool *removed = nullptr, bool ignoreSignal = false)
        {
            if (removed != nullptr)
                *removed = false;

            ITK_ABORT(
                order != QueueOrder::Ascending && internal_size != 0,
                "Trying to dequeue an in-order element of a non-ordered queue.\n");

            bool blockinAquireSuccess = false;
            if (blocking)
            {
                blockinAquireSuccess = semaphore.blockingAcquire();
                if (semaphore.isSignaled() && !ignoreSignal)
                    return T();
            }

            Platform::AutoLock autoLock(&mutex);
            if (internal_size == 0)
            {
                // not found the data to remove...
                // incr semaphore for another thread
                if (blockinAquireSuccess)
                    semaphore.release();
                return T();
            }
            else
            {
                {
                    const T &begin = front();
                    if (begin == v)
                    {
                        T result = popfront();
                        if (blockinAquireSuccess)
                            semaphore.release();
                        return result;
                    }
                    else if (v < begin)
                    {
                        // not found the data to remove...
                        // incr semaphore for another thread
                        if (blockinAquireSuccess)
                            semaphore.release();
                        return T();
                    }

                    const T &end = back();
                    if (end == v)
                    {
                        T result = popback();
                        if (blockinAquireSuccess)
                            semaphore.release();
                        return result;
                    }
                    else if (v > end)
                    {
                        // not found the data to remove...
                        // incr semaphore for another thread
                        if (blockinAquireSuccess)
                            semaphore.release();
                        return T();
                    }
                }

                // binary search
                int size = internal_size;
                size_t aux;
                size_t it = _start;
                while (true)
                {
                    int half_size = size / 2;

                    if (half_size == 0)
                    {
                        if (!(cyclic_block_array[it] == v))
                        {
                            it++;
                            if (it >= cyclic_block_array.size())
                                it = 0;
                        }
                        const T &result = cyclic_block_array[it];
                        if (result == v)
                        {
                            erase(it);
                            if (removed != nullptr)
                                *removed = true;
                            if (blockinAquireSuccess)
                                semaphore.release();
                            return result;
                        }

                        if (blockinAquireSuccess)
                            semaphore.release();
                        return T();
                    }

                    aux = it;
                    // std::advance(aux, half_size);
                    aux += half_size;
                    if (aux >= cyclic_block_array.size())
                        aux -= cyclic_block_array.size();

                    if (v < cyclic_block_array[aux])
                    {
                        size = half_size;
                    }
                    else if (v > cyclic_block_array[aux])
                    {
                        it = aux;
                        size = size - half_size;
                    }
                    else
                    {
                        if (cyclic_block_array[aux] == v)
                        {
                            T result = cyclic_block_array[aux];
                            erase(aux);
                            if (removed != nullptr)
                                *removed = true;
                            if (blockinAquireSuccess)
                                semaphore.release();
                            return result;
                        }
                    }
                }
            }

            // not found the data to remove...
            // incr semaphore for another thread
            if (blockinAquireSuccess)
                semaphore.release();
            return T();
        }

        void enqueueInOrder(const T &v)
        {
            mutex.lock();

            ITK_ABORT(
                order != QueueOrder::Ascending && internal_size != 0,
                "Trying to enqueue element in a non-ordered queue.\n");

            order = QueueOrder::Ascending;

            if (internal_size == 0)
            {
                pushback(v);
            }
            else
            {
                const auto &_front = this->front();
                bool compareResult_it_less_equal = (v < _front) ? true : (!(v > _front) ? true : false);
                if (compareResult_it_less_equal)
                {
                    pushfront(v);

                    mutex.unlock();
                    if (blocking)
                        semaphore.release();
                    return;
                }
                const auto &_back = this->back();
                bool compareResult_it_greated_last = (v > _back) ? true : (!(v < _back) ? true : false);
                if (compareResult_it_greated_last)
                {
                    pushback(v);

                    mutex.unlock();
                    if (blocking)
                        semaphore.release();
                    return;
                }

                // binary search
                int size = internal_size;
                size_t aux;
                size_t it = _start;
                while (true)
                {
                    int half_size = size / 2;

                    if (half_size == 0)
                    {
                        it++;
                        if (it >= cyclic_block_array.size())
                            it = 0;
                        insert(it, v);
                        break;
                    }

                    aux = it;
                    // std::advance(aux, half_size);
                    aux += half_size;
                    if (aux >= cyclic_block_array.size())
                        aux -= cyclic_block_array.size();

                    if (v < cyclic_block_array[aux])
                    {
                        size = half_size;
                    }
                    else if (v > cyclic_block_array[aux])
                    {
                        it = aux;
                        size = size - half_size;
                    }
                    else
                    {
                        insert(aux, v);
                        break;
                    }
                }
            }

            mutex.unlock();
            if (blocking)
                semaphore.release();
        }

        void enqueueInOrderReverse(const T &v)
        {
            mutex.lock();

            ITK_ABORT(
                order != QueueOrder::Descending && internal_size != 0,
                "Trying to enqueue element in a non-ordered queue.\n");

            order = QueueOrder::Descending;

            if (internal_size == 0)
            {
                pushback(v);
            }
            else
            {
                const auto &_front = this->front();
                bool compareResult_it_less_equal = (v > _front) ? true : (!(v < _front) ? true : false);
                if (compareResult_it_less_equal)
                {
                    pushfront(v);

                    mutex.unlock();
                    if (blocking)
                        semaphore.release();
                    return;
                }
                const auto &_back = this->back();
                bool compareResult_it_greated_last = (v < _back) ? true : (!(v > _back) ? true : false);
                if (compareResult_it_greated_last)
                {
                    pushback(v);

                    mutex.unlock();
                    if (blocking)
                        semaphore.release();
                    return;
                }

                // binary search
                int size = internal_size;
                size_t aux;
                size_t it = _start;
                while (true)
                {
                    int half_size = size / 2;

                    if (half_size == 0)
                    {
                        it++;
                        if (it >= cyclic_block_array.size())
                            it = 0;
                        insert(it, v);
                        break;
                    }

                    aux = it;
                    // std::advance(aux, half_size);
                    aux += half_size;
                    if (aux >= cyclic_block_array.size())
                        aux -= cyclic_block_array.size();

                    if (v > cyclic_block_array[aux])
                    {
                        size = half_size;
                    }
                    else if (v < cyclic_block_array[aux])
                    {
                        it = aux;
                        size = size - half_size;
                    }
                    else
                    {
                        insert(aux, v);
                        break;
                    }
                }
            }

            mutex.unlock();
            if (blocking)
                semaphore.release();
        }

        void enqueue(const T &v)
        {
            mutex.lock();

            order = QueueOrder::None;

            pushback(v);
            mutex.unlock();

            if (blocking)
                semaphore.release();
        }

        uint32_t size()
        {
            Platform::AutoLock autoLock(&mutex);
            return (uint32_t)internal_size;
        }

        T peek()
        {
            Platform::AutoLock autoLock(&mutex);
            // if (list.size() > 0)
            //     return list.front();
            if (internal_size == 0)
                return T();
            return cyclic_block_array[_start];
        }

        T dequeue(bool *isSignaled = nullptr, bool ignoreSignal = false)
        {

            if (blocking)
            {
                if (!semaphore.blockingAcquire() && !ignoreSignal)
                {
                    if (isSignaled != nullptr)
                        *isSignaled = true;
                    return T();
                }

                mutex.lock();
                T result = popfront();
                mutex.unlock();

                if (isSignaled != nullptr)
                    *isSignaled = false;
                return result;
            }

            mutex.lock();
            T result = popfront();
            mutex.unlock();

            if (isSignaled != nullptr)
                *isSignaled = false;

            return result;
        }

        T rdequeue(bool *isSignaled = nullptr, bool ignoreSignal = false)
        {

            if (blocking)
            {
                if (!semaphore.blockingAcquire() && !ignoreSignal)
                {
                    if (isSignaled != nullptr)
                        *isSignaled = true;
                    return T();
                }

                mutex.lock();
                T result = popback();
                mutex.unlock();

                if (isSignaled != nullptr)
                    *isSignaled = false;
                return result;
            }

            mutex.lock();
            T result = popback();
            mutex.unlock();

            if (isSignaled != nullptr)
                *isSignaled = false;
            return result;
        }

        bool isSignaledFromCurrentThread()
        {
            return semaphore.isSignaled();
        }
    };

}
