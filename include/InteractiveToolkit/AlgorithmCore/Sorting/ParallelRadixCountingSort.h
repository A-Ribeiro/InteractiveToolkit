#pragma once

#include "RadixCountingSort.h"
#include "../../Platform/ThreadPool.h"
#include "../../Platform/Core/ObjectBuffer.h"

namespace AlgorithmCore
{

    namespace Sorting
    {
        template <typename _type>
        class ParallelRadixCountingSort
        {
            template <typename T>
            struct TaskSort
            {
                T *data;
                T *tmp_buffer;
                size_t count;
                Platform::Semaphore *completion_semaphore;
            };

            template <typename T>
            struct TaskMerge
            {

                T *in;
                T *out;
                int64_t i;
                int64_t element_count;
                size_t count;
                Platform::Semaphore *completion_semaphore;
            };

        public:

            static void sort(_type *data,
                             const size_t &count,
                             Platform::ThreadPool *threadpool,
                             int64_t thread_count = -1)
            {
                if (thread_count == -1)
                {
                    thread_count = 1;
                    while ((thread_count << 1) <= threadpool->threadCount())
                        thread_count <<= 1;
                    // printf("Dividing sorting array into %i blocks\n", thread_count);
                }

                Platform::ObjectBuffer buffer;
                buffer.setSize(sizeof(_type) * count);
                _type *aux = ((_type *)buffer.data);

                int64_t job_thread_size = (int64_t)(count / thread_count);
                if (job_thread_size == 0)
                    job_thread_size = 1;

                // sort blocks
                Platform::Semaphore completion_semaphore(0);
                {
                    TaskSort<_type> task;
                    for (int64_t i = 0; i < count; i += job_thread_size)
                    {
                        int64_t index_start = i;
                        int64_t index_end_exclusive = i + job_thread_size;
                        if (index_end_exclusive > count)
                            index_end_exclusive = count;

                        task.data = data + index_start;
                        task.tmp_buffer = aux + index_start;
                        task.count = index_end_exclusive - index_start;
                        task.completion_semaphore = &completion_semaphore;

                        threadpool->postTask([task]()
                                             {
                                            AlgorithmCore::Sorting::RadixCountingSort<_type>::sort(task.data, (uint32_t)task.count, task.tmp_buffer);
                                            task.completion_semaphore->release(); });
                    }

                    // barrier -- finish all sorting
                    for (int64_t i = 0; i < count; i += job_thread_size)
                        completion_semaphore.blockingAcquire();
                }

                // merge down the blocks
                {
                    _type *in = data;
                    _type *out = aux;

                    int64_t element_count = job_thread_size;
                    TaskMerge<_type> task;
                    while (element_count < count)
                    {
                        // merge operation
                        for (int64_t i = 0; i < count; i += (element_count << 1))
                        {
                            task.in = in;
                            task.out = out;
                            task.i = i;
                            task.element_count = element_count;
                            task.count = count;
                            task.completion_semaphore = &completion_semaphore;

                            threadpool->postTask([task]()
                                                 {
                                                     int64_t write_index = task.i;
                                                     int64_t write_max = task.i + (task.element_count << 1);
                                                     if (write_max > (int64_t)task.count)
                                                         write_max = (int64_t)task.count;

                                                     int64_t a_index = task.i;
                                                     int64_t b_index = task.i + task.element_count;

                                                     int64_t a_max = b_index;
                                                     int64_t b_max = b_index + task.element_count;

                                                     if (a_max > (int64_t)task.count)
                                                         a_max = (int64_t)task.count;
                                                     if (b_max > (int64_t)task.count)
                                                         b_max = (int64_t)task.count;

                                                     while (write_index < write_max &&
                                                            a_index < a_max &&
                                                            b_index < b_max)
                                                     {

                                                         const auto &_a = task.in[a_index];
                                                         const auto &_b = task.in[b_index];

                                                         if (_a > _b)
                                                         {
                                                             task.out[write_index] = _b;
                                                             b_index++;
                                                         }
                                                         else
                                                         {
                                                             task.out[write_index] = _a;
                                                             a_index++;
                                                         }

                                                         write_index++;
                                                     }

                                                     while (a_index < a_max)
                                                         task.out[write_index++] = task.in[a_index++];
                                                     while (b_index < b_max)
                                                         task.out[write_index++] = task.in[b_index++];
                                                     task.completion_semaphore->release(); //
                                                 });
                        }

                        // barrier - wait all jobs done
                        for (int64_t i = 0; i < count; i += (element_count << 1))
                            completion_semaphore.blockingAcquire();

                        // swap in/out
                        auto aux = in;
                        in = out;
                        out = aux;

                        element_count = element_count << 1;
                    }

                    if (in != data)
                    {
                        // printf("Slow Sorting Warning: copy all data at end\n");
                        memcpy(data, in, sizeof(_type) * count);
                    }
                }
            }

