#pragma once

#include "../../common.h"

namespace AlgorithmCore
{

    namespace Polygon
    {

        template <typename T>
        class FastRemovalCyclicVector
        {
            struct prev_next_indices
            {
                T data;
                uint32_t prev;
                uint32_t next;
            };

            ITK_INLINE void remove(uint32_t index)
            {
                if (m_size == 0 || index >= indices.size())
                    return;

                const auto &idx_ref = indices[index];

                // Atualiza os ponteiros dos vizinhos
                indices[idx_ref.prev].next = idx_ref.next;
                indices[idx_ref.next].prev = idx_ref.prev;

                // move start to next
                if (!cache_friendly_removal)
                {
                    if (index == start_idx)
                        start_idx = idx_ref.next;
                }
                else
                    start_idx = idx_ref.next;

                m_size--;
            }

        public:
            ITK_INLINE FastRemovalCyclicVector(bool cache_friendly_removal_ = true)
            {
                cache_friendly_removal = cache_friendly_removal_;
            }

            ITK_INLINE T &operator[](uint32_t index)
            {
                return indices[index].data;
            }

            ITK_INLINE void resize(uint32_t size_, uint32_t start_idx_)
            {
                m_size = size_;
                indices.resize(m_size);
                // ultra fast initialization of prev and next indices
                for (uint32_t i = 0, prev = -1, next = 1; i < m_size; ++i, ++prev, ++next){
                    auto &item = indices[i];
                    item.prev = prev;
                    item.next = next;
                }
                    
                indices[0].prev = m_size - 1; // wrap around
                indices[m_size - 1].next = 0; // wrap around
                start_idx = start_idx_;
            }

            ITK_INLINE uint32_t size() const
            {
                return m_size;
            }

            class iterator;
            class const_iterator;
            friend class iterator;
            friend class const_iterator;

            class iterator
            {
            public:
                using iterator_category = std::input_iterator_tag;
                using value_type = T;
                using pointer = T *;
                using reference = T &;

                ITK_INLINE iterator(FastRemovalCyclicVector *vec, uint32_t idx)
                    : vec(vec), idx(idx), size(vec->m_size) {}

                ITK_INLINE reference operator*() const { return vec->indices[idx].data; }
                ITK_INLINE pointer operator->() const { return &vec->indices[idx].data; }

                // Prefix increment
                ITK_INLINE iterator operator++()
                {
                    iterator tmp = *this;
                    (*this)++;
                    return tmp;
                }

                // Postfix increment
                ITK_INLINE iterator &operator++(int)
                {
                    if (size == 0)
                    {
                        idx = (uint32_t)vec->indices.size(); // end
                        return *this;
                    }
                    size--;
                    idx = vec->indices[idx].next;
                    return *this;
                }

                ITK_INLINE reference element_back() const
                {
                    return vec->indices[vec->indices[idx].prev].data;
                }

                ITK_INLINE reference element_next() const
                {
                    return vec->indices[vec->indices[idx].next].data;
                }

                ITK_INLINE iterator &cyclic_increment()
                {
                    idx = vec->indices[idx].next;
                    return *this;
                }

                ITK_INLINE constexpr bool operator==(const iterator &other) const
                {
                    return idx == other.idx && vec == other.vec;
                }
                ITK_INLINE constexpr bool operator!=(const iterator &other) const
                {
                    return !(*this == other);
                }

            private:
                FastRemovalCyclicVector *vec;
                uint32_t idx;
                uint32_t size;

                friend class FastRemovalCyclicVector<T>;
            };

            class const_iterator
            {
            public:
                using iterator_category = std::input_iterator_tag;
                using value_type = const T;
                using pointer = const T *;
                using reference = const T &;

                ITK_INLINE const_iterator(const FastRemovalCyclicVector *vec, uint32_t idx)
                    : vec(vec), idx(idx), size(vec->m_size) {}

                ITK_INLINE reference operator*() const { return vec->indices[idx].data; }
                ITK_INLINE pointer operator->() const { return &vec->indices[idx].data; }

                // Prefix increment
                ITK_INLINE const_iterator operator++()
                {
                    const_iterator tmp = *this;
                    (*this)++;
                    return tmp;
                }

