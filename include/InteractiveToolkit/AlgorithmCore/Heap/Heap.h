#pragma once

#include "../../common.h"

namespace AlgorithmCore
{
    namespace Heap
    {
        namespace Order
        {
            /// \brief Heap ordering policy that produces a min-heap.
            ///
            /// With this policy the smallest element (according to the element's
            /// `operator<`) is kept at the root of the heap. The policy is expressed
            /// as a set of stateless comparison predicates consumed by the heap
            /// algorithms in this namespace.
            ///
            /// \author Alessandro Ribeiro
            ///
            struct MinHeap
            {
                /// \brief Test whether a child element should be swapped with its parent.
                ///
                /// Returns `true` when the child is strictly smaller than the parent,
                /// which violates the min-heap invariant and requires a swap.
                ///
                /// \author Alessandro Ribeiro
                /// \param parent The parent element.
                /// \param child The child element.
                /// \return `true` if the child must be moved above the parent.
                ///
                template <typename T>
                static constexpr inline bool can_swap(const T &parent, const T &child) noexcept { return child < parent; }
                /// \brief Test whether the right child should be selected during a heap-down.
                ///
                /// Returns `true` when the right child is strictly smaller than the left
                /// child, meaning the right child is the one that must be compared against
                /// the element being moved down.
                ///
                /// \author Alessandro Ribeiro
                /// \param left The left child element.
                /// \param right The right child element.
                /// \return `true` if the right child is the smallest of the two.
                ///
                template <typename T>
                static constexpr inline bool can_select_right_on_heap_down(const T &left, const T &right) noexcept { return right < left; }
            };
            /// \brief Heap ordering policy that produces a max-heap.
            ///
            /// With this policy the largest element (according to the element's
            /// `operator<`) is kept at the root of the heap. The policy is expressed
            /// as a set of stateless comparison predicates consumed by the heap
            /// algorithms in this namespace.
            ///
            /// \author Alessandro Ribeiro
            ///
            struct MaxHeap
            {
                /// \brief Test whether a child element should be swapped with its parent.
                ///
                /// Returns `true` when the parent is strictly smaller than the child,
                /// which violates the max-heap invariant and requires a swap.
                ///
                /// \author Alessandro Ribeiro
                /// \param parent The parent element.
                /// \param child The child element.
                /// \return `true` if the child must be moved above the parent.
                ///
                template <typename T>
                static constexpr inline bool can_swap(const T &parent, const T &child) noexcept { return parent < child; }
                /// \brief Test whether the right child should be selected during a heap-down.
                ///
                /// Returns `true` when the left child is strictly smaller than the right
                /// child, meaning the right child is the one that must be compared against
                /// the element being moved down.
                ///
                /// \author Alessandro Ribeiro
                /// \param left The left child element.
                /// \param right The right child element.
                /// \return `true` if the right child is the largest of the two.
                ///
                template <typename T>
                static constexpr inline bool can_select_right_on_heap_down(const T &left, const T &right) noexcept { return left < right; }
            };
            /// \brief Heap ordering policy that reuses the MaxHeap predicates (STL min-heap convention).
            ///
            /// The name follows the STL convention, where the comparison logic is inverted
            /// with respect to the native policies. STL heap algorithms (`std::push_heap`,
            /// `std::pop_heap`, `std::make_heap`) build a max-heap by default using
            /// `operator<`, so a min-heap requires an inverted comparator such as
            /// `std::greater<T>`. Because the comparison is inverted, this policy reuses
            /// the `MaxHeap` predicates to produce a min-heap in the STL sense.
            ///
            /// \author Alessandro Ribeiro
            ///
            struct StlMinHeap : public MaxHeap
            {
            };
            /// \brief Heap ordering policy that reuses the MinHeap predicates (STL max-heap convention).
            ///
            /// The name follows the STL convention, where the comparison logic is inverted
            /// with respect to the native policies. STL heap algorithms (`std::push_heap`,
            /// `std::pop_heap`, `std::make_heap`) build a max-heap by default using
            /// `operator<`, so a max-heap is the STL default. Because the comparison is
            /// inverted relative to the native `MaxHeap` policy, this policy reuses the
            /// `MinHeap` predicates to produce a max-heap in the STL sense.
            ///
            /// \author Alessandro Ribeiro
            ///
            struct StlMaxHeap : public MinHeap
            {
            };
        }

