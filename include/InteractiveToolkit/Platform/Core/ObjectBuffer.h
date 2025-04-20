#pragma once

// #include "../platform_common.h"
#include "../../common.h"
#include "../Mutex.h"
#include "../AutoLock.h"

#include "../../ITKCommon/ITKAbort.h"
#include "../../ITKCommon/Memory.h"

namespace Platform
{

    //
    // This Object Buffer have the default align to 32 bytes
    //   supporting 256 bits instructions ( All SSE and AVX/AVX2 ).
    //
    class ObjectBuffer
    {

        bool constructed_from_external_buffer;
        Platform::Mutex mutex;

    public:
        uint8_t *data;
        int64_t alloc_size;
        int64_t size;
        int align;

        ObjectBuffer(uint8_t *_data, int64_t _size, int _align = 32, bool force_copy = false)
        {
            if (force_copy){
                data = nullptr;
                size = 0;
                alloc_size = 0;
                align = 32;
                constructed_from_external_buffer = false;

                setSize(_size, _align);
                memcpy(data, _data, _size);

            } else {
                data = _data;
                size = _size;
                align = _align;
                alloc_size = 0;
                constructed_from_external_buffer = true;
            }
        }

        ObjectBuffer()
        {
            data = nullptr;
            size = 0;
            alloc_size = 0;
            align = 32;
            constructed_from_external_buffer = false;
        }

        // copy construction
        ObjectBuffer(const ObjectBuffer &v)
        {
            *this = v;
        }
        ObjectBuffer& operator=(const ObjectBuffer &v)
        {
            Platform::AutoLock _lovk_self(&mutex);
            // only to clean the vars ref
            if (constructed_from_external_buffer)
                free();
            setSize(v.size, v.align);
            memcpy(data, v.data, size);

            return *this;
        }
        ObjectBuffer(ObjectBuffer &v)
        {
            *this = v;
        }
        ObjectBuffer& operator=(ObjectBuffer &v)
        {
            Platform::AutoLock _lovk_v(&v.mutex);
            *this = *(const ObjectBuffer*)&v;
            return *this;
        }

        // rvalue attribution
        ObjectBuffer(ObjectBuffer &&v)
        {
            *this = std::move(v);
        }
        ObjectBuffer& operator=(ObjectBuffer &&v)
        {
            Platform::AutoLock _lovk_self(&mutex);
            Platform::AutoLock _lovk_v(&v.mutex);

            if (v.constructed_from_external_buffer){
                *this = v;
            } else {
                this->data = v.data;
                this->size = v.size;
                this->alloc_size = v.alloc_size;
                this->align = v.align;
                this->constructed_from_external_buffer = v.constructed_from_external_buffer;

                v.data = nullptr;
                v.size = 0;
                v.alloc_size = 0;
                v.align = 32;
                v.constructed_from_external_buffer = false;
            }
            
            return *this;
        }

        ~ObjectBuffer()
        {
            free();
        }

        ObjectBuffer *setSize(int64_t _size, int _align = 32)
        {

            ITK_ABORT(constructed_from_external_buffer, "Cannot resize a buffer object constructed from an external buffer.\n");

            Platform::AutoLock autoLock(&mutex);

            if (_size == size && _align == align)
                return this;

            if (_size > alloc_size || _align != align)
            {
                free();
                data = (uint8_t *)ITKCommon::Memory::malloc(_size, _align);
                alloc_size = _size;
                align = _align;
                // constructed_from_external_buffer = false;
            }

            size = _size;
            return this;
        }

        ObjectBuffer clone()
        {
            if (constructed_from_external_buffer)
                return ObjectBuffer(data, size, align);
            else
                return *this;
        }

        ObjectBuffer *free()
        {
            Platform::AutoLock autoLock(&mutex);
            if (!constructed_from_external_buffer && data != nullptr)
            {
                ITKCommon::Memory::free(data);
            }

            data = nullptr;
            size = 0;
            alloc_size = 0;
            align = 32;
            constructed_from_external_buffer = false;

            return this;
        }
    };
}
