#pragma once

//#include "../platform_common.h"
#include "../../common.h"
#include "../Mutex.h"
#include "../AutoLock.h"

#include "../../ITKCommon/ITKAbort.h"
#include "../../ITKCommon/Memory.h"

namespace Platform {

    //
    // This Object Buffer have the default align to 32 bytes
    //   supporting 256 bits instructions ( All SSE and AVX/AVX2 ).
    //
	class ObjectBuffer {

		//private copy constructores, to avoid copy...
        ObjectBuffer(const ObjectBuffer& v){}
        void operator=(const ObjectBuffer& v){}

		ObjectBuffer(ObjectBuffer && v){}
        void operator=(ObjectBuffer && v){}

		bool constructed_from_external_buffer;
		Platform::Mutex mutex;
	public:
		uint8_t *data;
		uint32_t alloc_size;
		uint32_t size;
        int align;

		ObjectBuffer(uint8_t *_data, uint32_t _size, int _align = 32) {
			data = _data;
			size = _size;
            align = _align;
			alloc_size = 0;
			constructed_from_external_buffer = true;
		}

		ObjectBuffer() {
			data = NULL;
			size = 0;
			alloc_size = 0;
            align = 32;
			constructed_from_external_buffer = false;
		}

		~ObjectBuffer() {
			free();
		}

		ObjectBuffer* setSize(uint32_t _size, int _align = 32) {

            ITK_ABORT(constructed_from_external_buffer, "Cannot resize a buffer object constructed from an external buffer.\n");

			Platform::AutoLock autoLock(&mutex);
			
			if (_size == size)
				return this;
			
			if (_size > alloc_size) {
				free();
				data = (uint8_t*)ITKCommon::Memory::malloc(_size, _align);
				alloc_size = _size;
                align = _align;
				//constructed_from_external_buffer = false;
			}

			size = _size;
			return this;
		}

        ObjectBuffer* copy(const ObjectBuffer* src) {
			Platform::AutoLock autoLock(&mutex);

            setSize(src->size, src->align);
            memcpy(data, src->data,size);

            return this;
        }

        ObjectBuffer* copy(ObjectBuffer* src) {
			Platform::AutoLock autoLockSelf(&mutex);
            Platform::AutoLock autoLockOther(&src->mutex);

            setSize(src->size, src->align);
            memcpy(data, src->data,size);

            return this;
        }

		ObjectBuffer* free() {
			Platform::AutoLock autoLock(&mutex);
			if (!constructed_from_external_buffer && data != NULL) {
				ITKCommon::Memory::free(data);
			}

			data = NULL;
			size = 0;
			alloc_size = 0;
			constructed_from_external_buffer = false;

			return this;
		}

	};
}

