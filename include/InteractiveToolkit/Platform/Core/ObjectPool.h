#pragma once

//#include "../platform_common.h"
#include "../../common.h"
#include "../Mutex.h"
#include "../AutoLock.h"

#include "ObjectQueue.h"

#include "../../ITKCommon/ITKAbort.h"
#include "../../ITKCommon/Memory.h"

namespace Platform {

    template <class T>
    class ObjectPool {

        struct ObjectPoolElement {
            bool ignore_placement_new_delete;
            T* data;

            ObjectPoolElement() {
                ignore_placement_new_delete = false;
                data = nullptr;
            }

            bool operator >(const ObjectPoolElement&b)const {
                return (uintptr_t)data > (uintptr_t)b.data;
            }
            bool operator <(const ObjectPoolElement&b)const {
                return (uintptr_t)data < (uintptr_t)b.data;
            }
            bool operator ==(const ObjectPoolElement&b)const {
                return (uintptr_t)data == (uintptr_t)b.data;
            }
        };

        Platform::Mutex mutex;

        ObjectQueue< ObjectPoolElement > available;
        std::unordered_map< T*, ObjectPoolElement > in_use;

        bool released;

        //private copy constructores, to avoid copy...
        ObjectPool(const ObjectPool& v) {}
        ObjectPool& operator=(const ObjectPool& v) {return *this;}

    public:

        ObjectPool() : available(false)
        {
            released = false;
        }

        ~ObjectPool() {
            
            Platform::AutoLock autoLock(&mutex);

            while (available.size() > 0) {
                ObjectPoolElement element = available.dequeue(nullptr,true);
                if (!element.ignore_placement_new_delete)
                    new (element.data) T();
                delete element.data;
            }

            typename std::unordered_map< T*, ObjectPoolElement >::iterator it = in_use.begin();
            while (it != in_use.end()) {
                ObjectPoolElement element = it->second;
                delete element.data;
                it++;
            }
            in_use.clear();

            released = true;
        }

        T* create(bool ignore_placement_new_delete = false) {
            Platform::AutoLock autoLock(&mutex);

            ITK_ABORT(released, "ERROR: trying to create element from a deleted pool");

            if (available.size() > 0) {
                ObjectPoolElement element = available.dequeue();
                element.ignore_placement_new_delete = ignore_placement_new_delete;

                in_use[element.data] = element;

                //placement new operator
                if (!element.ignore_placement_new_delete)
                    new (element.data) T();
                return element.data;
            }
            else {
                ObjectPoolElement newElement;
                newElement.ignore_placement_new_delete = ignore_placement_new_delete;
                newElement.data = new T();

                in_use[newElement.data] = newElement;

                return newElement.data;
            }
        }

        void release(T* data) {
            Platform::AutoLock autoLock(&mutex);

            ITK_ABORT(released, "ERROR: trying to release element from a deleted pool\n");

            //check in_use elements
            typename std::unordered_map< T*, ObjectPoolElement >::iterator it = in_use.find(data);

            if (it != in_use.end()) {
                ObjectPoolElement removedElement = it->second;

                in_use.erase(it);

                //placement delete operator
                if (!removedElement.ignore_placement_new_delete)
                    removedElement.data->~T();

                available.enqueue(removedElement);
            }
            else {
                ITK_ABORT(true, "ERROR: deleting unknown element...\n");
            }

        }
    };

}

