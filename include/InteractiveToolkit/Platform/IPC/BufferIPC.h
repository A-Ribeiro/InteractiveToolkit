#pragma once

#include "../platform_common.h"
#include "../Core/ObjectBuffer.h"
#include "../Thread.h"
#include "../Sleep.h"
#include "../../ITKCommon/Path.h"
#include "../../ITKCommon/ITKAbort.h"
#include "../Signal.h"

#include "../../EventCore/Event.h"

namespace Platform
{

    // const uint32_t BufferIPC_READ = 1 << 0;
    // const uint32_t BufferIPC_WRITE = 1 << 1;

    namespace IPC
    {

        class BufferIPC : public EventCore::HandleCallback
        {

            std::string semaphore_name;
            std::string buffer_name;

            uint8_t *real_data_ptr;

            bool isFirst;

            void releaseAll()
            {
                int instance_count = -1;

                Platform::AutoLock autoLock(&shm_mutex);

                ITKCommon::ITKAbort::Instance()->OnAbort.remove(&BufferIPC::onAbort, this);

#if !defined(_WIN32)
                lock(true);
                bool is_last_buffer = false;

                if (buffer_handle != BUFFER_HANDLE_nullptr)
                {
                    uint32_t *count = (uint32_t *)&real_data_ptr[size];
                    is_last_buffer = ((*count) - 1) == 0;
                }
#endif

                lock();

                if (buffer_handle != BUFFER_HANDLE_nullptr)
                {

                    uint32_t *count = (uint32_t *)&real_data_ptr[size];
                    (*count)--;

                    instance_count = *count;

#if defined(_WIN32)
                    if (real_data_ptr != 0)
                        UnmapViewOfFile(real_data_ptr);
                    CloseHandle(buffer_handle);
#elif defined(__linux__) || defined(__APPLE__)
                    if (real_data_ptr != MAP_FAILED)
                        munmap(real_data_ptr, size + sizeof(uint32_t));
                    close(buffer_handle); // close FD
                                          // shm_unlink(buffer_name.c_str());
#endif
                    buffer_handle = BUFFER_HANDLE_nullptr;
                    real_data_ptr = nullptr;
                    data = nullptr;
                }

                unlock();

                if (buffer_semaphore != nullptr)
                {
#if defined(_WIN32)
                    CloseHandle(buffer_semaphore);
#elif defined(__linux__) || defined(__APPLE__)
                    sem_close(buffer_semaphore);
                    // sem_unlink(semaphore_name.c_str());
#endif
                    buffer_semaphore = nullptr;

#if !defined(_WIN32)
                    // unlink all resources
                    if (is_last_buffer)
                    {
                        printf("[linux] ... last buffer, unlink resources ...\n");
                        shm_unlink(buffer_name.c_str());
                        sem_unlink(semaphore_name.c_str());

                        // unlink the lock_f
                        std::string global_lock_file = ITKCommon::Path::getDocumentsPath("ITKLib", "lock") + ITKCommon::PATH_SEPARATOR + this->name + std::string(".b.f_lock");
                        unlink(global_lock_file.c_str());
                    }

                    unlock(true);
#endif
                }


                if (instance_count == 0)
                    OnLastBufferFree(this);

            }

            void onAbort(const char *file, int line, const char *message)
            {
                Platform::AutoLock autoLock(&shm_mutex);
                if (force_finish_initialization)
                    finishInitialization();
                releaseAll();
            }
            Platform::Mutex shm_mutex;
            bool force_finish_initialization;

            // private copy constructores, to avoid copy...
            BufferIPC(const BufferIPC &v) {}
            BufferIPC& operator=(const BufferIPC &v) {return *this;}

        public:

            std::string name;

            EventCore::Event<void(BufferIPC*)> OnLastBufferFree;
            
#if defined(__linux__) || defined(__APPLE__)
            // unlink all resources
            static void force_shm_unlink(const std::string &name)
            {
                printf("[BufferIPC] force_shm_unlink: %s\n", name.c_str());

                std::string buffer_name = std::string("/") + std::string(name) + std::string("_abd");    // aribeiro_buffer_data
                std::string semaphore_name = std::string("/") + std::string(name) + std::string("_abs"); // aribeiro_buffer_semaphore

                shm_unlink(buffer_name.c_str());
                sem_unlink(semaphore_name.c_str());

                // unlink the lock_f
                std::string global_lock_file = ITKCommon::Path::getDocumentsPath("ITKLib", "lock") + ITKCommon::PATH_SEPARATOR + name + std::string(".b.f_lock");
                unlink(global_lock_file.c_str());
            }
#endif

