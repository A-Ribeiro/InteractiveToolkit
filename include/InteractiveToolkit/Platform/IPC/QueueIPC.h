#pragma once

#include "../platform_common.h"
#include "../Core/ObjectBuffer.h"
#include "../Thread.h"
#include "../Sleep.h"
#include "../../ITKCommon/Path.h"
#include "../../ITKCommon/ITKAbort.h"
#include "../Signal.h"
#include "ConditionIPC.h"

namespace Platform
{

    namespace IPC
    {

        const uint32_t QueueIPC_READ = 1 << 0;
        const uint32_t QueueIPC_WRITE = 1 << 1;

        struct QueueHeader
        {
            uint32_t subscribers_count;

            uint32_t write_pos;
            uint32_t read_pos;

            uint32_t queue_size;
            uint32_t buffer_size;

            uint32_t capacity;
            uint32_t size;
        };

        struct BufferHeader
        {
            uint32_t size;
        };

        class QueueIPC : public EventCore::HandleCallback
        {

            std::string name;

            std::string header_name;
            std::string buffer_name;
            std::string semaphore_name;

            void lock(bool from_constructor = false)
            {
                Platform::AutoLock autoLock(&shm_mutex);

#if defined(_WIN32)
                // lock semaphore
                if (queue_semaphore != NULL)
                    ITK_ABORT(WaitForSingleObject(queue_semaphore, INFINITE) != WAIT_OBJECT_0, "Error to lock queue semaphore. Error code: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#elif defined(__linux__) || defined(__APPLE__)
                if (from_constructor)
                {

                    ITK_ABORT(f_lock != -1, "Trying to lock twice from constructor.\n");

                    // file lock ... to solve the dead semaphore reinitialization...
                    std::string global_lock_file = ITKCommon::Path::getDocumentsPath("ITKLib", "lock") + ITKCommon::PATH_SEPARATOR + this->name + std::string(".q.f_lock");

                    f_lock = open(global_lock_file.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
                    ITK_ABORT(f_lock == -1, "Error to open f_lock. Error code: %s\n", strerror(errno));

                    int rc = lockf(f_lock, F_LOCK, 0);
                    ITK_ABORT(rc == -1, "Error to lock f_lock. Error code: %s\n", strerror(errno));
                }
                else
                {
                    // while semaphore is signaled, try to aquire until block...
                    if (queue_semaphore != NULL)
                        while (sem_wait(queue_semaphore) != 0)
                            ;
                }
#endif
            }

            void unlock(bool from_constructor = false)
            {
                Platform::AutoLock autoLock(&shm_mutex);

#if defined(_WIN32)
                // release semaphore
                if (queue_semaphore != NULL)
                    ITK_ABORT(!ReleaseSemaphore(queue_semaphore, 1, NULL), "Error to unlock queue semaphore. Error code: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#elif defined(__linux__) || defined(__APPLE__)
                if (from_constructor)
                {
                    ITK_ABORT(f_lock == -1, "Trying to unlock a non initialized lock from constructor.\n");

                    int rc = lockf(f_lock, F_ULOCK, 0);
                    ITK_ABORT(rc == -1, "Error to unlock f_lock. Error code: %s\n", strerror(errno));

                    close(f_lock);
                    f_lock = -1;
                }
                else
                {
                    if (queue_semaphore != NULL)
                        sem_post(queue_semaphore);
                }
#endif
            }

            void write_buffer(const uint8_t *data, uint32_t size)
            {

                ITK_ABORT(size > (queue_header_ptr->capacity - queue_header_ptr->size), "Error to write more than the buffer size\n.");

                uint32_t remaining_space = queue_header_ptr->capacity - queue_header_ptr->write_pos;
                if (remaining_space < size)
                {
                    // two memcpy
                    memcpy(&queue_buffer_ptr[queue_header_ptr->write_pos], data, remaining_space);
                    memcpy(&queue_buffer_ptr[0], &data[remaining_space], size - remaining_space);
                }
                else
                {
                    // one memcpy
                    memcpy(&queue_buffer_ptr[queue_header_ptr->write_pos], data, size);
                }
                queue_header_ptr->write_pos = (queue_header_ptr->write_pos + size) % queue_header_ptr->capacity;
                queue_header_ptr->size += size;
            }

            void read_buffer(uint8_t *data, uint32_t size)
            {

                ITK_ABORT(size > queue_header_ptr->size, "Error to read more than the buffer size\n.");

                uint32_t remaining_space = queue_header_ptr->capacity - queue_header_ptr->read_pos;
                if (remaining_space < size)
                {
                    // two memcpy
                    memcpy(data, &queue_buffer_ptr[queue_header_ptr->read_pos], remaining_space);
                    memcpy(&data[remaining_space], &queue_buffer_ptr[0], size - remaining_space);
                }
                else
                {
                    // one memcpy
                    memcpy(data, &queue_buffer_ptr[queue_header_ptr->read_pos], size);
                }
                queue_header_ptr->read_pos = (queue_header_ptr->read_pos + size) % queue_header_ptr->capacity;
                queue_header_ptr->size -= size;
            }

            // private copy constructores, to avoid copy...
            QueueIPC(const QueueIPC &v) {}
            void operator=(const QueueIPC &v) {}

            void onAbort(const char *file, int line, const char *message)
            {
                releaseAll();
            }

            void releaseAll()
            {
                Platform::AutoLock autoLock(&shm_mutex);

                ITKCommon::ITKAbort::Instance()->OnAbort.remove(&QueueIPC::onAbort, this);

#if !defined(_WIN32)
                lock(true);
                bool is_last_queue = false;

                if (queue_header_handle != BUFFER_HANDLE_NULL)
                {
                    is_last_queue = (queue_header_ptr->subscribers_count - 1) == 0;
                }
#endif

                lock();

                if (queue_buffer_handle != BUFFER_HANDLE_NULL)
                {
#if defined(_WIN32)
                    if (queue_buffer_ptr != 0)
                        UnmapViewOfFile(queue_buffer_ptr);
                    CloseHandle(queue_buffer_handle);
#elif defined(__linux__) || defined(__APPLE__)
                    if (queue_buffer_ptr != MAP_FAILED)
                        munmap(queue_buffer_ptr, queue_header_ptr->capacity);
                    close(queue_buffer_handle); // close FD
                                                // shm_unlink(buffer_name.c_str());
#endif
                    queue_buffer_handle = BUFFER_HANDLE_NULL;
                }

                if (queue_header_handle != BUFFER_HANDLE_NULL)
                {

                    queue_header_ptr->subscribers_count--;

#if defined(_WIN32)
                    if (queue_header_ptr != 0)
                        UnmapViewOfFile(queue_header_ptr);
                    CloseHandle(queue_header_handle);
#elif defined(__linux__) || defined(__APPLE__)
                    if (queue_header_ptr != MAP_FAILED)
                        munmap(queue_header_ptr, sizeof(QueueHeader));
                    close(queue_header_handle); // close FD
                                                // shm_unlink(header_name.c_str());
#endif
                    queue_header_handle = BUFFER_HANDLE_NULL;
                }

                unlock();

                if (queue_semaphore != NULL)
                {
#if defined(_WIN32)
                    CloseHandle(queue_semaphore);
#elif defined(__linux__) || defined(__APPLE__)
                    sem_close(queue_semaphore);
                    // sem_unlink(semaphore_name.c_str());
#endif

                    queue_semaphore = NULL;
                }

#if !defined(_WIN32)
                // unlink all resources
                if (is_last_queue)
                {
                    printf("[linux] ... last queue, unlink resources ...\n");
                    shm_unlink(buffer_name.c_str());
                    shm_unlink(header_name.c_str());
                    sem_unlink(semaphore_name.c_str());

                    // unlink the lock_f
                    std::string global_lock_file = ITKCommon::Path::getDocumentsPath("ITKLib", "lock") + ITKCommon::PATH_SEPARATOR + this->name + std::string(".q.f_lock");
                    unlink(global_lock_file.c_str());
                }

                unlock(true);
#endif
            }

            Platform::Mutex shm_mutex;

        public:

        #if defined(__linux__) || defined(__APPLE__)
            // unlink all resources
            static void force_shm_unlink(const std::string &name)
            {
                printf("[QueueIPC] force_shm_unlink: %s\n", name.c_str());

                std::string header_name = std::string("/") + std::string(name) + std::string("_aqh");    // aribeiro_queue_header
                std::string buffer_name = std::string("/") + std::string(name) + std::string("_aqb");    // aribeiro_queue_buffer
                std::string semaphore_name = std::string("/") + std::string(name) + std::string("_aqs"); // aribeiro_queue_semaphore

                shm_unlink(buffer_name.c_str());
                shm_unlink(header_name.c_str());
                sem_unlink(semaphore_name.c_str());

                // unlink the lock_f
                std::string global_lock_file = ITKCommon::Path::getDocumentsPath("ITKLib", "lock") + ITKCommon::PATH_SEPARATOR + name + std::string(".q.f_lock");
                unlink(global_lock_file.c_str());
            }
#endif

#if defined(_WIN32)
            HANDLE queue_semaphore;
            HANDLE queue_header_handle;
            HANDLE queue_buffer_handle;
#elif defined(__linux__) || defined(__APPLE__)

            sem_t *queue_semaphore;  // semaphore
            int queue_header_handle; // FD
            int queue_buffer_handle; // FD

            int f_lock;

#endif

            QueueHeader *queue_header_ptr; // read/write queue header
            uint8_t *queue_buffer_ptr;     // readonly or writeonly

            QueueIPC(const char *name = "default",
                     uint32_t mode = QueueIPC_READ | QueueIPC_WRITE,
                     uint32_t queue_size_ = 64,
                     uint32_t buffer_size_ = 1024)
            {

                Platform::AutoLock autoLock(&shm_mutex);
                ITKCommon::ITKAbort::Instance()->OnAbort.add(&QueueIPC::onAbort, this);

                queue_semaphore = NULL;
                queue_header_handle = BUFFER_HANDLE_NULL;
                queue_buffer_handle = BUFFER_HANDLE_NULL;

                this->name = name;

#if defined(_WIN32)
                header_name = std::string(name) + std::string("_aqh");    // aribeiro_queue_header
                buffer_name = std::string(name) + std::string("_aqb");    // aribeiro_queue_buffer
                semaphore_name = std::string(name) + std::string("_aqs"); // aribeiro_queue_semaphore
#elif defined(__linux__) || defined(__APPLE__)
                header_name = std::string("/") + std::string(name) + std::string("_aqh");    // aribeiro_queue_header
                buffer_name = std::string("/") + std::string(name) + std::string("_aqb");    // aribeiro_queue_buffer
                semaphore_name = std::string("/") + std::string(name) + std::string("_aqs"); // aribeiro_queue_semaphore

                f_lock = -1;
#endif

#if defined(_WIN32)
                SECURITY_DESCRIPTOR sd;
                InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
                SetSecurityDescriptorDacl(&sd, TRUE, (PACL)0, FALSE);

                SECURITY_ATTRIBUTES sa;
                sa.nLength = sizeof(sa);
                sa.lpSecurityDescriptor = &sd;
                sa.bInheritHandle = FALSE;

                queue_semaphore = CreateSemaphoreA(
                    &sa,                   // default security attributes
                    1,                     // initial count
                    LONG_MAX,              // maximum count
                    semaphore_name.c_str() // named semaphore
                );
                ITK_ABORT(queue_semaphore == 0, "Error to create global semaphore. Error code: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());

#elif defined(__linux__) || defined(__APPLE__)

                /*

                queue_semaphore = sem_open(
                    semaphore_name.c_str(),
                    O_CREAT,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
                    1
                );

                ITK_ABORT(queue_semaphore == SEM_FAILED, "Error to create global semaphore. Error code: %s\n", strerror(errno));

                */

#endif

                lock(true);

#if defined(_WIN32)
                // open the header memory section
                queue_header_handle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(QueueHeader), header_name.c_str());
                if (queue_header_handle == 0)
                {
                    unlock(true);
                    ITK_ABORT(true, "Error to create the header IPC queue.\n");
                }
                queue_header_ptr = (QueueHeader *)MapViewOfFile(queue_header_handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(QueueHeader));
                if (queue_header_ptr == 0)
                {
                    unlock(true);
                    ITK_ABORT(true, "Error to map the header IPC buffer.\n");
                }
#elif defined(__linux__) || defined(__APPLE__)
                queue_header_handle = shm_open(header_name.c_str(),
                                               O_CREAT | O_RDWR,
                                               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
                if (queue_header_handle == -1)
                {
                    queue_header_handle = BUFFER_HANDLE_NULL;
                    unlock(true);
                    ITK_ABORT(true, "Error to create the header IPC queue. Error code: %s\n", strerror(errno));
                }

                int rc;

                struct stat _stat;
                rc = fstat(queue_header_handle, &_stat);
                ITK_ABORT(rc != 0, "Error to stat the file descriptor. Error code: %s\n", strerror(errno));
                if (_stat.st_size == 0)
                {
                    rc = ftruncate(queue_header_handle, sizeof(QueueHeader));
                    // fallocate(queue_header_handle, 0, 0, sizeof(QueueHeader));
                    ITK_ABORT(rc != 0, "Error to truncate buffer. Error code: %s\n", strerror(errno));
                }

                queue_header_ptr = (QueueHeader *)mmap(
                    NULL,
                    sizeof(QueueHeader),
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED,
                    queue_header_handle,
                    0);
                if (queue_header_ptr == MAP_FAILED)
                {
                    unlock(true);
                    ITK_ABORT(true, "Error to map the header IPC buffer. Error code: %s\n", strerror(errno));
                }
#endif

                if (queue_header_ptr->subscribers_count > 0)
                {
                    printf("[QueueIPC] Not First Opened - Retrieving Shared Memory Information...\n");

#if !defined(_WIN32)

                    // initialize semaphore
                    queue_semaphore = sem_open(
                        semaphore_name.c_str(),
                        O_CREAT,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
                        1);

                    if (queue_semaphore == SEM_FAILED)
                    {
                        queue_semaphore = NULL;
                        unlock(true);
                        ITK_ABORT(true, "Error to create global semaphore. Error code: %s\n", strerror(errno));
                    }

                    lock(); // lock the created semaphore

#endif
                }
                else
                {
                    printf("[QueueIPC] First Opened - Creating Shared Memory...\n");

#if !defined(_WIN32)
                    // truncate semaphore before initialize it
                    sem_unlink(semaphore_name.c_str());

                    // initialize semaphore
                    queue_semaphore = sem_open(
                        semaphore_name.c_str(),
                        O_CREAT,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
                        1);

                    if (queue_semaphore == SEM_FAILED)
                    {
                        queue_semaphore = NULL;
                        unlock(true);
                        ITK_ABORT(true, "Error to create global semaphore. Error code: %s\n", strerror(errno));
                    }

                    lock(); // lock the created semaphore
#endif

                    queue_header_ptr->write_pos = 0;
                    queue_header_ptr->read_pos = 0;

                    queue_header_ptr->queue_size = queue_size_;
                    queue_header_ptr->buffer_size = buffer_size_ + sizeof(BufferHeader);
                    queue_header_ptr->capacity = queue_header_ptr->buffer_size * queue_header_ptr->queue_size;
                    queue_header_ptr->size = 0;
                }

#if defined(_WIN32)
                // open the buffer memory section
                queue_buffer_handle = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, queue_header_ptr->capacity, buffer_name.c_str());
                if (queue_buffer_handle == 0)
                {
                    unlock(true);
                    ITK_ABORT(true, "Error to create the buffer IPC queue.\n");
                }

                if (mode == (Platform::IPC::QueueIPC_READ | Platform::IPC::QueueIPC_WRITE))
                {
                    queue_buffer_ptr = (uint8_t *)MapViewOfFile(queue_buffer_handle, FILE_MAP_ALL_ACCESS, 0, 0, queue_header_ptr->capacity);
                    if (queue_buffer_ptr == 0)
                    {
                        unlock(true);
                        ITK_ABORT(true, "Error to map the IPC buffer.\n");
                    }
                }
                else if (mode == Platform::IPC::QueueIPC_READ)
                {
                    queue_buffer_ptr = (uint8_t *)MapViewOfFile(queue_buffer_handle, FILE_MAP_READ, 0, 0, queue_header_ptr->capacity);
                    if (queue_buffer_ptr == 0)
                    {
                        unlock(true);
                        ITK_ABORT(true, "Error to map the IPC buffer.\n");
                    }
                }
                else if (mode == Platform::IPC::QueueIPC_WRITE)
                {
                    queue_buffer_ptr = (uint8_t *)MapViewOfFile(queue_buffer_handle, FILE_MAP_WRITE, 0, 0, queue_header_ptr->capacity);
                    if (queue_buffer_ptr == 0)
                    {
                        unlock(true);
                        ITK_ABORT(true, "Error to map the IPC buffer.\n");
                    }
                }
                else
                {
                    unlock(true);
                    ITK_ABORT(true, "Queue opening mode not specified.\n");
                }
#elif defined(__linux__) || defined(__APPLE__)

                queue_buffer_handle = shm_open(buffer_name.c_str(),
                                               O_CREAT | O_RDWR,
                                               S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
                if (queue_buffer_handle == -1)
                {
                    queue_buffer_handle = BUFFER_HANDLE_NULL;

                    unlock();
                    unlock(true);
                    ITK_ABORT(true, "Error to create the buffer IPC queue. Error code: %s\n", strerror(errno));
                }

                // int rc;
                // struct stat _stat;
                rc = fstat(queue_buffer_handle, &_stat);
                ITK_ABORT(rc != 0, "Error to stat the file descriptor. Error code: %s\n", strerror(errno));
                if (_stat.st_size == 0)
                {
                    rc = ftruncate(queue_buffer_handle, queue_header_ptr->capacity);
                    // fallocate(queue_buffer_handle, 0, 0, queue_header_ptr->capacity);
                    ITK_ABORT(rc != 0, "Error to truncate buffer. Error code: %s\n", strerror(errno));
                }

                if (mode == (Platform::IPC::QueueIPC_READ | Platform::IPC::QueueIPC_WRITE))
                {
                    queue_buffer_ptr = (uint8_t *)mmap(
                        NULL,
                        queue_header_ptr->capacity,
                        PROT_READ | PROT_WRITE,
                        MAP_SHARED,
                        queue_buffer_handle,
                        0);
                    if (queue_buffer_ptr == MAP_FAILED)
                    {
                        unlock();
                        unlock(true);
                        ITK_ABORT(true, "Error to map the IPC buffer. Error code: %s\n", strerror(errno));
                    }
                }
                else if (mode == Platform::IPC::QueueIPC_READ)
                {
                    queue_buffer_ptr = (uint8_t *)mmap(
                        NULL,
                        queue_header_ptr->capacity,
                        PROT_READ,
                        MAP_SHARED,
                        queue_buffer_handle,
                        0);
                    if (queue_buffer_ptr == MAP_FAILED)
                    {
                        unlock();
                        unlock(true);
                        ITK_ABORT(true, "Error to map the IPC buffer. Error code: %s\n", strerror(errno));
                    }
                }
                else if (mode == Platform::IPC::QueueIPC_WRITE)
                {
                    queue_buffer_ptr = (uint8_t *)mmap(
                        NULL,
                        queue_header_ptr->capacity,
                        PROT_WRITE,
                        MAP_SHARED,
                        queue_buffer_handle,
                        0);
                    if (queue_buffer_ptr == MAP_FAILED)
                    {
                        unlock();
                        unlock(true);
                        ITK_ABORT(true, "Error to map the IPC buffer. Error code: %s\n", strerror(errno));
                    }
                }
                else
                {
                    unlock();
                    unlock(true);
                    ITK_ABORT(true, "Queue opening mode not specified.\n");
                }

#endif

                queue_header_ptr->subscribers_count++;

#if !defined(_WIN32)
                unlock(); // unlock the created semaphore
#endif
                unlock(true);

                //
                // Open Shared Header
                //
                /*
                if (mode == Platform::IPC::QueueIPC_WRITE) {

                    printf("[QueueIPC] Opening write\n");

                    queue_header_handle = CreateFileMappingA(
                        INVALID_HANDLE_VALUE, NULL,
                        PAGE_READWRITE, 0,
                        buffer_size,
                        header_name.c_str()
                    );
                    ITK_ABORT(queue_header_handle == 0, "Error to create the header IPC queue.\n");
                    queue_header_ptr = (QueueHeader*)MapViewOfFile(queue_header_handle, FILE_MAP_ALL_ACCESS, 0, 0, buffer_size);
                    ITK_ABORT(queue_header_ptr == 0, "Error to map the header IPC buffer.\n");


                    queue_buffer_handle = CreateFileMappingA(
                        INVALID_HANDLE_VALUE, NULL,
                        PAGE_READWRITE, 0,
                        buffer_size,
                        buffer_name.c_str()
                    );
                    ITK_ABORT(queue_buffer_handle == 0, "Error to create the buffer IPC queue.\n");
                    queue_buffer_ptr = (uint8_t*)MapViewOfFile(queue_buffer_handle, FILE_MAP_WRITE, 0, 0, buffer_size);
                    ITK_ABORT(queue_buffer_ptr == 0, "Error to map the IPC buffer.\n");

                } else if (mode == Platform::IPC::QueueIPC_READ) {

                    printf("[QueueIPC] Opening read\n");

                    queue_header_handle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, header_name.c_str());
                    ITK_ABORT(queue_header_handle == 0, "Error to create the header IPC queue.\n");
                    queue_header_ptr = (QueueHeader*)MapViewOfFile(queue_header_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
                    ITK_ABORT(queue_header_ptr == 0, "Error to map the header IPC buffer.\n");

                    queue_buffer_handle = OpenFileMappingA(FILE_MAP_READ, FALSE, buffer_name.c_str());
                    ITK_ABORT(queue_buffer_handle == 0, "Error to create the buffer IPC queue.\n");
                    queue_buffer_ptr = (uint8_t*)MapViewOfFile(queue_buffer_handle, FILE_MAP_READ, 0, 0, buffer_size);
                    ITK_ABORT(queue_buffer_ptr == 0, "Error to map the IPC buffer.\n");
                }
                */

                printStats();
            }

            bool writeHasEnoughSpace(uint32_t size, bool lock_if_true = false)
            {

                Platform::AutoLock autoLock(&shm_mutex);
                if (queue_semaphore == NULL)
                    return false;

                uint32_t size_request = size + sizeof(BufferHeader);

                ITK_ABORT(size_request > queue_header_ptr->capacity, "Buffer too big for this queue.\n");

                lock();

                uint32_t remaining_space = queue_header_ptr->capacity - queue_header_ptr->size;

                if (size_request <= remaining_space)
                {
                    if (!lock_if_true)
                        unlock();
                    return true;
                }
                else
                    unlock();

                return false;
            }

            bool writeHasEnoughSpace(const ObjectBuffer &inputBuffer, bool lock_if_true = false)
            {
                return writeHasEnoughSpace(inputBuffer.size, lock_if_true);
            }

            bool write(const uint8_t *data, uint32_t size, bool blocking = true, bool ignore_first_lock = false)
            {

                shm_mutex.lock();
                if (queue_semaphore == NULL)
                {
                    shm_mutex.unlock();
                    return false;
                }

                uint32_t size_request = size + sizeof(BufferHeader);

                ITK_ABORT(size_request > queue_header_ptr->capacity, "Buffer too big for this queue.\n");

                if (!ignore_first_lock)
                {

                    lock();
                    uint32_t remaining_space = queue_header_ptr->capacity - queue_header_ptr->size;
                    while (size_request > remaining_space)
                    {
                        unlock();
                        shm_mutex.unlock();

                        if (!blocking || Platform::Thread::isCurrentThreadInterrupted())
                            return false;

                        Platform::Sleep::millis(1);

                        shm_mutex.lock();
                        if (queue_semaphore == NULL)
                        {
                            shm_mutex.unlock();
                            return false;
                        }
                        lock();
                        remaining_space = queue_header_ptr->capacity - queue_header_ptr->size;
                    }
                }

                BufferHeader bufferHeader;
                bufferHeader.size = size;

                write_buffer((uint8_t *)&bufferHeader, sizeof(BufferHeader));
                write_buffer(data, size);

                // if (!ignore_first_lock)
                unlock();
                shm_mutex.unlock();

                return true;
            }

            bool write(const ObjectBuffer &inputBuffer, bool blocking = true, bool ignore_first_lock = false)
            {
                return write(inputBuffer.data, inputBuffer.size, blocking, ignore_first_lock);
            }

            bool readHasElement(bool lock_if_true = false)
            {
                Platform::AutoLock autoLock(&shm_mutex);
                if (queue_semaphore == NULL)
                    return false;

                lock();

                if (queue_header_ptr->size > 0)
                {
                    if (!lock_if_true)
                        unlock();
                    return true;
                }
                else
                    unlock();

                return false;
            }

            bool read(ObjectBuffer *outputBuffer, bool blocking = true, bool ignore_first_lock = false, bool *_signaled = NULL)
            {

                // Platform::AutoLock autoLock(&shm_mutex);
                shm_mutex.lock();
                if (queue_semaphore == NULL)
                {
                    shm_mutex.unlock();
                    if (_signaled != NULL)
                        *_signaled = false;
                    return false;
                }

                uint32_t total_chuncks = 0;

                if (!ignore_first_lock)
                {
                    lock();

                    while (queue_header_ptr->size == 0)
                    {
                        unlock();
                        shm_mutex.unlock();

                        if (!blocking){
                            if (_signaled != NULL)
                                *_signaled = false;
                            return false;
                        }

                        if (Platform::Thread::isCurrentThreadInterrupted()){
                            if (_signaled != NULL)
                                *_signaled = true;
                            return false;
                        }

                        Platform::Sleep::millis(1);

                        shm_mutex.lock();
                        if (queue_semaphore == NULL)
                        {
                            shm_mutex.unlock();
                            if (_signaled != NULL)
                                *_signaled = false;
                            return false;
                        }
                        lock();
                    }
                }

                BufferHeader bufferHeader;
                read_buffer((uint8_t *)&bufferHeader, sizeof(BufferHeader));
                outputBuffer->setSize(bufferHeader.size);
                read_buffer(outputBuffer->data, outputBuffer->size);

                // if (!ignore_first_lock)
                unlock();
                shm_mutex.unlock();

                if (_signaled != NULL)
                    *_signaled = false;

                return true;
            }

            virtual ~QueueIPC()
            {
                releaseAll();
            }

            void printStats()
            {
                printf("[QueueIPC] Queue Stats\n");

                printf("  semaphore name: %s\n", semaphore_name.c_str());
                printf("  header name: %s\n", header_name.c_str());
                printf("  buffer name: %s\n", buffer_name.c_str());

                printf("  subscribers_count:%u\n", queue_header_ptr->subscribers_count);

                printf("  write_pos:%u\n", queue_header_ptr->write_pos);
                printf("  read_pos:%u\n", queue_header_ptr->read_pos);

                printf("  queue_size:%u\n", queue_header_ptr->queue_size);
                printf("  buffer_size:%u\n", queue_header_ptr->buffer_size);

                printf("  capacity:%u\n", queue_header_ptr->capacity);
                printf("  size:%u\n", queue_header_ptr->size);
            }
        };
    }

}