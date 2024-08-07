#pragma once

#include "QueueIPC.h"
#include "SemaphoreIPC.h"
#include "ConditionIPC.h"

namespace Platform
{

    namespace IPC
    {

        class LowLatencyQueueIPC : public EventCore::HandleCallback
        {

            bool blocking_on_read;

            std::string name;

            std::string header_name;
            std::string buffer_name;
            std::string semaphore_name;       // mutex
            std::string semaphore_count_name; // real semaphore

            void lock(bool from_constructor = false)
            {
                Platform::AutoLock autoLock(&shm_mutex);

#if defined(_WIN32)
                // lock semaphore
                if (queue_semaphore != nullptr)
                    ITK_ABORT(WaitForSingleObject(queue_semaphore, INFINITE) != WAIT_OBJECT_0, "Error to lock queue semaphore. Error code: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#elif defined(__linux__) || defined(__APPLE__)

                if (from_constructor)
                {

                    ITK_ABORT(f_lock != -1, "Trying to lock twice from constructor.\n");

                    // file lock ... to solve the dead semaphore reinitialization...
                    std::string global_lock_file = ITKCommon::Path::getDocumentsPath("ITKLib", "lock") + ITKCommon::PATH_SEPARATOR + this->name + std::string(".llq.f_lock");

                    f_lock = open(global_lock_file.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
                    ITK_ABORT(f_lock == -1, "Error to open f_lock. Error code: %s\n", strerror(errno));

                    int rc = lockf(f_lock, F_LOCK, 0);
                    ITK_ABORT(rc == -1, "Error to lock f_lock. Error code: %s\n", strerror(errno));
                }
                else
                {
                    // while semaphore is signaled, try to aquire until block...
                    if (queue_semaphore != nullptr)
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
                if (queue_semaphore != nullptr)
                    ITK_ABORT(!ReleaseSemaphore(queue_semaphore, 1, nullptr), "Error to unlock queue semaphore. Error code: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
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
                    if (queue_semaphore != nullptr)
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

                // printf("\n    Queue write:%u total:%u\n", size,queue_header_ptr->size );
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

                // printf("\n    Queue reader:%u total:%u\n", size,queue_header_ptr->size );
            }

            void releaseAll(bool release_semaphore_ipc)
            {
                Platform::AutoLock autoLock(&shm_mutex);

                ITKCommon::ITKAbort::Instance()->OnAbort.remove(&LowLatencyQueueIPC::onAbort, this);

#if !defined(_WIN32)
                lock(true);
                bool is_last_queue = false;
                std::string sem_count_name = std::string("/") + semaphore_count_name;

                if (queue_header_handle != BUFFER_HANDLE_nullptr)
                {
                    is_last_queue = (queue_header_ptr->subscribers_count - 1) == 0;
                }
#endif

                lock();

                if (queue_buffer_handle != BUFFER_HANDLE_nullptr)
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
                    queue_buffer_handle = BUFFER_HANDLE_nullptr;
                }

                if (queue_header_handle != BUFFER_HANDLE_nullptr)
                {

                    queue_header_ptr->subscribers_count--;

#if defined(_WIN32)
                    if (queue_header_ptr != 0)
                        UnmapViewOfFile(queue_header_ptr);
                    CloseHandle(queue_header_handle);
#elif defined(__linux__) || defined(__APPLE__)
                    if (queue_header_ptr != MAP_FAILED)
                        munmap(queue_header_ptr, sizeof(Platform::IPC::QueueHeader));
                    close(queue_header_handle); // close FD
                                                // shm_unlink(header_name.c_str());
#endif
                    queue_header_handle = BUFFER_HANDLE_nullptr;
                }

                if (release_semaphore_ipc)
                {
                    if (semaphore_ipc != nullptr)
                    {

#if !defined(_WIN32)
                        sem_count_name = semaphore_ipc->name;
#endif

                        delete semaphore_ipc;
                        semaphore_ipc = nullptr;
                    }
                }
#if defined(_WIN32)
                else if (semaphore_ipc != nullptr)
                {
                    // force close handle...
                    semaphore_ipc->forceCloseWindows();
                }
#endif

                if (can_write_cond != nullptr)
                {
                    delete can_write_cond;
                    can_write_cond = nullptr;
                }
                if (can_write_cond_mutex != nullptr)
                {
                    delete can_write_cond_mutex;
                    can_write_cond_mutex = nullptr;
                }

                unlock();

                if (queue_semaphore != nullptr)
                {
#if defined(_WIN32)
                    CloseHandle(queue_semaphore);
#elif defined(__linux__) || defined(__APPLE__)
                    sem_close(queue_semaphore);
                    // sem_unlink(semaphore_name.c_str());
#endif

                    queue_semaphore = nullptr;
                }

#if !defined(_WIN32)
                // unlink all resources
                if (is_last_queue)
                {
                    printf("[linux] ... last low latency queue, unlink resources ...\n");
                    shm_unlink(buffer_name.c_str());
                    shm_unlink(header_name.c_str());
                    sem_unlink(semaphore_name.c_str());
                    sem_unlink(sem_count_name.c_str());

                    // unlink the lock_f
                    std::string global_lock_file = ITKCommon::Path::getDocumentsPath("ITKLib", "lock") + ITKCommon::PATH_SEPARATOR + this->name + std::string(".llq.f_lock");
                    unlink(global_lock_file.c_str());
                }

                unlock(true);
#endif
            }

            void onAbort(const char *file, int line, const char *message)
            {
                releaseAll(false);
            }

            Platform::Mutex shm_mutex;

        public:
#if defined(__linux__) || defined(__APPLE__)
            // unlink all resources
            static void force_shm_unlink(const std::string &name)
            {
                printf("[LowLatencyQueueIPC] force_shm_unlink: %s\n", name.c_str());

                std::string header_name = std::string("/") + std::string(name) + std::string("_allqh");    // aribeiro_ll_queue_header
                std::string buffer_name = std::string("/") + std::string(name) + std::string("_allqb");    // aribeiro_ll_queue_buffer
                std::string semaphore_name = std::string("/") + std::string(name) + std::string("_allqs"); // aribeiro_ll_queue_semaphore
                std::string semaphore_count_name = std::string(name) + std::string("_allqsc");             // aribeiro_ll_queue_semaphore_count
                std::string sem_count_name = std::string("/") + semaphore_count_name;

                shm_unlink(buffer_name.c_str());
                shm_unlink(header_name.c_str());
                sem_unlink(semaphore_name.c_str());
                sem_unlink(sem_count_name.c_str());

                // unlink the lock_f
                std::string global_lock_file = ITKCommon::Path::getDocumentsPath("ITKLib", "lock") + ITKCommon::PATH_SEPARATOR + name + std::string(".llq.f_lock");
                unlink(global_lock_file.c_str());
            }
#endif

            // unblocked if the containing threads have been interrupted...
            SemaphoreIPC *semaphore_ipc;

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

            // write condition
            ConditionIPC *can_write_cond;
            SemaphoreIPC *can_write_cond_mutex;

            LowLatencyQueueIPC(const char *name = "default",
                               uint32_t mode = QueueIPC_READ | QueueIPC_WRITE,
                               uint32_t queue_size_ = 64,
                               uint32_t buffer_size_ = 1024,
                               bool blocking_on_read_ = true,
                               bool use_write_contition_variable = true)
            {
                Platform::AutoLock autoLock(&shm_mutex);

                can_write_cond = nullptr;
                can_write_cond_mutex = nullptr;

                if (use_write_contition_variable)
                {
                    can_write_cond = new ConditionIPC(name);
                    can_write_cond_mutex = new SemaphoreIPC(std::string(name) + std::string("_cv_wc"), 1, can_write_cond->bufferIPC()->isFirstProcess());
#if defined(__linux__) || defined(__APPLE__)
                    can_write_cond->bufferIPC()->OnLastBufferFree.add([&](Platform::IPC::BufferIPC *)
                                                                      {
                                                                          Platform::AutoLock autoLock(&shm_mutex);
                                                                          if (can_write_cond_mutex != nullptr)
                                                                          {
                                                                              delete can_write_cond_mutex;
                                                                              can_write_cond_mutex = nullptr;
                                                                          }
                                                                          // SemaphoreIPC::force_shm_unlink(this->name + std::string("_cv_wc"));
                                                                      });
#endif
                }

                ITKCommon::ITKAbort::Instance()->OnAbort.add(&LowLatencyQueueIPC::onAbort, this);

                semaphore_ipc = nullptr;

                this->blocking_on_read = blocking_on_read_;

                queue_semaphore = nullptr;
                queue_header_handle = BUFFER_HANDLE_nullptr;
                queue_buffer_handle = BUFFER_HANDLE_nullptr;

                this->name = name;

#if defined(_WIN32)
                header_name = std::string(name) + std::string("_allqh");    // aribeiro_ll_queue_header
                buffer_name = std::string(name) + std::string("_allqb");    // aribeiro_ll_queue_buffer
                semaphore_name = std::string(name) + std::string("_allqs"); // aribeiro_ll_queue_semaphore
#elif defined(__linux__) || defined(__APPLE__)
                header_name = std::string("/") + std::string(name) + std::string("_allqh");    // aribeiro_ll_queue_header
                buffer_name = std::string("/") + std::string(name) + std::string("_allqb");    // aribeiro_ll_queue_buffer
                semaphore_name = std::string("/") + std::string(name) + std::string("_allqs"); // aribeiro_ll_queue_semaphore

                f_lock = -1;
#endif

                semaphore_count_name = std::string(name) + std::string("_allqsc"); // aribeiro_ll_queue_semaphore_count

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
                queue_header_handle = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(Platform::IPC::QueueHeader), header_name.c_str());
                if (queue_header_handle == 0)
                {
                    unlock(true);
                    ITK_ABORT(true, "Error to create the header IPC queue.\n");
                }
                queue_header_ptr = (Platform::IPC::QueueHeader *)MapViewOfFile(queue_header_handle, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Platform::IPC::QueueHeader));
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
                    queue_header_handle = BUFFER_HANDLE_nullptr;
                    unlock(true);
                    ITK_ABORT(true, "Error to create the header IPC queue. Error code: %s\n", strerror(errno));
                }

                int rc;

                struct stat _stat;
                rc = fstat(queue_header_handle, &_stat);
                ITK_ABORT(rc != 0, "Error to stat the file descriptor. Error code: %s\n", strerror(errno));
                if (_stat.st_size == 0)
                {
                    rc = ftruncate(queue_header_handle, sizeof(Platform::IPC::QueueHeader));
                    // fallocate(queue_header_handle, 0, 0, sizeof(Platform::IPC::QueueHeader));
                    ITK_ABORT(rc != 0, "Error to truncate buffer. Error code: %s\n", strerror(errno));
                }

                queue_header_ptr = (Platform::IPC::QueueHeader *)mmap(
                    nullptr,
                    sizeof(Platform::IPC::QueueHeader),
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

                    printf("[LowLatencyQueueIPC] Not First Opened - Retrieving Shared Memory Information...\n");
                    semaphore_ipc = new SemaphoreIPC(semaphore_count_name, 0, false, false);

#if !defined(_WIN32)
                    // initialize semaphore
                    queue_semaphore = sem_open(
                        semaphore_name.c_str(),
                        O_CREAT,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
                        1);

                    if (queue_semaphore == SEM_FAILED)
                    {
                        queue_semaphore = nullptr;
                        unlock(true);
                        ITK_ABORT(true, "Error to create global semaphore. Error code: %s\n", strerror(errno));
                    }

                    lock(); // lock the created semaphore
#endif
                }
                else
                {
                    printf("[LowLatencyQueueIPC] First Opened - Creating Shared Memory...\n");

                    semaphore_ipc = new SemaphoreIPC(semaphore_count_name, 0, true, false);

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
                        queue_semaphore = nullptr;
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
                queue_buffer_handle = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, queue_header_ptr->capacity, buffer_name.c_str());
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
                    queue_buffer_handle = BUFFER_HANDLE_nullptr;

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
                        nullptr,
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
                        nullptr,
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
                        nullptr,
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

                    printf("[LowLatencyQueueIPC] Opening write\n");

                    queue_header_handle = CreateFileMappingA(
                        INVALID_HANDLE_VALUE, nullptr,
                        PAGE_READWRITE, 0,
                        buffer_size,
                        header_name.c_str()
                    );
                    ITK_ABORT(queue_header_handle == 0, "Error to create the header IPC queue.\n");
                    queue_header_ptr = (Platform::IPC::QueueHeader*)MapViewOfFile(queue_header_handle, FILE_MAP_ALL_ACCESS, 0, 0, buffer_size);
                    ITK_ABORT(queue_header_ptr == 0, "Error to map the header IPC buffer.\n");


                    queue_buffer_handle = CreateFileMappingA(
                        INVALID_HANDLE_VALUE, nullptr,
                        PAGE_READWRITE, 0,
                        buffer_size,
                        buffer_name.c_str()
                    );
                    ITK_ABORT(queue_buffer_handle == 0, "Error to create the buffer IPC queue.\n");
                    queue_buffer_ptr = (uint8_t*)MapViewOfFile(queue_buffer_handle, FILE_MAP_WRITE, 0, 0, buffer_size);
                    ITK_ABORT(queue_buffer_ptr == 0, "Error to map the IPC buffer.\n");

                } else if (mode == Platform::IPC::QueueIPC_READ) {

                    printf("[LowLatencyQueueIPC] Opening read\n");

                    queue_header_handle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, header_name.c_str());
                    ITK_ABORT(queue_header_handle == 0, "Error to create the header IPC queue.\n");
                    queue_header_ptr = (Platform::IPC::QueueHeader*)MapViewOfFile(queue_header_handle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
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
                if (queue_semaphore == nullptr)
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
                return writeHasEnoughSpace((uint32_t)inputBuffer.size, lock_if_true);
            }

