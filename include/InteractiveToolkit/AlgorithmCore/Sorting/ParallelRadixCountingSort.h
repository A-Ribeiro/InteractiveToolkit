#pragma once

#include "RadixCountingSort.h"
#include "../../Platform/ThreadPool.h"
#include "../../Platform/Core/ObjectBuffer.h"

#define ParallelRadixCountingSort_SinglePass 1
#define ParallelRadixCountingSort_ReductionPass 2

#define ParallelRadixCountingSort_mode ParallelRadixCountingSort_SinglePass

namespace AlgorithmCore
{

    namespace Sorting
    {
        template <typename _type>
        class ParallelRadixCountingSort
        {
            // template <typename T>
            // struct TaskSort
            // {
            //     T *data;
            //     T *tmp_buffer;
            //     size_t count;
            //     Platform::Semaphore *completion_semaphore;
            // };

            // template <typename T>
            // struct TaskMerge
            // {

            //     T *in;
            //     T *out;
            //     uint64_t i;
            //     uint64_t element_count;
            //     size_t count;
            //     Platform::Semaphore *completion_semaphore;
            // };

            // Check block max size limit
            template <typename _type_internal, typename std::enable_if<std::is_same<_type_internal, uint64_t>::value, bool>::type = true>
            static inline uint64_t read_sort_uint_value(_type_internal item_count)
            {
                return item_count;
            }
            template <typename _type_internal, typename std::enable_if<std::is_same<_type_internal, int64_t>::value, bool>::type = true>
            static inline uint64_t read_sort_uint_value(_type_internal item_count)
            {
                return (uint64_t)item_count ^ UINT64_C(0x8000000000000000);
            }
            template <typename _type_internal, typename std::enable_if<std::is_same<_type_internal, uint32_t>::value, bool>::type = true>
            static inline uint32_t read_sort_uint_value(_type_internal item_count)
            {
                return item_count;
            }
            template <typename _type_internal, typename std::enable_if<std::is_same<_type_internal, int32_t>::value, bool>::type = true>
            static inline uint32_t read_sort_uint_value(_type_internal item_count)
            {
                return (uint32_t)item_count ^ UINT32_C(0x80000000);
            }