        /// \brief Trait that updates a heap item's position id when the item supports it.
        ///
        /// Primary template (fallback): used when the element type does not provide a
        /// `set_heap_id` member. It is a no-op, so heap algorithms can always call it
        /// without knowing whether the element tracks its own position.
        ///
        /// \author Alessandro Ribeiro
        ///
        /// \tparam T The heap element type.
        ///
        template <typename T, typename = void>
        struct SetHeapItemID : std::false_type
        {
            /// \brief No-op position update for elements that do not track their heap id.
            ///
            /// \author Alessandro Ribeiro
            /// \param v The heap element (unused).
            /// \param id The new position in the heap (unused).
            ///
            static const void set_heap_id(T &v, std::size_t id) noexcept {}
        };

        /// \brief Trait specialization for heap items that expose a `set_heap_id` member.
        ///
        /// Selected via SFINAE when `T` declares a `set_heap_id` member function. In that
        /// case the heap algorithms keep the element's internal position id in sync
        /// every time the element is swapped, which enables O(1) decrease-key style
        /// updates in algorithms such as Dijkstra.
        ///
        /// \author Alessandro Ribeiro
        ///
        /// \tparam T The heap element type, required to provide `set_heap_id(std::size_t)`.
        ///
        template <typename T>
        struct SetHeapItemID<T, decltype(&T::set_heap_id, void())> : std::true_type
        {
            /// \brief Forward the new heap position to the element's `set_heap_id` member.
            ///
            /// \author Alessandro Ribeiro
            /// \param v The heap element.
            /// \param id The new position of the element in the heap.
            ///
            static const void set_heap_id(T &v, std::size_t id) noexcept { v.set_heap_id(id); }
        };

        /// \brief Restore the heap invariant by moving an element up towards the root.
        ///
        /// Starting at `pos`, the element is repeatedly swapped with its parent while
        /// the ordering policy says the child should be above the parent. Each swap
        /// updates the position id of the two moved elements through the
        /// `SetHeapItemID` trait.
        ///
        /// Example:
        ///
        /// \code
        /// std::vector<HeapItem> heap;
        /// heap.push_back( item );
        ///
        /// AlgorithmCore::Heap::up_heap<AlgorithmCore::Heap::Order::StlMinHeap, std::vector<HeapItem>>( heap, heap.size() - 1 );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \tparam Order The heap ordering policy (min-heap or max-heap predicates).
        /// \tparam T The container type; its `value_type` must support `operator<` and the container must support `operator[]`.
        /// \param v The heap container.
        /// \param pos The index of the element to move up.
        ///
        template <typename Order = Order::StlMinHeap, typename T>
        static inline void up_heap(T &v, std::size_t pos) noexcept
        {
            using SetHeapItemID_T = SetHeapItemID<typename T::value_type>;

            if (pos <= 1)
                return;

            std::size_t to_insert = pos;
            std::size_t parent = (to_insert - 1) >> 1;

            // while heap criteria is not satisfied, swap with the parent and continue up the heap
            while (to_insert > 0 && Order::can_swap(v[parent], v[to_insert]))
            {
                std::swap(v[parent], v[to_insert]);

                SetHeapItemID_T::set_heap_id(v[parent], parent);
                SetHeapItemID_T::set_heap_id(v[to_insert], to_insert);

                to_insert = parent;
                parent = (to_insert - 1) >> 1;
            }
        }

        /// \brief Insert the last element of the container into the heap.
        ///
        /// Treats `v[0 .. heap_size - 1]` as a heap and assumes the newly appended
        /// element (at index `heap_size - 1`) is the only one violating the heap
        /// invariant. It registers the element's position id and then calls
        /// `up_heap` to restore the invariant.
        ///
        /// Example:
        ///
        /// \code
        /// std::vector<HeapItem> heap;
        /// heap.push_back( new_item );
        ///
        /// AlgorithmCore::Heap::push_heap<AlgorithmCore::Heap::Order::StlMinHeap, std::vector<HeapItem>>( heap );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \tparam Order The heap ordering policy (min-heap or max-heap predicates).
        /// \tparam T The container type; its `value_type` must support `operator<` and the container must support `operator[]` and `size()`.
        /// \param v The heap container.
        /// \param size_ The number of elements that form the heap; pass `SIZE_MAX` (default) to use the whole container.
        ///
        template <typename Order = Order::StlMinHeap, typename T>
        static inline void push_heap(T &v, std::size_t size_ = SIZE_MAX) noexcept
        {
            using SetHeapItemID_T = SetHeapItemID<typename T::value_type>;

            std::size_t heap_size = (size_ != SIZE_MAX) ? size_ : v.size();

            if (heap_size == 0)
                return;

            std::size_t to_insert = heap_size - 1;

            // printf("push_heap: %zu\n", (uint64_t)v[to_insert].heap_total_cost);

            SetHeapItemID_T::set_heap_id(v[to_insert], to_insert);

            up_heap<Order, T>(v, to_insert);
        }