            static void sortIndex(AlgorithmCore::Sorting::SortIndex<_type> *data,
                             const size_t &count,
                             Platform::ThreadPool *threadpool,
                             int thread_count = -1)
            {
                if (thread_count == -1)
                {
                    thread_count = 1;
                    while ((thread_count << 1) <= threadpool->threadCount())
                    {
                        thread_count <<= 1;
                    }
                    // printf("Dividing sorting array into %i blocks\n", thread_count);
                }

                Platform::ObjectBuffer buffer;
                buffer.setSize(sizeof(AlgorithmCore::Sorting::SortIndex<_type>) * (uint32_t)count);
                AlgorithmCore::Sorting::SortIndex<_type> *aux = ((AlgorithmCore::Sorting::SortIndex<_type> *)buffer.data);

                int64_t job_thread_size = (int64_t) (count / thread_count);
                if (job_thread_size == 0)
                    job_thread_size = 1;

                // sort blocks
                Platform::Semaphore completion_semaphore(0);
                {
                    TaskSort<AlgorithmCore::Sorting::SortIndex<_type>> task;
                    for (int64_t i = 0; i < count; i += job_thread_size)
                    {
                        int64_t index_start = i;
                        int64_t index_end_exclusive = i + job_thread_size;
                        if (index_end_exclusive > count)
                            index_end_exclusive = count;

                        task.data = data + index_start;
                        task.tmp_buffer = aux + index_start;
                        task.count = index_end_exclusive - index_start;
                        task.completion_semaphore = &completion_semaphore;

                        threadpool->postTask([task]()
                                             {
                                            AlgorithmCore::Sorting::RadixCountingSort<_type>::sortIndex(task.data, (uint32_t)task.count, task.tmp_buffer);
                                            task.completion_semaphore->release(); });
                    }

                    // barrier -- finish all sorting
                    for (int64_t i = 0; i < count; i += job_thread_size)
                        completion_semaphore.blockingAcquire();
                }

                // merge down the blocks
                {
                    AlgorithmCore::Sorting::SortIndex<_type> *in = data;
                    AlgorithmCore::Sorting::SortIndex<_type> *out = aux;

                    int64_t element_count = job_thread_size;

                    TaskMerge<AlgorithmCore::Sorting::SortIndex<_type>> task;
                    while (element_count < count)
                    {
                        // merge operation
                        for (int64_t i = 0; i < count; i += (element_count << 1))
                        {
                            
                            task.in = in;
                            task.out = out;
                            task.i = i;
                            task.element_count = element_count;
                            task.count = count;
                            task.completion_semaphore = &completion_semaphore;

                            threadpool->postTask([task]()
                                                 {
                                                     int64_t write_index = task.i;
                                                     int64_t write_max = task.i + (task.element_count << 1);
                                                     if (write_max > (int64_t)task.count)
                                                         write_max = (int64_t)task.count;

                                                     int64_t a_index = task.i;
                                                     int64_t b_index = task.i + task.element_count;

                                                     int64_t a_max = b_index;
                                                     int64_t b_max = b_index + task.element_count;

                                                     if (a_max > (int64_t)task.count)
                                                         a_max = (int64_t)task.count;
                                                     if (b_max > (int64_t)task.count)
                                                         b_max = (int64_t)task.count;

                                                     while (write_index < write_max &&
                                                            a_index < a_max &&
                                                            b_index < b_max)
                                                     {

                                                         const auto &_a = task.in[a_index];
                                                         const auto &_b = task.in[b_index];

                                                         if (_a.toSort > _b.toSort)
                                                         {
                                                             task.out[write_index] = _b;
                                                             b_index++;
                                                         }
                                                         else
                                                         {
                                                             task.out[write_index] = _a;
                                                             a_index++;
                                                         }

                                                         write_index++;
                                                     }

                                                     while (a_index < a_max)
                                                         task.out[write_index++] = task.in[a_index++];
                                                     while (b_index < b_max)
                                                         task.out[write_index++] = task.in[b_index++];
                                                     task.completion_semaphore->release(); //
                                                 });
                        }

                        // barrier - wait all jobs done
                        for (int64_t i = 0; i < count; i += (element_count << 1))
                            completion_semaphore.blockingAcquire();

                        // swap in/out
                        auto aux = in;
                        in = out;
                        out = aux;

                        element_count = element_count << 1;
                    }

                    if (in != data)
                    {
                        // printf("Slow Sorting Warning: copy all data at end\n");
                        memcpy(data, in, sizeof(AlgorithmCore::Sorting::SortIndex<_type>) * count);
                    }
                }
            }
        };


        using ParallelRadixCountingSortu32 = ParallelRadixCountingSort<uint32_t>;
        using ParallelRadixCountingSorti32 = ParallelRadixCountingSort<int32_t>;

        using ParallelRadixCountingSortu64 = ParallelRadixCountingSort<uint64_t>;
        using ParallelRadixCountingSorti64 = ParallelRadixCountingSort<int64_t>;
    }
}