            bool write(const uint8_t *data, uint32_t size, bool blocking = true, bool ignore_first_lock = false)
            {

                shm_mutex.lock();
                if (queue_semaphore == nullptr)
                {
                    shm_mutex.unlock();
                    return false;
                }

                uint32_t size_request = size + sizeof(BufferHeader);

                ITK_ABORT(size_request > queue_header_ptr->capacity, "Buffer too big for this queue.\n");

                if (!ignore_first_lock)
                {
                    if (can_write_cond_mutex != nullptr)
                    {
                        //printf("trying to lock mutex cond\n");
                        AutoLockSemaphoreIPC ipc_lock(can_write_cond_mutex);
                        if (ipc_lock.signaled) {
                            shm_mutex.unlock();
                            return false;
                        }

                        // use condition
                        lock();
                        uint32_t remaining_space = queue_header_ptr->capacity - queue_header_ptr->size;
                        while (size_request > remaining_space)
                        {
                            unlock();
                            shm_mutex.unlock();

                            if (!blocking)
                                return false;

                            //printf("wait cond\n");
                            bool signaled = false;
                            can_write_cond->wait(can_write_cond_mutex, &signaled);
                            if (signaled) {
                                ipc_lock.cancelAutoRelease();
                                return false;
                            }

                            shm_mutex.lock();
                            if (queue_semaphore == nullptr)
                            {
                                shm_mutex.unlock();
                                return false;
                            }
                            lock();
                            remaining_space = queue_header_ptr->capacity - queue_header_ptr->size;
                        }
                    }
                    else
                    {

                        lock();
                        uint32_t remaining_space = queue_header_ptr->capacity - queue_header_ptr->size;
                        while (size_request > remaining_space)
                        {
                            unlock();
                            shm_mutex.unlock();

                            if (!blocking || Platform::Thread::isCurrentThreadInterrupted())
                                return false;

                            // maybe the yield give us a better result...but increases the CPU usage
                            // Platform::Sleep::millis(1);
                            Platform::Sleep::yield();

                            shm_mutex.lock();
                            if (queue_semaphore == nullptr)
                            {
                                shm_mutex.unlock();
                                return false;
                            }
                            lock();
                            remaining_space = queue_header_ptr->capacity - queue_header_ptr->size;
                        }
                    }
                }

                BufferHeader bufferHeader;
                bufferHeader.size = size;

                write_buffer((uint8_t *)&bufferHeader, sizeof(BufferHeader));
                write_buffer(data, size);

                // if (!ignore_first_lock)
                unlock();
                shm_mutex.unlock();

                if (blocking_on_read)
                    semaphore_ipc->release();

                return true;
            }

