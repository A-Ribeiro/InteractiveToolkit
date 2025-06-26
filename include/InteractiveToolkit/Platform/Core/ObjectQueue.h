#pragma once

// #include "../platform_common.h"
#include "../../common.h"
#include "../Mutex.h"
#include "../AutoLock.h"
#include "../Semaphore.h"

#include "../../ITKCommon/ITKAbort.h"
#include "../../ITKCommon/Memory.h"

#include "./SmartVector.h"

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

        SmartVector<T> queue;

    public:
        // deleted copy constructor and assign operator, to avoid copy...
        ObjectQueue(const ObjectQueue &v) = delete;
        ObjectQueue &operator=(const ObjectQueue &v) = delete;

        ObjectQueue(bool blocking = true)  noexcept : semaphore(0)
        {
            this->blocking = blocking;
            order = QueueOrder::None;
        }

        QueueOrder getOrder() noexcept
        {
            return order;
        }

        T removeInOrder(const T &v, bool *removed = nullptr, bool ignoreSignal = false)
        {
            if (removed != nullptr)
                *removed = false;

            ITK_ABORT(
                order != QueueOrder::Ascending && queue.size() != 0,
                "Trying to dequeue an in-order element of a non-ordered queue.\n");

            bool blockinAquireSuccess = false;
            if (blocking)
            {
                blockinAquireSuccess = semaphore.blockingAcquire();
                if (semaphore.isSignaled() && !ignoreSignal)
                    return T();
            }

            Platform::AutoLock autoLock(&mutex);
            if (queue.size() == 0)
            {
                // not found the data to remove...
                // incr semaphore for another thread
                if (blockinAquireSuccess)
                    semaphore.release();
                return T();
            }

            auto it = std::lower_bound(queue.begin(), queue.end(), v);
            if (it == queue.end() || *it != v)
            {
                // not found the data to remove...
                // incr semaphore for another thread
                if (blockinAquireSuccess)
                    semaphore.release();
                return T();
            }

            // found the data to remove...
            // do not need to release the semaphore here,
            // because we removed an element from the queue
            T result = *it;
            queue.erase(it);
            if (removed != nullptr)
                *removed = true;

            return result;
        }

        T removeInOrderReverse(const T &v, bool *removed = nullptr, bool ignoreSignal = false)
        {
            if (removed != nullptr)
                *removed = false;

            ITK_ABORT(
                order != QueueOrder::Descending && queue.size() != 0,
                "Trying to dequeue an in-order reverse element of a non-ordered queue.\n");

            bool blockinAquireSuccess = false;
            if (blocking)
            {
                blockinAquireSuccess = semaphore.blockingAcquire();
                if (semaphore.isSignaled() && !ignoreSignal)
                    return T();
            }

            Platform::AutoLock autoLock(&mutex);
            if (queue.size() == 0)
            {
                // not found the data to remove...
                // incr semaphore for another thread
                if (blockinAquireSuccess)
                    semaphore.release();
                return T();
            }

            auto it = std::lower_bound(queue.begin(), queue.end(), v, std::greater<T>());
            if (it == queue.end() || *it != v)
            {
                // not found the data to remove...
                // incr semaphore for another thread
                if (blockinAquireSuccess)
                    semaphore.release();
                return T();
            }

            // found the data to remove...
            // do not need to release the semaphore here,
            // because we removed an element from the queue
            T result = *it;
            queue.erase(it);
            if (removed != nullptr)
                *removed = true;

            return result;
        }

        void enqueueInOrder(const T &v)
        {
            Platform::AutoLock autoLock(&mutex);

            ITK_ABORT(
                order != QueueOrder::Ascending && queue.size() != 0,
                "Trying to enqueue element in a non-ordered queue.\n");

            order = QueueOrder::Ascending;

            queue.insert(std::upper_bound(queue.begin(), queue.end(), v), v);

            if (blocking)
                semaphore.release();
        }

        void enqueueInOrderReverse(const T &v)
        {
            Platform::AutoLock autoLock(&mutex);

            ITK_ABORT(
                order != QueueOrder::Descending && queue.size() != 0,
                "Trying to enqueue element in a non-ordered queue.\n");

            order = QueueOrder::Descending;

            queue.insert(std::upper_bound(queue.begin(), queue.end(), v, std::greater<T>()), v);

            if (blocking)
                semaphore.release();
        }

        void enqueue(const T &v)
        {
            Platform::AutoLock autoLock(&mutex);
            order = QueueOrder::None;
            queue.push_back(v);
            if (blocking)
                semaphore.release();
        }

        uint32_t size()
        {
            Platform::AutoLock autoLock(&mutex);
            return (uint32_t)queue.size();
        }

        T peek()
        {
            Platform::AutoLock autoLock(&mutex);
            // if (list.size() > 0)
            //     return list.front();
            if (queue.size() == 0)
                return T();
            T result = queue.front();
            return result;
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

                Platform::AutoLock autoLock(&mutex);

                ITK_ABORT(queue.size() == 0,
                          "Trying to dequeue an element from an empty queue.\n");

                T result = queue.front();
                queue.pop_front();

                if (isSignaled != nullptr)
                    *isSignaled = false;
                return result;
            }

            Platform::AutoLock autoLock(&mutex);

            if (queue.size() == 0)
            {
                if (isSignaled != nullptr)
                    *isSignaled = false;
                return T();
            }

            T result = queue.front();
            queue.pop_front();

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

                Platform::AutoLock autoLock(&mutex);

                ITK_ABORT(queue.size() == 0,
                          "Trying to rdequeue an element from an empty queue.\n");

                T result = queue.back();
                queue.pop_back();

                if (isSignaled != nullptr)
                    *isSignaled = false;
                return result;
            }

            Platform::AutoLock autoLock(&mutex);
            if (queue.size() == 0)
            {
                if (isSignaled != nullptr)
                    *isSignaled = false;
                return T();
            }

            T result = queue.back();
            queue.pop_back();

            if (isSignaled != nullptr)
                *isSignaled = false;
            return result;
        }

        bool isSignaledFromCurrentThread() noexcept
        {
            return semaphore.isSignaled();
        }
    };

}