                // Postfix increment
                ITK_INLINE const_iterator &operator++(int)
                {
                    if (size == 0)
                    {
                        idx = vec->indices.size(); // end
                        return *this;
                    }
                    size--;
                    idx = vec->indices[idx].next;
                    return *this;
                }

                ITK_INLINE reference element_back() const
                {
                    return vec->indices[vec->indices[idx].prev].data;
                }

                ITK_INLINE reference element_next() const
                {
                    return vec->indices[vec->indices[idx].next].data;
                }

                ITK_INLINE const_iterator &cyclic_increment()
                {
                    idx = vec->indices[idx].next;
                    return *this;
                }

                ITK_INLINE constexpr bool operator==(const const_iterator &other) const
                {
                    return idx == other.idx && vec == other.vec;
                }
                ITK_INLINE constexpr bool operator!=(const const_iterator &other) const
                {
                    return !(*this == other);
                }

            private:
                const FastRemovalCyclicVector *vec;
                uint32_t idx;
                uint32_t size;

                friend class FastRemovalCyclicVector<T>;
            };

            ITK_INLINE iterator begin() { return iterator(this, start_idx); }
            ITK_INLINE iterator end() { return iterator(this, (uint32_t)indices.size()); }
            ITK_INLINE const_iterator begin() const { return const_iterator(this, start_idx); }
            ITK_INLINE const_iterator end() const { return const_iterator(this, (uint32_t)indices.size()); }
            ITK_INLINE const_iterator cbegin() const { return begin(); }
            ITK_INLINE const_iterator cend() const { return end(); }

            ITK_INLINE void remove(const iterator &it)
            {
                remove(it.idx);
            }
            ITK_INLINE void remove(const const_iterator &it)
            {
                remove(it.idx);
            }

        private:
            // std::vector<T> data;
            std::vector<prev_next_indices> indices;
            uint32_t m_size;
            uint32_t start_idx;
            bool cache_friendly_removal;
        };

        class FastRemovalCyclicVector_OnlyIndex_Uint32
        {
            struct prev_next_indices
            {
                uint32_t prev;
                uint32_t next;
            };

            ITK_INLINE void remove(uint32_t index)
            {
                if (m_size == 0 || index >= indices.size())
                    return;

                const auto &idx_ref = indices[index];

                // Atualiza os ponteiros dos vizinhos
                indices[idx_ref.prev].next = idx_ref.next;
                indices[idx_ref.next].prev = idx_ref.prev;

                // move start to next
                if (!cache_friendly_removal)
                {
                    if (index == start_idx)
                        start_idx = idx_ref.next;
                }
                else
                    start_idx = idx_ref.next;

                m_size--;
            }

        public:
            ITK_INLINE FastRemovalCyclicVector_OnlyIndex_Uint32(bool cache_friendly_removal_ = true)
            {
                cache_friendly_removal = cache_friendly_removal_;
            }

            // ITK_INLINE T &operator[](uint32_t index)
            // {
            //     return indices[index].data;
            // }

            ITK_INLINE void resize(uint32_t size_, uint32_t start_idx_)
            {
                m_size = size_;
                indices.resize(m_size);
                // ultra fast initialization of prev and next indices
                for (uint32_t i = 0, prev = -1, next = 1; i < m_size; ++i, ++prev, ++next)
                    indices[i] = {prev, next};
                indices[0].prev = m_size - 1; // wrap around
                indices[m_size - 1].next = 0; // wrap around
                start_idx = start_idx_;
            }

            ITK_INLINE uint32_t size() const
            {
                return m_size;
            }

            class iterator;
            class const_iterator;
            friend class iterator;
            friend class const_iterator;

            class iterator
            {
            public:
                using iterator_category = std::input_iterator_tag;
                using value_type = uint32_t;
                // using pointer = T *;
                // using reference = T &;

                ITK_INLINE iterator(FastRemovalCyclicVector_OnlyIndex_Uint32 *vec, uint32_t idx)
                    : vec(vec), idx(idx), size(vec->m_size) {}

                // ITK_INLINE reference operator*() const { return vec->indices[idx].data; }
                // ITK_INLINE pointer operator->() const { return &vec->indices[idx].data; }