            bool write(const ObjectBuffer &inputBuffer, bool blocking = true, bool ignore_first_lock = false)
            {
                return write(inputBuffer.data, (uint32_t)inputBuffer.size, blocking, ignore_first_lock);
            }

            bool read(ObjectBuffer *outputBuffer, bool *_signaled = nullptr)
            {

                if (blocking_on_read)
                {
                    bool signaled = !semaphore_ipc->blockingAcquire();
                    if (signaled){
                        if (_signaled != nullptr)
                            *_signaled = signaled;
                        return false;
                    }
                }

                shm_mutex.lock();
                if (queue_semaphore == nullptr)
                {
                    if (blocking_on_read)
                        semaphore_ipc->release();
                    shm_mutex.unlock();
                    if (_signaled != nullptr)
                        *_signaled = false;
                    return false;
                }

                lock();

                if (queue_header_ptr->size == 0)
                {
                    unlock();
                    shm_mutex.unlock();

                    if (_signaled != nullptr)
                        *_signaled = false;

                    // printf("ERROR: Trying to read element from an empty queue.\n");
                    return false;
                }

                BufferHeader bufferHeader;
                read_buffer((uint8_t *)&bufferHeader, sizeof(BufferHeader));
                outputBuffer->setSize(bufferHeader.size);
                read_buffer(outputBuffer->data, (uint32_t)outputBuffer->size);

                unlock();
                shm_mutex.unlock();


                if (can_write_cond_mutex != nullptr)
                {
                    //printf("trying to lock mutex cond\n");
                    AutoLockSemaphoreIPC ipc_lock(can_write_cond_mutex);
                    if (!ipc_lock.signaled) {
                        // aquired critical area...
                    }
                    //printf("cond->notify_all\n");
                    can_write_cond->notify_all();
                }

                if (_signaled != nullptr)
                    *_signaled = false;
                return true;
            }

            ~LowLatencyQueueIPC()
            {
                releaseAll(true);
            }

            void printStats()
            {
                printf("[LowLatencyQueueIPC] Queue Stats\n");

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

            // only check if this queue is signaled for the current thread...
            // it may be active in another thread...
            // bool isSignaled()
            // {
            //     Platform::AutoLock autoLock(&shm_mutex);
            //     if (queue_semaphore == nullptr)
            //         return true;
            //     return Platform::Thread::isCurrentThreadInterrupted();
            //     // return semaphore_ipc->isSignaled();
            // }
        };

    }

}