        public:
            static void sort(_type *data,
                             const size_t &count,
                             Platform::ThreadPool *threadpool,
                             _type *tmp_array = nullptr,
                             Platform::Semaphore *completion_semaphore_ = nullptr,
                             int thread_count = -1,
                             uint64_t per_task_max_loop_count = 16 * 1024,
                             uint64_t min_blocks_to_paralelize = 8)
            {
                if (thread_count == -1)
                    thread_count = threadpool->threadCount() * 4;

                uint64_t virt_threads = (count + (uint64_t)thread_count - 1) / (uint64_t)thread_count;

                uint64_t min_thread_count = per_task_max_loop_count;
                virt_threads = (virt_threads > min_thread_count) ? virt_threads : min_thread_count;

                uint64_t virt_blocks = (count + virt_threads - 1) / virt_threads;

                // printf("virt_threads: %zu virt_blocks: %zu\n", virt_threads, virt_blocks);

                if (virt_blocks <= min_blocks_to_paralelize)
                {
                    // printf("ParallelRadixCountingSort: Not enough blocks to parallelize, using single thread instead\n");
                    RadixCountingSort<_type>::sort(data, (uint32_t)count, tmp_array);
                    return;
                }

                uint64_t virt_threads_256 = min_thread_count / virt_blocks;
                if (virt_threads_256 > 256)
                    virt_threads_256 = 256;
                else if (virt_threads_256 == 0)
                    virt_threads_256 = 1;
                // distribute the 256 threads among the CPU Threads
                // uint64_t virt_threads_256 = (256 + (uint64_t)thread_count - 1) / (uint64_t)thread_count;
                uint64_t virt_blocks_256 = (256 + virt_threads_256 - 1) / virt_threads_256;

                // printf("virt_threads_256: %zu virt_blocks_256: %zu\n", virt_threads_256, virt_blocks_256);

                Platform::Semaphore *completion_semaphore = completion_semaphore_;
                if (completion_semaphore_ == nullptr)
                    completion_semaphore = new Platform::Semaphore(0);

                Platform::ObjectBuffer buffer;

                _type *data_in = data;
                _type *data_out = tmp_array;

                if (tmp_array == nullptr)
                {
                    buffer.setSize(sizeof(_type) * (uint32_t)count);
                    data_out = ((_type *)buffer.data);
                }

                std::vector<uint32_t> histogram_per_block_out_vec(virt_blocks * 256);
                uint32_t *histogram_per_block_out = histogram_per_block_out_vec.data();

                uint32_t s_hist_total[256];

                uint64_t element_count = (uint64_t)count;

                uint32_t *histogram_to_offset = histogram_per_block_out;
                uint64_t histogram_to_offset_block_count = virt_blocks;

#if ParallelRadixCountingSort_mode == ParallelRadixCountingSort_ReductionPass
                uint64_t reduced_block_count = threadpool->threadCount() * 2;
                reduced_block_count = (virt_blocks < reduced_block_count) ? virt_blocks : reduced_block_count;

                std::vector<uint32_t> histogram_reduced_per_proc_vec(reduced_block_count * 256);
                uint32_t *histogram_reduced_per_proc = histogram_reduced_per_proc_vec.data();

                uint64_t reduce_group_size = (virt_blocks + reduced_block_count - 1) / reduced_block_count;

                // map blocks from histogram_per_block_out into blocks of histogram_reduced_per_proc
                histogram_to_offset = histogram_reduced_per_proc;
                histogram_to_offset_block_count = reduced_block_count;

                // printf("reduced_block_count: %zu of %zu blocks\n", reduced_block_count, virt_blocks);
#endif

                for (int digit_part = 0; digit_part < sizeof(_type); digit_part++)
                {
                    // printf("[digit %d] Start\n", digit_part);

                    memset(s_hist_total, 0, sizeof(uint32_t) * 256);
                    memset(histogram_per_block_out, 0, sizeof(uint32_t) * virt_blocks * 256);

                    // histogram on each block
                    // printf("    [histogram]\n");

                    for (uint64_t curr_block = 0; curr_block < virt_blocks; curr_block++)
                        threadpool->postTask(
                            [&completion_semaphore, curr_block, digit_part, data_in, &histogram_per_block_out, element_count, virt_threads]()
                            {
                                uint64_t data_index_start = curr_block * virt_threads;
                                uint64_t histogram_index_start = curr_block * 256;

                                uint64_t thread_count_end = data_index_start + virt_threads;
                                if (thread_count_end > element_count)
                                    thread_count_end = element_count;
                                if (thread_count_end <= data_index_start)
                                {
                                    completion_semaphore->release();
                                    return;
                                }
                                thread_count_end -= data_index_start;

                                // printf("        - Processing thread range: %llu - %llu\n", data_index_start + 0, data_index_start + thread_count_end);

                                int shift = digit_part * 8;
                                for (uint64_t thread_id = 0; thread_id < thread_count_end; thread_id++)
                                {
                                    uint64_t data_index = data_index_start + thread_id;
                                    const auto &item = data_in[data_index];
                                    auto data = read_sort_uint_value<_type>(item);
                                    int digit = (int)((data >> shift) & 0xff);
                                    histogram_per_block_out[histogram_index_start + (uint64_t)digit]++;
                                }
                                completion_semaphore->release();
                            });
                    // barrier - histogram
                    for (uint64_t curr_block = 0; curr_block < virt_blocks; curr_block++)
                        completion_semaphore->blockingAcquire();

#if ParallelRadixCountingSort_mode == ParallelRadixCountingSort_ReductionPass
                    // map blocks from histogram_per_block_out into blocks of histogram_reduced_per_proc

                    for (uint64_t curr_block = 0; curr_block < reduced_block_count; curr_block++)
                    {
                        for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                        {
                            threadpool->postTask(
                                [&completion_semaphore, curr_block, curr_block_256, &histogram_reduced_per_proc, reduced_block_count, histogram_per_block_out, virt_blocks, virt_threads_256, reduce_group_size]()
                                {
                                    uint64_t thread_id_start = curr_block_256 * virt_threads_256;
                                    uint64_t thread_id_end = thread_id_start + virt_threads_256;
                                    if (thread_id_end > 256)
                                        thread_id_end = 256;
                                    // printf("        - Processing thread range: %llu - %llu\n", thread_id_start, thread_id_end);
                                    for (uint64_t thread_id = thread_id_start; thread_id < thread_id_end; thread_id++)
                                    {
                                        uint64_t bucket = thread_id;
                                        uint32_t s_hist_bucket = 0;

                                        uint64_t block_id_start = curr_block * reduce_group_size;
                                        uint64_t block_id_end = block_id_start + reduce_group_size;
                                        block_id_end = (block_id_end >= virt_blocks) ? virt_blocks : block_id_end;

                                        for (uint64_t block_id = block_id_start; block_id < block_id_end; block_id++)
                                        {
                                            uint64_t index = block_id * 256 + bucket;
                                            s_hist_bucket += histogram_per_block_out[index];
                                        }

                                        histogram_reduced_per_proc[curr_block * 256 + bucket] = s_hist_bucket;
                                    }
                                    completion_semaphore->release();
                                });
                        }
                    }
                    // barrier - histogram
                    for (uint64_t curr_block = 0; curr_block < reduced_block_count; curr_block++)
                        for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                            completion_semaphore->blockingAcquire();

#endif

                    // prefix sum on global histogram - transforming it into offsets
                    // printf("    [PrefixSum]\n");
                    for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                    {
                        threadpool->postTask(
                            [&completion_semaphore, curr_block_256, histogram_to_offset, &s_hist_total, virt_threads_256, histogram_to_offset_block_count]()
                            {
                                uint64_t thread_id_start = curr_block_256 * virt_threads_256;
                                uint64_t thread_id_end = thread_id_start + virt_threads_256;
                                if (thread_id_end > 256)
                                    thread_id_end = 256;
                                // printf("        - Processing thread range: %llu - %llu\n", thread_id_start, thread_id_end);
                                for (uint64_t thread_id = thread_id_start; thread_id < thread_id_end; thread_id++)
                                {
                                    for (uint64_t block_id = 0; block_id < histogram_to_offset_block_count; block_id++)
                                        s_hist_total[thread_id] += histogram_to_offset[block_id * 256 + thread_id];
                                }
                                completion_semaphore->release();
                            });
                    }
                    // barrier - prefix sum
                    for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                        completion_semaphore->blockingAcquire();
                    uint32_t sum = 0;
                    for (uint32_t i = 0; i < 256; i++)
                    {
                        uint32_t temp = s_hist_total[i];
                        s_hist_total[i] = sum;
                        sum += temp;
                    }
                    // offsets on each block
                    // printf("    [Offset]\n");
                    for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                    {
                        threadpool->postTask(
                            [&completion_semaphore, curr_block_256, &histogram_to_offset, &s_hist_total, virt_threads_256, histogram_to_offset_block_count]()
                            {
                                uint64_t thread_id_start = curr_block_256 * virt_threads_256;
                                uint64_t thread_id_end = thread_id_start + virt_threads_256;
                                if (thread_id_end > 256)
                                    thread_id_end = 256;
                                // printf("        - Processing thread range: %llu - %llu\n", thread_id_start, thread_id_end);
                                for (uint64_t thread_id = thread_id_start; thread_id < thread_id_end; thread_id++)
                                {
                                    for (uint64_t block_id = 0; block_id < histogram_to_offset_block_count; block_id++)
                                    {
                                        uint64_t index = block_id * 256 + thread_id;
                                        uint32_t hist_count = histogram_to_offset[index];
                                        histogram_to_offset[index] = s_hist_total[thread_id];
                                        s_hist_total[thread_id] += hist_count;
                                    }
                                }
                                completion_semaphore->release();
                            });
                    }
                    // barrier - offset
                    for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                        completion_semaphore->blockingAcquire();

#if ParallelRadixCountingSort_mode == ParallelRadixCountingSort_ReductionPass
                    // reverse map blocks from histogram_per_block_out from blocks of histogram_reduced_per_proc

                    for (uint64_t curr_block = 0; curr_block < reduced_block_count; curr_block++)
                    {
                        for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                        {
                            threadpool->postTask(
                                [&completion_semaphore, curr_block, curr_block_256, histogram_reduced_per_proc, reduced_block_count, &histogram_per_block_out, virt_blocks, virt_threads_256, reduce_group_size]()
                                {
                                    uint64_t thread_id_start = curr_block_256 * virt_threads_256;
                                    uint64_t thread_id_end = thread_id_start + virt_threads_256;
                                    if (thread_id_end > 256)
                                        thread_id_end = 256;
                                    // printf("        - Processing thread range: %llu - %llu\n", thread_id_start, thread_id_end);
                                    for (uint64_t thread_id = thread_id_start; thread_id < thread_id_end; thread_id++)
                                    {
                                        uint64_t bucket = thread_id;
                                        uint32_t s_offset_bucket = histogram_reduced_per_proc[curr_block * 256 + bucket];

                                        uint64_t block_id_start = curr_block * reduce_group_size;
                                        uint64_t block_id_end = block_id_start + reduce_group_size;
                                        block_id_end = (block_id_end >= virt_blocks) ? virt_blocks : block_id_end;

                                        for (uint64_t block_id = block_id_start; block_id < block_id_end; block_id++)
                                        {
                                            uint64_t index = block_id * 256 + bucket;
                                            uint32_t hist_count = histogram_per_block_out[index];
                                            histogram_per_block_out[index] = s_offset_bucket;
                                            s_offset_bucket += hist_count;
                                        }
                                    }
                                    completion_semaphore->release();
                                });
                        }
                    }
                    // barrier - histogram
                    for (uint64_t curr_block = 0; curr_block < reduced_block_count; curr_block++)
                        for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                            completion_semaphore->blockingAcquire();

#endif

                    // scatter
                    // printf("    [Scatter]\n");
                    for (uint64_t curr_block = 0; curr_block < virt_blocks; curr_block++)
                        threadpool->postTask(
                            [&completion_semaphore, curr_block, digit_part, data_in, &data_out, &histogram_per_block_out, element_count, virt_threads]()
                            {
                                uint32_t *block_digit_offset = histogram_per_block_out + curr_block * 256;
                                uint64_t data_index_start = curr_block * virt_threads;

                                uint64_t thread_count_end = data_index_start + virt_threads;
                                if (thread_count_end > element_count)
                                    thread_count_end = element_count;
                                if (thread_count_end <= data_index_start)
                                {
                                    completion_semaphore->release();
                                    return;
                                }
                                thread_count_end -= data_index_start;

                                int shift = digit_part * 8;
                                for (uint64_t thread_id = 0; thread_id < thread_count_end; thread_id++)
                                {
                                    uint64_t data_index = data_index_start + thread_id;
                                    const auto &item = data_in[data_index];
                                    auto data = read_sort_uint_value<_type>(item);
                                    int digit = (int)((data >> shift) & 0xff);
                                    uint32_t dest_index = block_digit_offset[digit]++;
                                    data_out[dest_index] = item;
                                }
                                completion_semaphore->release();
                            });
                    // barrier - scatter
                    for (uint64_t curr_block = 0; curr_block < virt_blocks; curr_block++)
                        completion_semaphore->blockingAcquire();

                    // swap in/out
                    std::swap(data_in, data_out);
                }

                if (data_in != data)
                {
                    // printf("Sorting Not Returning Correctly Warning: copy all data at end\n");
                    memcpy(data, data_in, sizeof(AlgorithmCore::Sorting::SortIndex<_type>) * count);
                }

                if (completion_semaphore_ == nullptr)
                    delete completion_semaphore;

                // if (thread_count == -1)
                // {
                //     thread_count = 1;
                //     while ((thread_count << 1) <= threadpool->threadCount())
                //         thread_count <<= 1;
                //     // printf("Dividing sorting array into %i blocks\n", thread_count);
                // }

                // Platform::ObjectBuffer buffer;
                // buffer.setSize(sizeof(_type) * count);
                // _type *aux = ((_type *)buffer.data);

                // uint64_t job_thread_size = (uint64_t)(count / thread_count);
                // if (job_thread_size == 0)
                //     job_thread_size = 1;

                // // sort blocks
                // Platform::Semaphore completion_semaphore(0);
                // {
                //     TaskSort<_type> task;
                //     for (uint64_t i = 0; i < count; i += job_thread_size)
                //     {
                //         uint64_t index_start = i;
                //         uint64_t index_end_exclusive = i + job_thread_size;
                //         if (index_end_exclusive > count)
                //             index_end_exclusive = count;

                //         task.data = data + index_start;
                //         task.tmp_buffer = aux + index_start;
                //         task.count = index_end_exclusive - index_start;
                //         task.completion_semaphore = &completion_semaphore;

                //         threadpool->postTask([task]()
                //                              {
                //                             AlgorithmCore::Sorting::RadixCountingSort<_type>::sort(task.data, (uint32_t)task.count, task.tmp_buffer);
                //                             task.completion_semaphore->release(); });
                //     }

                //     // barrier -- finish all sorting
                //     for (uint64_t i = 0; i < count; i += job_thread_size)
                //         completion_semaphore.blockingAcquire();
                // }

                // // merge down the blocks
                // {
                //     _type *in = data;
                //     _type *out = aux;

                //     uint64_t element_count = job_thread_size;
                //     TaskMerge<_type> task;
                //     while (element_count < count)
                //     {
                //         // merge operation
                //         for (uint64_t i = 0; i < count; i += (element_count << 1))
                //         {
                //             task.in = in;
                //             task.out = out;
                //             task.i = i;
                //             task.element_count = element_count;
                //             task.count = count;
                //             task.completion_semaphore = &completion_semaphore;

                //             threadpool->postTask(
                //                 [task]()
                //                 {
                //                     uint64_t write_index = task.i;
                //                     uint64_t write_max = task.i + (task.element_count << 1);
                //                     if (write_max > (uint64_t)task.count)
                //                         write_max = (uint64_t)task.count;

                //                     uint64_t a_index = task.i;
                //                     uint64_t b_index = task.i + task.element_count;

                //                     uint64_t a_max = b_index;
                //                     uint64_t b_max = b_index + task.element_count;

                //                     if (a_max > (uint64_t)task.count)
                //                         a_max = (uint64_t)task.count;
                //                     if (b_max > (uint64_t)task.count)
                //                         b_max = (uint64_t)task.count;

                //                     while (write_index < write_max &&
                //                            a_index < a_max &&
                //                            b_index < b_max)
                //                     {

                //                         const auto &_a = task.in[a_index];
                //                         const auto &_b = task.in[b_index];

                //                         if (_a > _b)
                //                         {
                //                             task.out[write_index] = _b;
                //                             b_index++;
                //                         }
                //                         else
                //                         {
                //                             task.out[write_index] = _a;
                //                             a_index++;
                //                         }

                //                         write_index++;
                //                     }

                //                     while (a_index < a_max)
                //                         task.out[write_index++] = task.in[a_index++];
                //                     while (b_index < b_max)
                //                         task.out[write_index++] = task.in[b_index++];
                //                     task.completion_semaphore->release(); //
                //                 });
                //         }

                //         // barrier - wait all jobs done
                //         for (uint64_t i = 0; i < count; i += (element_count << 1))
                //             completion_semaphore.blockingAcquire();

                //         // swap in/out
                //         auto aux = in;
                //         in = out;
                //         out = aux;

                //         element_count = element_count << 1;
                //     }

                //     if (in != data)
                //     {
                //         // printf("Slow Sorting Warning: copy all data at end\n");
                //         memcpy(data, in, sizeof(_type) * count);
                //     }
                // }
            }

