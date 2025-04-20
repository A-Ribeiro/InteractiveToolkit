#pragma once

//#include "../platform_common.h"
#include "../../common.h"
#include "../Mutex.h"
#include "../AutoLock.h"
#include "../Semaphore.h"

#include "../../ITKCommon/ITKAbort.h"
#include "../../ITKCommon/Memory.h"

namespace Platform {

    enum class QueueOrder : uint8_t {
        None,
        Ascending,
        Descending
    };

    template <typename T>
    class ObjectQueue {
        Platform::Mutex mutex;
        Platform::Semaphore semaphore;
        bool blocking;
        
        QueueOrder order;

        std::list<T> list;

        //private copy constructores, to avoid copy...
        ObjectQueue(const ObjectQueue& v) {}
        ObjectQueue& operator=(const ObjectQueue& v) {return *this;}

    public:

        ObjectQueue(bool blocking = true) :semaphore(0) {
            this->blocking = blocking;
            order = QueueOrder::None;
        }

        QueueOrder getOrder() {
            return order;
        }

        T removeInOrder(const T &v, bool ignoreSignal = false) {

            ITK_ABORT(
                order != QueueOrder::Ascending && list.size() != 0, 
                "Trying to dequeue an in-order element of a non-ordered queue.\n");

            bool blockinAquireSuccess = false;
            if (blocking) {
                blockinAquireSuccess = semaphore.blockingAcquire();
                if (semaphore.isSignaled() && !ignoreSignal)
                    return T();
            }

            Platform::AutoLock autoLock(&mutex);

            if (list.size() == 0) {
                // not found the data to remove... 
                // incr semaphore for another thread
                if (blockinAquireSuccess)
                    semaphore.release();
                return T();
            }
            else {
                {
                    T begin = list.front();
                    if (begin == v) {
                        list.pop_front();
                        if (blockinAquireSuccess)
                            semaphore.release();
                        return begin;
                    }
                    else if (v < begin) {
                        // not found the data to remove... 
                        // incr semaphore for another thread
                        if (blockinAquireSuccess)
                            semaphore.release();
                        return T();
                    }

                    T end = list.back();
                    if (end == v) {
                        list.pop_back();
                        if (blockinAquireSuccess)
                            semaphore.release();
                        return end;
                    }
                    else if (v > end) {
                        // not found the data to remove... 
                        // incr semaphore for another thread
                        if (blockinAquireSuccess)
                            semaphore.release();
                        return T();
                    }
                }

                //binary search
                int size = list.size();
                typename std::list<T>::iterator aux;
                typename std::list<T>::iterator it = list.begin();
                while (true) {
                    int half_size = size / 2;

                    if (half_size == 0) {
                        T result = (*it);
                        if (!(result == v)) {
                            it++;
                            result = (*it);
                        }
                        list.erase(it);
                        return result;
                    }

                    aux = it;
                    std::advance(aux, half_size);

                    if (v < *aux) {
                        size = half_size;
                    }
                    else if (v > *aux) {
                        it = aux;
                        size = size - half_size;
                    }
                    else {
                        if ((*aux) == v) {
                            T result = (*aux);
                            list.erase(aux);
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

        void enqueueInOrder(const T &v) {
            mutex.lock();

            ITK_ABORT(
                order != QueueOrder::Ascending && list.size() != 0, 
                "Trying to enqueue element in a non-ordered queue.\n");
            
            order = QueueOrder::Ascending;

            if (list.size() == 0) {
                list.push_back(v);
            }
            else {
                bool compareResult_it_less_equal = (v < list.front()) ? true : (!(v > list.front()) ? true : false);
                if (compareResult_it_less_equal) {
                    list.push_front(v);

                    mutex.unlock();
                    if (blocking)
                        semaphore.release();
                    return;
                }
                bool compareResult_it_greated_last = (v > list.back()) ? true : (!(v < list.back()) ? true : false);
                if (compareResult_it_greated_last) {
                    list.push_back(v);

                    mutex.unlock();
                    if (blocking)
                        semaphore.release();
                    return;
                }

                //binary search
                int size = list.size();
                typename std::list<T>::iterator aux;
                typename std::list<T>::iterator it = list.begin();
                while (true) {
                    int half_size = size / 2;

                    if (half_size == 0) {
                        it++;
                        list.insert(it, v);
                        break;
                    }

                    aux = it;
                    std::advance(aux, half_size);

                    if (v < *aux) {
                        size = half_size;
                    }
                    else if (v > *aux) {
                        it = aux;
                        size = size - half_size;
                    }
                    else {
                        list.insert(aux, v);
                        break;
                    }
                }

            }

            mutex.unlock();
            if (blocking)
                semaphore.release();
        }

        void enqueueInOrderReverse(const T &v) {
            mutex.lock();

            ITK_ABORT(
                order != QueueOrder::Descending && list.size() != 0, 
                "Trying to enqueue element in a non-ordered queue.\n");
            
            order = QueueOrder::Descending;

            if (list.size() == 0) {
                list.push_back(v);
            }
            else {
                bool compareResult_it_less_equal = (v > list.front()) ? true : (!(v < list.front()) ? true : false);
                if (compareResult_it_less_equal) {
                    list.push_front(v);

                    mutex.unlock();
                    if (blocking)
                        semaphore.release();
                    return;
                }
                bool compareResult_it_greated_last = (v < list.back()) ? true : (!(v > list.back()) ? true : false);
                if (compareResult_it_greated_last) {
                    list.push_back(v);

                    mutex.unlock();
                    if (blocking)
                        semaphore.release();
                    return;
                }

                //binary search
                int size = list.size();
                typename std::list<T>::iterator aux;
                typename std::list<T>::iterator it = list.begin();
                while (true) {
                    int half_size = size / 2;

                    if (half_size == 0) {
                        it++;
                        list.insert(it, v);
                        break;
                    }

                    aux = it;
                    std::advance(aux, half_size);

                    if (v > *aux) {
                        size = half_size;
                    }
                    else if (v < *aux) {
                        it = aux;
                        size = size - half_size;
                    }
                    else {
                        list.insert(aux, v);
                        break;
                    }
                }

            }


            mutex.unlock();
            if (blocking)
                semaphore.release();

        }

        void enqueue(const T &v) {
            mutex.lock();

            //ITK_ABORT(ordered == true, "Trying to enqueue element in an ordered queue.\n");
            order = QueueOrder::None;

            list.push_back(v);
            mutex.unlock();

            if (blocking)
                semaphore.release();
        }

        uint32_t size() {
            Platform::AutoLock autoLock(&mutex);
            return (uint32_t)list.size();
        }

        T peek() {
            Platform::AutoLock autoLock(&mutex);
            if (list.size() > 0)
                return list.front();
            return T();
        }

        T dequeue(bool *isSignaled = nullptr, bool ignoreSignal = false) {

            if (blocking) {
                if (!semaphore.blockingAcquire() && !ignoreSignal){
                    if (isSignaled != nullptr)
                        *isSignaled = true;
                    return T();
                }

                mutex.lock();
                T result = list.front();
                list.pop_front();
                mutex.unlock();

                if (isSignaled != nullptr)
                    *isSignaled = false;
                return result;
            }

            mutex.lock();
            if (list.size() > 0) {
                T result = list.front();
                list.pop_front();
                mutex.unlock();
                if (isSignaled != nullptr)
                    *isSignaled = false;
                return result;
            }
            mutex.unlock();

            if (isSignaled != nullptr)
                *isSignaled = false;
            return T();
        }

        T rdequeue(bool *isSignaled = nullptr, bool ignoreSignal = false) {

            if (blocking) {
                if (!semaphore.blockingAcquire() && !ignoreSignal) {
                    if (isSignaled != nullptr)
                        *isSignaled = true;
                    return T();
                }

                mutex.lock();
                T result = list.back();
                list.pop_back();
                mutex.unlock();
                if (isSignaled != nullptr)
                    *isSignaled = false;
                return result;
            }

            mutex.lock();
            if (list.size() > 0) {
                T result = list.back();
                list.pop_back();
                mutex.unlock();
                if (isSignaled != nullptr)
                    *isSignaled = false;
                return result;
            }
            mutex.unlock();
            if (isSignaled != nullptr)
                *isSignaled = false;
            return T();
        }

        bool isSignaledFromCurrentThread() {
            return semaphore.isSignaled();
        }
    };

}