            void lock(bool from_constructor = false)
            {
                Platform::AutoLock autoLock(&shm_mutex);

#if defined(_WIN32)
                // lock semaphore
                ITK_ABORT(WaitForSingleObject(buffer_semaphore, INFINITE) != WAIT_OBJECT_0, "Error to lock queue semaphore. Error code: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#elif defined(__linux__) || defined(__APPLE__)
                if (from_constructor)
                {

                    ITK_ABORT(f_lock != -1, "Trying to lock twice from constructor.\n");

                    // file lock ... to solve the dead semaphore reinitialization...
                    std::string global_lock_file = ITKCommon::Path::getDocumentsPath("ITKLib", "lock") + ITKCommon::PATH_SEPARATOR + this->name + std::string(".b.f_lock");

                    f_lock = open(global_lock_file.c_str(), O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
                    ITK_ABORT(f_lock == -1, "Error to open f_lock. Error code: %s\n", strerror(errno));

                    int rc = lockf(f_lock, F_LOCK, 0);
                    ITK_ABORT(rc == -1, "Error to lock f_lock. Error code: %s\n", strerror(errno));
                }
                else
                {
                    // while semaphore is signaled, try to aquire until block...
                    if (buffer_semaphore != nullptr)
                        while (sem_wait(buffer_semaphore) != 0)
                            ;
                }
#endif
            }

            void unlock(bool from_constructor = false)
            {
#if defined(_WIN32)
                // release semaphore
                ITK_ABORT(!ReleaseSemaphore(buffer_semaphore, 1, nullptr), "Error to unlock queue semaphore. Error code: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
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
                    if (buffer_semaphore != nullptr)
                        sem_post(buffer_semaphore);
                }
#endif
            }

#if defined(_WIN32)
            HANDLE buffer_semaphore;
            HANDLE buffer_handle;

#elif defined(__linux__) || defined(__APPLE__)

            sem_t *buffer_semaphore; // semaphore
            int buffer_handle;       // FD

            int f_lock; // global file lock

#endif

            uint8_t *data;
            uint32_t size;

            BufferIPC(const char *name = "default",
                      // uint32_t mode = BufferIPC_READ | BufferIPC_WRITE,
                      uint32_t buffer_size_ = 1024)
            {

                // Platform::AutoLock autoLock(&shm_mutex);
                shm_mutex.lock();
                ITKCommon::ITKAbort::Instance()->OnAbort.add(&BufferIPC::onAbort, this);

                force_finish_initialization = true;

                size = buffer_size_;

                buffer_semaphore = nullptr;
                buffer_handle = BUFFER_HANDLE_nullptr;

                this->name = name;

#if defined(_WIN32)
                buffer_name = std::string(name) + std::string("_abd");    // aribeiro_buffer_data
                semaphore_name = std::string(name) + std::string("_abs"); // aribeiro_buffer_semaphore
#elif defined(__linux__) || defined(__APPLE__)
                buffer_name = std::string("/") + std::string(name) + std::string("_abd");    // aribeiro_buffer_data
                semaphore_name = std::string("/") + std::string(name) + std::string("_abs"); // aribeiro_buffer_semaphore

                f_lock = -1;
#endif

                // printf("semaphore_name: %s\n", semaphore_name.c_str());
                // printf("buffer_name: %s\n", buffer_name.c_str());

#if defined(_WIN32)
                SECURITY_DESCRIPTOR sd;
                InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
                SetSecurityDescriptorDacl(&sd, TRUE, (PACL)0, FALSE);

                SECURITY_ATTRIBUTES sa;
                sa.nLength = sizeof(sa);
                sa.lpSecurityDescriptor = &sd;
                sa.bInheritHandle = FALSE;

                buffer_semaphore = CreateSemaphoreA(
                    &sa,                   // default security attributes
                    1,                     // initial count
                    LONG_MAX,              // maximum count
                    semaphore_name.c_str() // named semaphore
                );
                ITK_ABORT(buffer_semaphore == 0, "Error to create global semaphore. Error code: %s\n", ITKPlatformUtil::win32_GetLastErrorToString().c_str());
#elif defined(__linux__) || defined(__APPLE__)
/*
        buffer_semaphore = sem_open(
            semaphore_name.c_str(),
            O_CREAT,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
            1
        );

        ITK_ABORT(buffer_semaphore == SEM_FAILED, "Error to create global semaphore. Error code: %s\n", strerror(errno) );
*/
#endif

                lock(true);

#if defined(_WIN32)
                // open the buffer memory section
                buffer_handle = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, size + sizeof(uint32_t), buffer_name.c_str());
                if (buffer_handle == 0)
                {
                    unlock(true);
                    ITK_ABORT(true, "Error to create the buffer IPC queue.\n");
                }

                // if (mode == ( BufferIPC_READ | BufferIPC_WRITE) ) {
                real_data_ptr = (uint8_t *)MapViewOfFile(buffer_handle, FILE_MAP_ALL_ACCESS, 0, 0, size + sizeof(uint32_t));
                if (real_data_ptr == 0)
                {
                    unlock(true);
                    ITK_ABORT(true, "Error to map the IPC buffer.\n");
                }
                /*} else if (mode == BufferIPC_READ) {
                    real_data_ptr = (uint8_t*)MapViewOfFile(buffer_handle, FILE_MAP_READ, 0, 0, size + sizeof(uint32_t));
                    if (real_data_ptr == 0) {
                        unlock(true);
                        ITK_ABORT(true, "Error to map the IPC buffer.\n");
                    }
                } else if (mode == BufferIPC_WRITE) {
                    real_data_ptr = (uint8_t*)MapViewOfFile(buffer_handle, FILE_MAP_WRITE, 0, 0, size + sizeof(uint32_t));
                    if (real_data_ptr == 0) {
                        unlock(true);
                        ITK_ABORT(true, "Error to map the IPC buffer.\n");
                    }
                } else {
                    unlock(true);
                    ITK_ABORT(true, "Buffer opening mode not specified.\n");
                }*/

#elif defined(__linux__) || defined(__APPLE__)

                buffer_handle = shm_open(buffer_name.c_str(),
                                         O_CREAT | O_RDWR,
                                         S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
                if (buffer_handle == -1)
                {
                    buffer_handle = BUFFER_HANDLE_nullptr;
                    unlock(true);
                    ITK_ABORT(true, "Error to create the buffer IPC queue. Error code: %s\n", strerror(errno));
                }

                int rc;

                struct stat _stat;
                rc = fstat(buffer_handle, &_stat);
                ITK_ABORT(rc != 0, "Error to stat the file descriptor. Error code: %s\n", strerror(errno));
                if (_stat.st_size == 0)
                {
                    rc = ftruncate(buffer_handle, size + sizeof(uint32_t));
                    // fallocate(buffer_handle, 0, 0, size + sizeof(uint32_t));
                    ITK_ABORT(rc != 0, "Error to truncate buffer. Error code: %s\n", strerror(errno));

                    // truncate semaphore and lock it...
                    sem_unlink(semaphore_name.c_str());
                }

                // just initialize the semaphore
                buffer_semaphore = sem_open(
                    semaphore_name.c_str(),
                    O_CREAT,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH,
                    1);

                if (buffer_semaphore == SEM_FAILED)
                {
                    buffer_semaphore = nullptr;
                    ITK_ABORT(true, "Error to create global semaphore. Error code: %s\n", strerror(errno));
                }

                lock(); // lock the created semaphore

                // if (mode == (BufferIPC_READ | BufferIPC_WRITE)) {
                real_data_ptr = (uint8_t *)mmap(
                    nullptr,
                    size + sizeof(uint32_t),
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED,
                    buffer_handle,
                    0);
                if (real_data_ptr == MAP_FAILED)
                {
                    unlock();
                    unlock(true);
                    ITK_ABORT(true, "Error to map the IPC buffer. Error code: %s\n", strerror(errno));
                }
                /*}
                else if (mode == BufferIPC_READ) {
                    real_data_ptr = (uint8_t*)mmap(
                        nullptr,
                        size + sizeof(uint32_t),
                        PROT_READ,
                        MAP_SHARED,
                        buffer_handle,
                        0
                    );
                    if (real_data_ptr == MAP_FAILED) {
                        unlock();
                        unlock(true);
                        ITK_ABORT(true, "Error to map the IPC buffer. Error code: %s\n", strerror(errno) );
                    }
                }
                else if (mode == BufferIPC_WRITE) {
                    real_data_ptr = (uint8_t*)mmap(
                        nullptr,
                        size + sizeof(uint32_t),
                        PROT_WRITE,
                        MAP_SHARED,
                        buffer_handle,
                        0
                    );
                    if (real_data_ptr == MAP_FAILED) {
                        unlock();
                        unlock(true);
                        ITK_ABORT(true, "Error to map the IPC buffer. Error code: %s\n", strerror(errno) );
                    }
                }
                else {
                    unlock();
                    unlock(true);
                    ITK_ABORT(true, "Buffer opening mode not specified.\n");
                }*/

#endif

                uint32_t *count = (uint32_t *)&real_data_ptr[size];
                (*count)++;

                isFirst = (*count) == 1;

                data = &real_data_ptr[0];

                // printf("Initialization OK.\n");

                // unlock();
            }

            bool isFirstProcess()
            {
                return isFirst;
            }
            void finishInitialization()
            {
// ITK_ABORT(!isFirst, "Calling finishInitialization on a non-first memory initialization process.\n");
#if !defined(_WIN32)
                unlock();
#endif

                unlock(true);

                force_finish_initialization = false;
                shm_mutex.unlock();
            }

            ~BufferIPC()
            {
                releaseAll();
            }
        };

    }
}
