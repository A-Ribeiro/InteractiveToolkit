#pragma once

#include "../../common.h"

namespace AlgorithmCore
{
    namespace Heap
    {
        namespace Order
        {
            struct MinHeap
            {
                template <typename T>
                static constexpr inline bool can_swap(const T &parent, const T &child) noexcept { return parent > child; }
                template <typename T>
                static constexpr inline bool can_select_right_on_heap_down(const T &left, const T &right) noexcept { return right < left; }
            };
            struct MaxHeap
            {
                template <typename T>
                static constexpr inline bool can_swap(const T &parent, const T &child) noexcept { return parent < child; }
                template <typename T>
                static constexpr inline bool can_select_right_on_heap_down(const T &left, const T &right) noexcept { return right > left; }
            };
        }

        template <typename Order = Order::MinHeap, typename T>
        static inline void push_heap(T &v) noexcept
        {
            if (v.size() <= 1)
                return;

            std::size_t to_insert = v.size() - 1;
            std::size_t parent = (to_insert - 1) >> 1;

            // while heap criteria is not satisfied, swap with the parent and continue up the heap
            while (to_insert > 0 && Order::can_swap(v[parent], v[to_insert]))
            {
                std::swap(v[parent], v[to_insert]);
                to_insert = parent;
                parent = (to_insert - 1) >> 1;
            }
        }

        template <typename Order = Order::MinHeap, typename T>
        static inline void pop_heap(T &v) noexcept
        {
            if (v.size() <= 1)
                return;

            std::size_t heap_size = v.size() - 1;

            std::swap(v[0], v[heap_size]);

            std::size_t to_move_down = 0;
            std::size_t aux = (to_move_down << 1);
            std::size_t left = aux + 1;
            std::size_t right = aux + 2;

            // Pick the child
            std::size_t child = (right < heap_size && Order::can_select_right_on_heap_down(v[left], v[right])) ? right : left;

            // while heap criteria is not satisfied, swap with the child and continue down the heap
            while (left < heap_size && Order::can_swap(v[to_move_down], v[child]))
            {
                std::swap(v[to_move_down], v[child]);
                to_move_down = child;

                aux = (to_move_down << 1);
                left = aux + 1;
                right = aux + 2;

                // Pick the child
                child = (right < heap_size && Order::can_select_right_on_heap_down(v[left], v[right])) ? right : left;
            }
        }
    }
}