                ITK_INLINE value_type operator*() const { return idx; }

                // Prefix increment
                ITK_INLINE iterator operator++()
                {
                    iterator tmp = *this;
                    (*this)++;
                    return tmp;
                }

                // Postfix increment
                ITK_INLINE iterator &operator++(int)
                {
                    if (size == 0)
                    {
                        idx = (uint32_t)vec->indices.size(); // end
                        return *this;
                    }
                    size--;
                    idx = vec->indices[idx].next;
                    return *this;
                }

                ITK_INLINE value_type element_back() const
                {
                    return vec->indices[idx].prev;
                }

                ITK_INLINE value_type element_current() const
                {
                    return idx;
                }

                ITK_INLINE value_type element_next() const
                {
                    return vec->indices[idx].next;
                }

                ITK_INLINE iterator &cyclic_increment()
                {
                    idx = vec->indices[idx].next;
                    return *this;
                }

                ITK_INLINE constexpr bool operator==(const iterator &other) const
                {
                    return idx == other.idx && vec == other.vec;
                }
                ITK_INLINE constexpr bool operator!=(const iterator &other) const
                {
                    return !(*this == other);
                }

            private:
                FastRemovalCyclicVector_OnlyIndex_Uint32 *vec;
                uint32_t idx;
                uint32_t size;

                friend class FastRemovalCyclicVector_OnlyIndex_Uint32;
            };

            class const_iterator
            {
            public:
                using iterator_category = std::input_iterator_tag;
                using value_type = const uint32_t;
                // using pointer = const T *;
                // using reference = const T &;

                ITK_INLINE const_iterator(const FastRemovalCyclicVector_OnlyIndex_Uint32 *vec, uint32_t idx)
                    : vec(vec), idx(idx), size(vec->m_size) {}

                // ITK_INLINE reference operator*() const { return vec->indices[idx].data; }
                // ITK_INLINE pointer operator->() const { return &vec->indices[idx].data; }

                ITK_INLINE value_type operator*() const { return idx; }

                // Prefix increment
                ITK_INLINE const_iterator operator++()
                {
                    const_iterator tmp = *this;
                    (*this)++;
                    return tmp;
                }

                // Postfix increment
                ITK_INLINE const_iterator &operator++(int)
                {
                    if (size == 0)
                    {
                        idx = vec->indices.size(); // end
                        return *this;
                    }
                    size--;
                    idx = vec->indices[idx].next;
                    return *this;
                }

                ITK_INLINE value_type element_back() const
                {
                    return vec->indices[idx].prev;
                }

                ITK_INLINE value_type element_current() const
                {
                    return idx;
                }

                ITK_INLINE value_type element_next() const
                {
                    return vec->indices[idx].next;
                }

                ITK_INLINE const_iterator &cyclic_increment()
                {
                    idx = vec->indices[idx].next;
                    return *this;
                }

                ITK_INLINE constexpr bool operator==(const const_iterator &other) const
                {
                    return idx == other.idx && vec == other.vec;
                }
                ITK_INLINE constexpr bool operator!=(const const_iterator &other) const
                {
                    return !(*this == other);
                }

            private:
                const FastRemovalCyclicVector_OnlyIndex_Uint32 *vec;
                uint32_t idx;
                uint32_t size;

                friend class FastRemovalCyclicVector_OnlyIndex_Uint32;
            };

            ITK_INLINE iterator begin() { return iterator(this, start_idx); }
            ITK_INLINE iterator end() { return iterator(this, (uint32_t)indices.size()); }
            ITK_INLINE const_iterator begin() const { return const_iterator(this, start_idx); }
            ITK_INLINE const_iterator end() const { return const_iterator(this, (uint32_t)indices.size()); }
            ITK_INLINE const_iterator cbegin() const { return begin(); }
            ITK_INLINE const_iterator cend() const { return end(); }

            ITK_INLINE void remove(const iterator &it)
            {
                remove(it.idx);
            }
            ITK_INLINE void remove(const const_iterator &it)
            {
                remove(it.idx);
            }

        private:
            // std::vector<T> data;
            std::vector<prev_next_indices> indices;
            uint32_t m_size;
            uint32_t start_idx;
            bool cache_friendly_removal;
        };

    }
}