        /// \brief Restore the heap invariant by moving an element down towards the leaves.
        ///
        /// Starting at `pos`, the element is repeatedly swapped with its smallest (or
        /// largest, depending on the ordering policy) child while the ordering policy
        /// says a child should be above it. Each swap updates the position id of the
        /// two moved elements through the `SetHeapItemID` trait.
        ///
        /// Example:
        ///
        /// \code
        /// std::vector<HeapItem> heap;
        /// // heap is a valid heap except possibly at index 0
        ///
        /// AlgorithmCore::Heap::down_heap<AlgorithmCore::Heap::Order::StlMinHeap, std::vector<HeapItem>>( heap, 0 );
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \tparam Order The heap ordering policy (min-heap or max-heap predicates).
        /// \tparam T The container type; its `value_type` must support `operator<` and the container must support `operator[]` and `size()`.
        /// \param v The heap container.
        /// \param pos The index of the element to move down.
        /// \param size_ The number of elements that form the heap; pass `SIZE_MAX` (default) to use the whole container.
        ///
        template <typename Order = Order::StlMinHeap, typename T>
        static inline void down_heap(T &v, std::size_t pos, std::size_t size_ = SIZE_MAX) noexcept
        {
            using SetHeapItemID_T = SetHeapItemID<typename T::value_type>;

            std::size_t heap_size = (size_ != SIZE_MAX) ? size_ : v.size();

            if (heap_size <= 1)
                return;

            std::size_t to_move_down = pos;
            std::size_t aux = (to_move_down << 1);
            std::size_t left = aux + 1;
            std::size_t right = aux + 2;

            // Pick the child
            std::size_t child = (right < heap_size && Order::can_select_right_on_heap_down(v[left], v[right])) ? right : left;

            // while heap criteria is not satisfied, swap with the child and continue down the heap
            while (left < heap_size && Order::can_swap(v[to_move_down], v[child]))
            {
                std::swap(v[to_move_down], v[child]);

                SetHeapItemID_T::set_heap_id(v[to_move_down], to_move_down);
                SetHeapItemID_T::set_heap_id(v[child], child);

                to_move_down = child;

                aux = (to_move_down << 1);
                left = aux + 1;
                right = aux + 2;

                // Pick the child
                child = (right < heap_size && Order::can_select_right_on_heap_down(v[left], v[right])) ? right : left;
            }
        }

        /// \brief Remove the root element from the heap while preserving the heap invariant.
        ///
        /// Swaps the root with the last element of the heap, marks the last element as
        /// no longer part of the heap (position id `SIZE_MAX`), and then calls
        /// `down_heap` on the new root to restore the invariant over
        /// `v[0 .. heap_size - 2]`. The caller is responsible for actually removing the
        /// last element from the container.
        ///
        /// Example:
        ///
        /// \code
        /// std::vector<HeapItem> heap;
        /// // heap is a valid heap
        ///
        /// AlgorithmCore::Heap::pop_heap<AlgorithmCore::Heap::Order::StlMinHeap, std::vector<HeapItem>>( heap );
        ///
        /// HeapItem top = heap.back();
        /// heap.pop_back();
        /// \endcode
        ///
        /// \author Alessandro Ribeiro
        /// \tparam Order The heap ordering policy (min-heap or max-heap predicates).
        /// \tparam T The container type; its `value_type` must support `operator<` and the container must support `operator[]` and `size()`.
        /// \param v The heap container.
        /// \param size_ The number of elements that form the heap; pass `SIZE_MAX` (default) to use the whole container.
        ///
        template <typename Order = Order::StlMinHeap, typename T>
        static inline void pop_heap(T &v, std::size_t size_ = SIZE_MAX) noexcept
        {
            using SetHeapItemID_T = SetHeapItemID<typename T::value_type>;

            std::size_t heap_size = (size_ != SIZE_MAX) ? size_ : v.size();

            if (heap_size == 0)
                return;
            else if (heap_size == 1)
            {
                // printf("pop_heap: %zu\n", (uint64_t)v[0].heap_total_cost);
                SetHeapItemID_T::set_heap_id(v[0], SIZE_MAX);
                return;
            }

            // printf("pop_heap: %zu\n", (uint64_t)v[0].heap_total_cost);

            std::size_t heap_size_minus_one = heap_size - 1;
            std::swap(v[0], v[heap_size_minus_one]);

            SetHeapItemID_T::set_heap_id(v[0], 0);
            SetHeapItemID_T::set_heap_id(v[heap_size_minus_one], SIZE_MAX);

            down_heap<Order, T>(v, 0, heap_size_minus_one);
        }
    }
}