            static void sortIndex(AlgorithmCore::Sorting::SortIndex<_type> *data,
                                  const size_t &count,
                                  Platform::ThreadPool *threadpool,
                                  AlgorithmCore::Sorting::SortIndex<_type> *tmp_array = nullptr,
                                  Platform::Semaphore *completion_semaphore_ = nullptr,
                                  int thread_count = -1,
                                  uint64_t per_task_max_loop_count = 16 * 1024,
                                  uint64_t min_blocks_to_paralelize = 8)
            {
                if (thread_count == -1)
                    thread_count = threadpool->threadCount() * 4;

                // number of threads per block (max... considering the total processor count)
                uint64_t virt_threads = (count + (uint64_t)thread_count - 1) / (uint64_t)thread_count;

                uint64_t min_thread_count = per_task_max_loop_count;
                virt_threads = (virt_threads > min_thread_count) ? virt_threads : min_thread_count;

                uint64_t virt_blocks = (count + virt_threads - 1) / virt_threads;

                // printf("virt_threads: %zu virt_blocks: %zu\n", virt_threads, virt_blocks);

                if (virt_blocks <= min_blocks_to_paralelize)
                {
                    // printf("ParallelRadixCountingSort: Not enough blocks to parallelize, using single thread instead\n");
                    RadixCountingSort<_type>::sortIndex(data, (uint32_t)count, tmp_array);
                    return;
                }

                uint64_t virt_threads_256 = min_thread_count / virt_blocks;
                if (virt_threads_256 > 256)
                    virt_threads_256 = 256;
                else if (virt_threads_256 == 0)
                    virt_threads_256 = 1;
                // distribute the 256 threads among the CPU Threads
                // uint64_t virt_threads_256 = (256 + (uint64_t)thread_count - 1) / (uint64_t)thread_count;
                uint64_t virt_blocks_256 = (256 + virt_threads_256 - 1) / virt_threads_256;

                // printf("virt_threads_256: %zu virt_blocks_256: %zu\n", virt_threads_256, virt_blocks_256);

                Platform::Semaphore *completion_semaphore = completion_semaphore_;
                if (completion_semaphore_ == nullptr)
                    completion_semaphore = new Platform::Semaphore(0);

                Platform::ObjectBuffer buffer;

                AlgorithmCore::Sorting::SortIndex<_type> *data_in = data;
                AlgorithmCore::Sorting::SortIndex<_type> *data_out = tmp_array;

                if (tmp_array == nullptr)
                {
                    buffer.setSize(sizeof(AlgorithmCore::Sorting::SortIndex<_type>) * (uint32_t)count);
                    data_out = ((AlgorithmCore::Sorting::SortIndex<_type> *)buffer.data);
                }

                std::vector<uint32_t> histogram_per_block_out_vec(virt_blocks * 256);
                uint32_t *histogram_per_block_out = histogram_per_block_out_vec.data();

                uint32_t s_hist_total[256];

                uint64_t element_count = (uint64_t)count;

                uint32_t *histogram_to_offset = histogram_per_block_out;
                uint64_t histogram_to_offset_block_count = virt_blocks;

#if ParallelRadixCountingSort_mode == ParallelRadixCountingSort_ReductionPass
                uint64_t reduced_block_count = threadpool->threadCount() * 2;
                reduced_block_count = (virt_blocks < reduced_block_count) ? virt_blocks : reduced_block_count;

                std::vector<uint32_t> histogram_reduced_per_proc_vec(reduced_block_count * 256);
                uint32_t *histogram_reduced_per_proc = histogram_reduced_per_proc_vec.data();

                uint64_t reduce_group_size = (virt_blocks + reduced_block_count - 1) / reduced_block_count;

                // map blocks from histogram_per_block_out into blocks of histogram_reduced_per_proc
                histogram_to_offset = histogram_reduced_per_proc;
                histogram_to_offset_block_count = reduced_block_count;

                // printf("reduced_block_count: %zu of %zu blocks\n", reduced_block_count, virt_blocks);
#endif

                for (int digit_part = 0; digit_part < sizeof(_type); digit_part++)
                {
                    // printf("[digit %d] Start\n", digit_part);

                    memset(s_hist_total, 0, sizeof(uint32_t) * 256);
                    memset(histogram_per_block_out, 0, sizeof(uint32_t) * virt_blocks * 256);

                    // histogram on each block
                    // printf("    [histogram]\n");

                    for (uint64_t curr_block = 0; curr_block < virt_blocks; curr_block++)
                        threadpool->postTask(
                            [&completion_semaphore, curr_block, digit_part, data_in, &histogram_per_block_out, element_count, virt_threads]()
                            {
                                uint64_t data_index_start = curr_block * virt_threads;
                                uint64_t histogram_index_start = curr_block * 256;

                                uint64_t thread_count_end = data_index_start + virt_threads;
                                if (thread_count_end > element_count)
                                    thread_count_end = element_count;
                                if (thread_count_end <= data_index_start)
                                {
                                    completion_semaphore->release();
                                    return;
                                }
                                thread_count_end -= data_index_start;

                                // printf("        - Processing thread range: %llu - %llu\n", data_index_start + 0, data_index_start + thread_count_end);
                                int shift = digit_part * 8;
                                for (uint64_t thread_id = 0; thread_id < thread_count_end; thread_id++)
                                {
                                    uint64_t data_index = data_index_start + thread_id;
                                    const auto &item = data_in[data_index];
                                    auto data = read_sort_uint_value<_type>(item.toSort);
                                    int digit = (int)((data >> shift) & 0xff);
                                    histogram_per_block_out[histogram_index_start + (uint64_t)digit]++;
                                }
                                completion_semaphore->release();
                            });
                    // barrier - histogram
                    for (uint64_t curr_block = 0; curr_block < virt_blocks; curr_block++)
                        completion_semaphore->blockingAcquire();

#if ParallelRadixCountingSort_mode == ParallelRadixCountingSort_ReductionPass
                    // map blocks from histogram_per_block_out into blocks of histogram_reduced_per_proc

                    for (uint64_t curr_block = 0; curr_block < reduced_block_count; curr_block++)
                    {
                        for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                        {
                            threadpool->postTask(
                                [&completion_semaphore, curr_block, curr_block_256, &histogram_reduced_per_proc, reduced_block_count, histogram_per_block_out, virt_blocks, virt_threads_256, reduce_group_size]()
                                {
                                    uint64_t thread_id_start = curr_block_256 * virt_threads_256;
                                    uint64_t thread_id_end = thread_id_start + virt_threads_256;
                                    if (thread_id_end > 256)
                                        thread_id_end = 256;
                                    // printf("        - Processing thread range: %llu - %llu\n", thread_id_start, thread_id_end);
                                    for (uint64_t thread_id = thread_id_start; thread_id < thread_id_end; thread_id++)
                                    {
                                        uint64_t bucket = thread_id;
                                        uint32_t s_hist_bucket = 0;

                                        uint64_t block_id_start = curr_block * reduce_group_size;
                                        uint64_t block_id_end = block_id_start + reduce_group_size;
                                        block_id_end = (block_id_end >= virt_blocks) ? virt_blocks : block_id_end;

                                        for (uint64_t block_id = block_id_start; block_id < block_id_end; block_id++)
                                        {
                                            uint64_t index = block_id * 256 + bucket;
                                            s_hist_bucket += histogram_per_block_out[index];
                                        }

                                        histogram_reduced_per_proc[curr_block * 256 + bucket] = s_hist_bucket;
                                    }
                                    completion_semaphore->release();
                                });
                        }
                    }
                    // barrier - histogram
                    for (uint64_t curr_block = 0; curr_block < reduced_block_count; curr_block++)
                        for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                            completion_semaphore->blockingAcquire();

#endif

                    // prefix sum on global histogram - transforming it into offsets
                    // printf("    [PrefixSum]\n");
                    for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                    {
                        threadpool->postTask(
                            [&completion_semaphore, curr_block_256, histogram_to_offset, &s_hist_total, virt_threads_256, histogram_to_offset_block_count]()
                            {
                                uint64_t thread_id_start = curr_block_256 * virt_threads_256;
                                uint64_t thread_id_end = thread_id_start + virt_threads_256;
                                if (thread_id_end > 256)
                                    thread_id_end = 256;
                                // printf("        - Processing thread range: %llu - %llu\n", thread_id_start, thread_id_end);
                                for (uint64_t thread_id = thread_id_start; thread_id < thread_id_end; thread_id++)
                                {
                                    for (uint64_t block_id = 0; block_id < histogram_to_offset_block_count; block_id++)
                                        s_hist_total[thread_id] += histogram_to_offset[block_id * 256 + thread_id];
                                }
                                completion_semaphore->release();
                            });
                    }
                    // barrier - prefix sum
                    for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                        completion_semaphore->blockingAcquire();

                    uint32_t sum = 0;
                    for (uint32_t i = 0; i < 256; i++)
                    {
                        uint32_t temp = s_hist_total[i];
                        s_hist_total[i] = sum;
                        sum += temp;
                    }
                    // offsets on each block
                    // printf("    [Offset]\n");
                    for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                    {
                        threadpool->postTask(
                            [&completion_semaphore, curr_block_256, &histogram_to_offset, &s_hist_total, virt_threads_256, histogram_to_offset_block_count]()
                            {
                                uint64_t thread_id_start = curr_block_256 * virt_threads_256;
                                uint64_t thread_id_end = thread_id_start + virt_threads_256;
                                if (thread_id_end > 256)
                                    thread_id_end = 256;
                                // printf("        - Processing thread range: %llu - %llu\n", thread_id_start, thread_id_end);
                                for (uint64_t thread_id = thread_id_start; thread_id < thread_id_end; thread_id++)
                                {
                                    for (uint64_t block_id = 0; block_id < histogram_to_offset_block_count; block_id++)
                                    {
                                        uint64_t index = block_id * 256 + thread_id;
                                        uint32_t hist_count = histogram_to_offset[index];
                                        histogram_to_offset[index] = s_hist_total[thread_id];
                                        s_hist_total[thread_id] += hist_count;
                                    }
                                }
                                completion_semaphore->release();
                            });
                    }
                    // barrier - offset
                    for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                        completion_semaphore->blockingAcquire();

#if ParallelRadixCountingSort_mode == ParallelRadixCountingSort_ReductionPass
                    // reverse map blocks from histogram_per_block_out from blocks of histogram_reduced_per_proc

                    for (uint64_t curr_block = 0; curr_block < reduced_block_count; curr_block++)
                    {
                        for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                        {
                            threadpool->postTask(
                                [&completion_semaphore, curr_block, curr_block_256, histogram_reduced_per_proc, reduced_block_count, &histogram_per_block_out, virt_blocks, virt_threads_256, reduce_group_size]()
                                {
                                    uint64_t thread_id_start = curr_block_256 * virt_threads_256;
                                    uint64_t thread_id_end = thread_id_start + virt_threads_256;
                                    if (thread_id_end > 256)
                                        thread_id_end = 256;
                                    // printf("        - Processing thread range: %llu - %llu\n", thread_id_start, thread_id_end);
                                    for (uint64_t thread_id = thread_id_start; thread_id < thread_id_end; thread_id++)
                                    {
                                        uint64_t bucket = thread_id;
                                        uint32_t s_offset_bucket = histogram_reduced_per_proc[curr_block * 256 + bucket];

                                        uint64_t block_id_start = curr_block * reduce_group_size;
                                        uint64_t block_id_end = block_id_start + reduce_group_size;
                                        block_id_end = (block_id_end >= virt_blocks) ? virt_blocks : block_id_end;

                                        for (uint64_t block_id = block_id_start; block_id < block_id_end; block_id++)
                                        {
                                            uint64_t index = block_id * 256 + bucket;
                                            uint32_t hist_count = histogram_per_block_out[index];
                                            histogram_per_block_out[index] = s_offset_bucket;
                                            s_offset_bucket += hist_count;
                                        }
                                    }
                                    completion_semaphore->release();
                                });
                        }
                    }
                    // barrier - histogram
                    for (uint64_t curr_block = 0; curr_block < reduced_block_count; curr_block++)
                        for (uint64_t curr_block_256 = 0; curr_block_256 < virt_blocks_256; curr_block_256++)
                            completion_semaphore->blockingAcquire();

#endif

                    // scatter
                    // printf("    [Scatter]\n");
                    for (uint64_t curr_block = 0; curr_block < virt_blocks; curr_block++)
                        threadpool->postTask(
                            [&completion_semaphore, curr_block, digit_part, data_in, &data_out, &histogram_per_block_out, element_count, virt_threads]()
                            {
                                uint32_t *block_digit_offset = histogram_per_block_out + curr_block * 256;
                                uint64_t data_index_start = curr_block * virt_threads;

                                uint64_t thread_count_end = data_index_start + virt_threads;
                                if (thread_count_end > element_count)
                                    thread_count_end = element_count;
                                if (thread_count_end <= data_index_start)
                                {
                                    completion_semaphore->release();
                                    return;
                                }
                                thread_count_end -= data_index_start;

                                int shift = digit_part * 8;
                                for (uint64_t thread_id = 0; thread_id < thread_count_end; thread_id++)
                                {
                                    uint64_t data_index = data_index_start + thread_id;
                                    const auto &item = data_in[data_index];
                                    auto data = read_sort_uint_value<_type>(item.toSort);
                                    int digit = (int)((data >> shift) & 0xff);
                                    uint32_t dest_index = block_digit_offset[digit]++;
                                    data_out[dest_index] = item;
                                }
                                completion_semaphore->release();
                            });
                    // barrier - scatter
                    for (uint64_t curr_block = 0; curr_block < virt_blocks; curr_block++)
                        completion_semaphore->blockingAcquire();

                    // swap in/out
                    std::swap(data_in, data_out);
                }

                if (data_in != data)
                {
                    // printf("Sorting Not Returning Correctly Warning: copy all data at end\n");
                    memcpy(data, data_in, sizeof(AlgorithmCore::Sorting::SortIndex<_type>) * count);
                }

                if (completion_semaphore_ == nullptr)
                    delete completion_semaphore;

                // if (thread_count == -1)
                // {
                //     thread_count = 1;
                //     while ((thread_count << 1) <= threadpool->threadCount())
                //     {
                //         thread_count <<= 1;
                //     }
                //     // printf("Dividing sorting array into %i blocks\n", thread_count);
                // }

                // Platform::ObjectBuffer buffer;
                // buffer.setSize(sizeof(AlgorithmCore::Sorting::SortIndex<_type>) * (uint32_t)count);
                // AlgorithmCore::Sorting::SortIndex<_type> *aux = ((AlgorithmCore::Sorting::SortIndex<_type> *)buffer.data);

                // uint64_t job_thread_size = (uint64_t) (count / thread_count);
                // if (job_thread_size == 0)
                //     job_thread_size = 1;

                // // sort blocks
                // Platform::Semaphore completion_semaphore(0);
                // {
                //     TaskSort<AlgorithmCore::Sorting::SortIndex<_type>> task;
                //     for (uint64_t i = 0; i < count; i += job_thread_size)
                //     {
                //         uint64_t index_start = i;
                //         uint64_t index_end_exclusive = i + job_thread_size;
                //         if (index_end_exclusive > count)
                //             index_end_exclusive = count;

                //         task.data = data + index_start;
                //         task.tmp_buffer = aux + index_start;
                //         task.count = index_end_exclusive - index_start;
                //         task.completion_semaphore = &completion_semaphore;

                //         threadpool->postTask([task]()
                //                              {
                //                             AlgorithmCore::Sorting::RadixCountingSort<_type>::sortIndex(task.data, (uint32_t)task.count, task.tmp_buffer);
                //                             task.completion_semaphore->release(); });
                //     }

                //     // barrier -- finish all sorting
                //     for (uint64_t i = 0; i < count; i += job_thread_size)
                //         completion_semaphore.blockingAcquire();
                // }

                // // merge down the blocks
                // {
                //     AlgorithmCore::Sorting::SortIndex<_type> *in = data;
                //     AlgorithmCore::Sorting::SortIndex<_type> *out = aux;

                //     uint64_t element_count = job_thread_size;

                //     TaskMerge<AlgorithmCore::Sorting::SortIndex<_type>> task;
                //     while (element_count < count)
                //     {
                //         // merge operation
                //         for (uint64_t i = 0; i < count; i += (element_count << 1))
                //         {

                //             task.in = in;
                //             task.out = out;
                //             task.i = i;
                //             task.element_count = element_count;
                //             task.count = count;
                //             task.completion_semaphore = &completion_semaphore;

                //             threadpool->postTask([task]()
                //                                  {
                //                                      uint64_t write_index = task.i;
                //                                      uint64_t write_max = task.i + (task.element_count << 1);
                //                                      if (write_max > (uint64_t)task.count)
                //                                          write_max = (uint64_t)task.count;

                //                                      uint64_t a_index = task.i;
                //                                      uint64_t b_index = task.i + task.element_count;

                //                                      uint64_t a_max = b_index;
                //                                      uint64_t b_max = b_index + task.element_count;

                //                                      if (a_max > (uint64_t)task.count)
                //                                          a_max = (uint64_t)task.count;
                //                                      if (b_max > (uint64_t)task.count)
                //                                          b_max = (uint64_t)task.count;

                //                                      while (write_index < write_max &&
                //                                             a_index < a_max &&
                //                                             b_index < b_max)
                //                                      {

                //                                          const auto &_a = task.in[a_index];
                //                                          const auto &_b = task.in[b_index];

                //                                          if (_a.toSort > _b.toSort)
                //                                          {
                //                                              task.out[write_index] = _b;
                //                                              b_index++;
                //                                          }
                //                                          else
                //                                          {
                //                                              task.out[write_index] = _a;
                //                                              a_index++;
                //                                          }

                //                                          write_index++;
                //                                      }

                //                                      while (a_index < a_max)
                //                                          task.out[write_index++] = task.in[a_index++];
                //                                      while (b_index < b_max)
                //                                          task.out[write_index++] = task.in[b_index++];
                //                                      task.completion_semaphore->release(); //
                //                                  });
                //         }

                //         // barrier - wait all jobs done
                //         for (uint64_t i = 0; i < count; i += (element_count << 1))
                //             completion_semaphore.blockingAcquire();

                //         // swap in/out
                //         auto aux = in;
                //         in = out;
                //         out = aux;

                //         element_count = element_count << 1;
                //     }

                //     if (in != data)
                //     {
                //         // printf("Slow Sorting Warning: copy all data at end\n");
                //         memcpy(data, in, sizeof(AlgorithmCore::Sorting::SortIndex<_type>) * count);
                //     }
                // }
            }
        };

        using ParallelRadixCountingSortu32 = ParallelRadixCountingSort<uint32_t>;
        using ParallelRadixCountingSorti32 = ParallelRadixCountingSort<int32_t>;

        using ParallelRadixCountingSortu64 = ParallelRadixCountingSort<uint64_t>;
        using ParallelRadixCountingSorti64 = ParallelRadixCountingSort<int64_t>;
    }
}