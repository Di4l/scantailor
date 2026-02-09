/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2025

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <vector>
#include <map>
#include <memory>
#include <functional>
#include <algorithm>
#include <utility>
#include <stdexcept>
#include <type_traits>
#include <tuple>

namespace st
{
namespace multi_index
{

// ============================================================================
// TAGS - Para identificar índices (similar a boost::multi_index::tag)
// ============================================================================

template<typename Name>
struct tag
{
    using name = Name;
};

// ============================================================================
// KEY EXTRACTORS - Extraen la clave de un elemento
// ============================================================================

template<typename T, typename MemberType, MemberType T::*Member>
struct member
{
    using key_type = MemberType;
    
    static key_type extract(const T& item)
    {
        return item.*Member;
    }
    
    key_type operator()(const T& item) const
    {
        return extract(item);
    }
};

template<typename... KeyExtractors>
struct composite_key
{
    using key_type = std::tuple<typename KeyExtractors::key_type...>;
    
    static key_type extract(const auto& item)
    {
        return std::make_tuple(KeyExtractors::extract(item)...);
    }
    
    key_type operator()(const auto& item) const
    {
        return extract(item);
    }
};

// ============================================================================
// INDEX BASE CLASS
// ============================================================================

template<typename T>
class index_base
{
protected:
    std::vector<T>* m_storage = nullptr;
    
public:
    virtual ~index_base() noexcept = default;
    
    virtual void on_insert(size_t pos) = 0;
    virtual void on_erase(size_t pos) = 0;
    virtual void on_clear() noexcept = 0;
    virtual void set_storage(std::vector<T>* storage) noexcept
    {
        m_storage = storage;
    }
};

// ============================================================================
// ORDERED_UNIQUE_INDEX
// ============================================================================

template<typename T, typename Tag, typename KeyExtractor>
class ordered_unique_index : public index_base<T>
{
public:
    using tag_type = Tag;
    using key_type = typename KeyExtractor::key_type;
    using map_type = std::map<key_type, size_t>;
    
private:
    map_type m_index;
    
public:
    // -------- Iterators --------
    class iterator
    {
    private:
        std::vector<T>* m_storage;
        typename map_type::iterator m_map_it;
        
        friend class ordered_unique_index;
        
        iterator(std::vector<T>* storage, typename map_type::iterator it) noexcept
            : m_storage(storage), m_map_it(it)
        {
        }
        
    public:
        iterator() noexcept : m_storage(nullptr)
        {
        }
        
        T& operator*() noexcept
        {
            return (*m_storage)[m_map_it->second];
        }
        
        T* operator->() noexcept
        {
            return &(*m_storage)[m_map_it->second];
        }

        T& operator*() const noexcept
        {
            return (*m_storage)[m_map_it->second];
        }

        T* operator->() const noexcept
        {
            return &(*m_storage)[m_map_it->second];
        }
        
        iterator& operator++() noexcept
        {
            ++m_map_it;
            return *this;
        }
        
        iterator operator++(int) noexcept
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        iterator& operator--() noexcept
        {
            --m_map_it;
            return *this;
        }
        
        iterator operator--(int) noexcept
        {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const noexcept
        {
            return m_map_it == other.m_map_it;
        }
        
        bool operator!=(const iterator& other) const noexcept
        {
            return m_map_it != other.m_map_it;
        }
    };
    
    class const_iterator
    {
    private:
        const std::vector<T>* m_storage;
        typename map_type::const_iterator m_map_it;
        
        friend class ordered_unique_index;
        
        const_iterator(const std::vector<T>* storage, typename map_type::const_iterator it) noexcept
            : m_storage(storage), m_map_it(it)
        {
        }
        
    public:
        const_iterator() noexcept : m_storage(nullptr)
        {
        }
        
        const T& operator*() const noexcept
        {
            return (*m_storage)[m_map_it->second];
        }
        
        const T* operator->() const noexcept
        {
            return &(*m_storage)[m_map_it->second];
        }
        
        const_iterator& operator++() noexcept
        {
            ++m_map_it;
            return *this;
        }
        
        const_iterator operator++(int) noexcept
        {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        const_iterator& operator--() noexcept
        {
            --m_map_it;
            return *this;
        }
        
        const_iterator operator--(int) noexcept
        {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const const_iterator& other) const noexcept
        {
            return m_map_it == other.m_map_it;
        }
        
        bool operator!=(const const_iterator& other) const noexcept
        {
            return m_map_it != other.m_map_it;
        }
    };
    
    // -------- Operations --------
    iterator find(const key_type& key) noexcept
    {
        auto it = m_index.find(key);
        if (it != m_index.end())
            return iterator(this->m_storage, it);
        return end();
    }
    
    const_iterator find(const key_type& key) const noexcept
    {
        auto it = m_index.find(key);
        if (it != m_index.end())
            return const_iterator(this->m_storage, it);
        return end();
    }
    
    iterator upper_bound(const key_type& key) noexcept
    {
        auto it = m_index.upper_bound(key);
        if (it != m_index.end())
            return iterator(this->m_storage, it);
        return end();
    }
    
    const_iterator upper_bound(const key_type& key) const noexcept
    {
        auto it = m_index.upper_bound(key);
        if (it != m_index.end())
            return const_iterator(this->m_storage, it);
        return end();
    }
    
    iterator lower_bound(const key_type& key) noexcept
    {
        auto it = m_index.lower_bound(key);
        if (it != m_index.end())
            return iterator(this->m_storage, it);
        return end();
    }
    
    const_iterator lower_bound(const key_type& key) const noexcept
    {
        auto it = m_index.lower_bound(key);
        if (it != m_index.end())
            return const_iterator(this->m_storage, it);
        return end();
    }
    
    iterator begin() noexcept
    {
        return iterator(this->m_storage, m_index.begin());
    }
    
    const_iterator begin() const noexcept
    {
        return const_iterator(this->m_storage, m_index.cbegin());
    }
    
    iterator end() noexcept
    {
        return iterator(this->m_storage, m_index.end());
    }
    
    const_iterator end() const noexcept
    {
        return const_iterator(this->m_storage, m_index.cend());
    }

    iterator iterator_to(T const& value) noexcept
    {
        const T* ptr = &value;
        for (auto it = m_index.begin(); it != m_index.end(); ++it) {
            if (&(*this->m_storage)[it->second] == ptr) {
                return iterator(this->m_storage, it);
            }
        }
        return end();
    }

    const_iterator iterator_to(T const& value) const noexcept
    {
        const T* ptr = &value;
        for (auto it = m_index.cbegin(); it != m_index.cend(); ++it) {
            if (&(*this->m_storage)[it->second] == ptr) {
                return const_iterator(this->m_storage, it);
            }
        }
        return end();
    }
    
    size_t size() const noexcept
    {
        return m_index.size();
    }
    
    bool empty() const noexcept
    {
        return m_index.empty();
    }
    
    template<typename Modifier>
    void modify(iterator it, Modifier modifier)
    {
        size_t const pos = it.m_map_it->second;
        T& item = (*this->m_storage)[pos];
        typename map_type::iterator map_it = it.m_map_it;
        modifier(item);
        key_type const new_key = KeyExtractor()(item);
        m_index.erase(map_it);
        it.m_map_it = m_index.insert({ new_key, pos }).first;
    }

    // -------- Index Updates --------
    void on_insert(size_t pos) override
    {
        if (pos < this->m_storage->size())
        {
            const auto& item = (*this->m_storage)[pos];
            auto key = KeyExtractor()(item);
            m_index[key] = pos;
        }
    }
    
    void on_erase(size_t pos) override
    {
        // Find and remove the entry with value == pos
        for (auto it = m_index.begin(); it != m_index.end(); ++it)
        {
            if (it->second == pos)
            {
                m_index.erase(it);
                break;
            }
        }
        // Adjust indices > pos
        for (auto& [key, idx] : m_index)
        {
            if (idx > pos)
                idx--;
        }
    }
    
    void on_clear() noexcept override
    {
        m_index.clear();
    }
};

// ============================================================================
// ORDERED_NON_UNIQUE_INDEX
// ============================================================================

template<typename T, typename Tag, typename KeyExtractor, typename Compare = std::less<typename KeyExtractor::key_type>>
class ordered_non_unique_index : public index_base<T>
{
public:
    using tag_type = Tag;
    using key_type = typename KeyExtractor::key_type;
    using map_type = std::multimap<key_type, size_t, Compare>;
    
private:
    map_type m_index;
    
public:
    // -------- Iterators --------
    class iterator
    {
    private:
        std::vector<T>* m_storage;
        typename map_type::iterator m_map_it;
        
        friend class ordered_non_unique_index;
        
        iterator(std::vector<T>* storage, typename map_type::iterator it) noexcept
            : m_storage(storage), m_map_it(it)
        {
        }
        
    public:
        iterator() noexcept : m_storage(nullptr)
        {
        }
        
        T& operator*() noexcept
        {
            return (*m_storage)[m_map_it->second];
        }
        
        T* operator->() noexcept
        {
            return &(*m_storage)[m_map_it->second];
        }
        
        iterator& operator++() noexcept
        {
            ++m_map_it;
            return *this;
        }
        
        iterator operator++(int) noexcept
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        iterator& operator--() noexcept
        {
            --m_map_it;
            return *this;
        }
        
        iterator operator--(int) noexcept
        {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const noexcept
        {
            return m_map_it == other.m_map_it;
        }
        
        bool operator!=(const iterator& other) const noexcept
        {
            return m_map_it != other.m_map_it;
        }
    };
    
    class const_iterator
    {
    private:
        std::vector<T>* m_storage;
        typename map_type::const_iterator m_map_it;
        
        friend class ordered_non_unique_index;
        
        const_iterator(std::vector<T>* storage, typename map_type::const_iterator it) noexcept
            : m_storage(storage), m_map_it(it)
        {
        }
        
    public:
        const_iterator() noexcept : m_storage(nullptr)
        {
        }
        
        const T& operator*() const noexcept
        {
            return (*m_storage)[m_map_it->second];
        }
        
        const T* operator->() const noexcept
        {
            return &(*m_storage)[m_map_it->second];
        }
        
        const_iterator& operator++() noexcept
        {
            ++m_map_it;
            return *this;
        }
        
        const_iterator operator++(int) noexcept
        {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        const_iterator& operator--() noexcept
        {
            --m_map_it;
            return *this;
        }
        
        const_iterator operator--(int) noexcept
        {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const const_iterator& other) const noexcept
        {
            return m_map_it == other.m_map_it;
        }
        
        bool operator!=(const const_iterator& other) const noexcept
        {
            return m_map_it != other.m_map_it;
        }
    };
    
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    
    // -------- Accessors --------
    iterator begin() noexcept
    {
        return iterator(this->m_storage, m_index.begin());
    }
    
    iterator end() noexcept
    {
        return iterator(this->m_storage, m_index.end());
    }
    
    const_iterator begin() const noexcept
    {
        return const_iterator(this->m_storage, m_index.begin());
    }
    
    const_iterator end() const noexcept
    {
        return const_iterator(this->m_storage, m_index.end());
    }
    
    reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }
    
    reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }
    
    const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }
    
    const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }
    
    // -------- Find --------
    iterator find(const key_type& key) noexcept
    {
        return iterator(this->m_storage, m_index.find(key));
    }
    
    const_iterator find(const key_type& key) const noexcept
    {
        return const_iterator(this->m_storage, m_index.find(key));
    }
    
    // -------- Lower/Upper bound --------
    iterator lower_bound(const key_type& key) noexcept
    {
        return iterator(this->m_storage, m_index.lower_bound(key));
    }
    
    const_iterator lower_bound(const key_type& key) const noexcept
    {
        return const_iterator(this->m_storage, m_index.lower_bound(key));
    }
    
    iterator upper_bound(const key_type& key) noexcept
    {
        return iterator(this->m_storage, m_index.upper_bound(key));
    }
    
    const_iterator upper_bound(const key_type& key) const noexcept
    {
        return const_iterator(this->m_storage, m_index.upper_bound(key));
    }
    
    // -------- Insert --------
    std::pair<iterator, bool> insert(const T& value)
    {
        key_type key = KeyExtractor::extract(value);
        size_t pos = this->m_storage->size();
        this->m_storage->push_back(value);
        
        auto it = m_index.insert({key, pos});
        return {iterator(this->m_storage, it), true};
    }
    
    // -------- Erase --------
    iterator erase(const_iterator pos)
    {
        size_t storage_pos = pos.m_map_it->second;
        this->m_storage->erase(this->m_storage->begin() + storage_pos);
        
        auto next = m_index.erase(pos.m_map_it);
        
        // Adjust indices > storage_pos
        for (auto& [key, idx] : m_index)
        {
            if (idx > storage_pos)
                idx--;
        }
        
        return iterator(this->m_storage, next);
    }
    
    // -------- Clear --------
    void clear() noexcept
    {
        m_index.clear();
    }
    
    // -------- Callbacks --------
    void on_insert(size_t pos) override
    {
        if (pos < this->m_storage->size())
        {
            const T& value = (*this->m_storage)[pos];
            key_type key = KeyExtractor::extract(value);
            m_index.insert({key, pos});
            
            // Adjust indices >= pos
            for (auto& [k, idx] : m_index)
            {
                if (idx > pos)
                    idx++;
            }
        }
    }
    
    void on_erase(size_t pos) override
    {
        // Find and remove all entries with value == pos
        for (auto it = m_index.begin(); it != m_index.end(); )
        {
            if (it->second == pos)
            {
                it = m_index.erase(it);
            }
            else
            {
                ++it;
            }
        }
        // Adjust indices > pos
        for (auto& [key, idx] : m_index)
        {
            if (idx > pos)
                idx--;
        }
    }
    
    void on_clear() noexcept override
    {
        m_index.clear();
    }
};

// ============================================================================
// SEQUENCED_INDEX
// ============================================================================

template<typename T, typename Tag>
class sequenced_index : public index_base<T>
{
public:
    using tag_type = Tag;
    
private:
    std::vector<size_t> m_order;
    
public:
    // -------- Iterators --------
    class iterator
    {
    private:
        std::vector<T>* m_storage;
        typename std::vector<size_t>::iterator m_order_it;
        
        friend class sequenced_index;
        
        iterator(std::vector<T>* storage, typename std::vector<size_t>::iterator it) noexcept
            : m_storage(storage), m_order_it(it)
        {
        }
        
    public:
        iterator() noexcept : m_storage(nullptr)
        {
        }
        
        T& operator*() noexcept
        {
            return (*m_storage)[*m_order_it];
        }
        
        T* operator->() noexcept
        {
            return &(*m_storage)[*m_order_it];
        }

        T& operator*() const noexcept
        {
            return (*m_storage)[*m_order_it];
        }

        T* operator->() const noexcept
        {
            return &(*m_storage)[*m_order_it];
        }
        
        iterator& operator++() noexcept
        {
            ++m_order_it;
            return *this;
        }
        
        iterator operator++(int) noexcept
        {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        iterator& operator--() noexcept
        {
            --m_order_it;
            return *this;
        }
        
        iterator operator--(int) noexcept
        {
            iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const noexcept
        {
            return m_order_it == other.m_order_it;
        }
        
        bool operator!=(const iterator& other) const noexcept
        {
            return m_order_it != other.m_order_it;
        }
    };
    
    class const_iterator
    {
    private:
        const std::vector<T>* m_storage;
        typename std::vector<size_t>::const_iterator m_order_it;
        
        friend class sequenced_index;
        
        const_iterator(const std::vector<T>* storage, typename std::vector<size_t>::const_iterator it) noexcept
            : m_storage(storage), m_order_it(it)
        {
        }
        
    public:
        const_iterator() noexcept : m_storage(nullptr)
        {
        }
        
        const T& operator*() const noexcept
        {
            return (*m_storage)[*m_order_it];
        }
        
        const T* operator->() const noexcept
        {
            return &(*m_storage)[*m_order_it];
        }
        
        const_iterator& operator++() noexcept
        {
            ++m_order_it;
            return *this;
        }
        
        const_iterator operator++(int) noexcept
        {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        const_iterator& operator--() noexcept
        {
            --m_order_it;
            return *this;
        }
        
        const_iterator operator--(int) noexcept
        {
            const_iterator tmp = *this;
            --(*this);
            return tmp;
        }
        
        bool operator==(const const_iterator& other) const noexcept
        {
            return m_order_it == other.m_order_it;
        }
        
        bool operator!=(const const_iterator& other) const noexcept
        {
            return m_order_it != other.m_order_it;
        }
    };
    
    // -------- Operations --------
    iterator begin() noexcept
    {
        return iterator(this->m_storage, m_order.begin());
    }
    
    const_iterator begin() const noexcept
    {
        return const_iterator(this->m_storage, m_order.cbegin());
    }
    
    iterator end() noexcept
    {
        return iterator(this->m_storage, m_order.end());
    }
    
    const_iterator end() const noexcept
    {
        return const_iterator(this->m_storage, m_order.cend());
    }
    
    size_t size() const noexcept
    {
        return m_order.size();
    }
    
    bool empty() const noexcept
    {
        return m_order.empty();
    }
    
    T& front() noexcept
    {
        return (*this->m_storage)[m_order.front()];
    }
    
    const T& front() const noexcept
    {
        return (*this->m_storage)[m_order.front()];
    }
    
    T& back() noexcept
    {
        return (*this->m_storage)[m_order.back()];
    }
    
    const T& back() const noexcept
    {
        return (*this->m_storage)[m_order.back()];
    }
    
    // -------- Index Updates --------
    void on_insert(size_t pos) override
    {
        m_order.push_back(pos);
    }
    
    void on_erase(size_t pos) override
    {
        auto it = std::find(m_order.begin(), m_order.end(), pos);
        if (it != m_order.end())
        {
            m_order.erase(it);
        }
        // Adjust indices > pos
        for (auto& idx : m_order)
        {
            if (idx > pos)
                idx--;
        }
    }
    
    void on_clear() noexcept override
    {
        m_order.clear();
    }

    void pop_front()
    {
        if (!m_order.empty()) {
            m_order.erase(m_order.begin());
        }
    }

    template<typename Compare>
    void sort(Compare comp)
    {
        std::sort(m_order.begin(), m_order.end(), [this, &comp](size_t lhs, size_t rhs) {
            return comp((*this->m_storage)[lhs], (*this->m_storage)[rhs]);
        });
    }

    iterator iterator_to(T const& value) noexcept
    {
        const T* ptr = &value;
        auto it = std::find_if(m_order.begin(), m_order.end(), [this, ptr](size_t idx) {
            return &(*this->m_storage)[idx] == ptr;
        });
        return iterator(this->m_storage, it);
    }

    const_iterator iterator_to(T const& value) const noexcept
    {
        const T* ptr = &value;
        auto it = std::find_if(m_order.cbegin(), m_order.cend(), [this, ptr](size_t idx) {
            return &(*this->m_storage)[idx] == ptr;
        });
        return const_iterator(this->m_storage, it);
    }
    
    // -------- Relocate (move element to different position) --------
    iterator relocate(iterator position, iterator element) noexcept
    {
        if (element == position)
            return position;
        
        // Get the storage indices
        size_t elem_storage_idx = *element.m_order_it;
        
        // Find and erase the element from current position
        auto elem_order_it = element.m_order_it;
        auto new_order_it = m_order.erase(elem_order_it);
        
        // Insert at new position
        auto insert_it = m_order.insert(position.m_order_it, elem_storage_idx);
        
        return iterator(this->m_storage, insert_it);
    }
    
    // -------- push_front (add to beginning, used for FIFO queues) --------
    std::pair<iterator, bool> push_front(const T& value) noexcept
    {
        size_t pos = this->m_storage->size();
        this->m_storage->push_back(value);
        m_order.insert(m_order.begin(), pos);
        return {iterator(this->m_storage, m_order.begin()), true};
    }
    
    // -------- insert (insert at position) --------
    std::pair<iterator, bool> insert(iterator position, const T& value) noexcept
    {
        size_t pos = this->m_storage->size();
        this->m_storage->push_back(value);
        auto it = m_order.insert(position.m_order_it, pos);
        return {iterator(this->m_storage, it), true};
    }
    
    // -------- erase (erase at position) --------
    iterator erase(iterator position) noexcept
    {
        size_t storage_idx = *position.m_order_it;
        auto next_it = m_order.erase(position.m_order_it);
        
        // Adjust indices in m_order that were after the erased element
        for (auto& idx : m_order)
        {
            if (idx > storage_idx)
                idx--;
        }
        
        // Mark the element as invalid in storage (we're not actually removing from storage)
        // This is because storage uses push_back, so we can't erase from middle
        return iterator(this->m_storage, next_it);
    }
};

// ============================================================================
// INDEX_BY - Container para múltiples índices
// ============================================================================

template<typename... Indices>
class indexed_by
{
public:
    std::tuple<Indices...> indices;
    
    template<size_t N>
    auto& get() noexcept
    {
        return std::get<N>(indices);
    }
    
    template<size_t N>
    const auto& get() const noexcept
    {
        return std::get<N>(indices);
    }
    
    template<typename T>
    void init_storage(std::vector<T>* storage) noexcept
    {
        init_storage_impl<0>(storage);
    }
    
    void on_insert(size_t pos)
    {
        on_insert_impl<0>(pos);
    }
    
    void on_erase(size_t pos)
    {
        on_erase_impl<0>(pos);
    }
    
    void on_clear() noexcept
    {
        on_clear_impl<0>();
    }
    
private:
    template<size_t N, typename T>
    void init_storage_impl(std::vector<T>* storage) noexcept
    {
        if constexpr (N < sizeof...(Indices))
        {
            std::get<N>(indices).set_storage(storage);
            init_storage_impl<N + 1>(storage);
        }
    }
    
    template<size_t N>
    void on_insert_impl(size_t pos)
    {
        if constexpr (N < sizeof...(Indices))
        {
            std::get<N>(indices).on_insert(pos);
            on_insert_impl<N + 1>(pos);
        }
    }
    
    template<size_t N>
    void on_erase_impl(size_t pos)
    {
        if constexpr (N < sizeof...(Indices))
        {
            std::get<N>(indices).on_erase(pos);
            on_erase_impl<N + 1>(pos);
        }
    }
    
    template<size_t N>
    void on_clear_impl() noexcept
    {
        if constexpr (N < sizeof...(Indices))
        {
            std::get<N>(indices).on_clear();
            on_clear_impl<N + 1>();
        }
    }
};


// ============================================================================
// INDEX TYPE RESOLUTION HELPER
// ============================================================================

// Forward declaration for index type resolution
template<typename T, typename IndexedBy>
class multi_index_container;

// Template to get the correct index type from a multi_index_container
template<typename Container, typename Tag>
struct container_index;

// Specializations for each index tag will be provided after multi_index_container is fully defined

// ============================================================================
// MULTI_INDEX_CONTAINER
// ============================================================================

template<typename T, typename IndexedBy>
class multi_index_container
{
private:
    std::vector<T> m_storage;
    IndexedBy m_indices;
    
public:
    using value_type = T;
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;
    
    // -------- Index type accessor (compatible with boost::multi_index) --------
    template<typename Tag>
    struct index
    {
        // Delegate to helper struct defined after class
        using type = typename container_index<multi_index_container, Tag>::type;
    };
    
    multi_index_container() noexcept
    {
        m_indices.init_storage(&m_storage);
    }
    
    // -------- Insert --------
    std::pair<iterator, bool> insert(const T& item)
    {
        // Note: For more complex uniqueness checking, override in derived
        // This basic version just appends
        m_storage.push_back(item);
        size_t pos = m_storage.size() - 1;
        m_indices.on_insert(pos);
        
        return {m_storage.begin() + pos, true};
    }
    
    // -------- Erase --------
    iterator erase(const_iterator it)
    {
        size_t pos = std::distance(m_storage.cbegin(), it);
        m_indices.on_erase(pos);
        return m_storage.erase(it);
    }
    
    // -------- Clear --------
    void clear() noexcept
    {
        m_storage.clear();
        m_indices.on_clear();
    }
    
    // -------- Size and empty --------
    size_t size() const noexcept
    {
        return m_storage.size();
    }
    
    bool empty() const noexcept
    {
        return m_storage.empty();
    }
    
    // -------- Access indices --------
    template<typename Tag>
    auto& get() noexcept
    {
        return get_impl<Tag, 0>();
    }
    
    template<typename Tag>
    const auto& get() const noexcept
    {
        return get_impl<Tag, 0>();
    }
    
    // -------- Project (convert iterator from one index to another) --------
    // Accepts an iterator from any index and returns an iterator in the target index
    template<typename Tag, typename SourceIterator>
    auto project(SourceIterator source_it) noexcept -> typename index<Tag>::type::iterator
    {
        // Get reference to the actual element in storage
        const T& element = *source_it;
        
        // Find the element pointer in storage
        const T* element_ptr = &element;
        
        // Get the target index
        auto& target_index = get<Tag>();
        
        // Find the element in the target index by comparing pointers
        for (auto it = target_index.begin(); it != target_index.end(); ++it)
        {
            if (&(*it) == element_ptr)
                return it;
        }
        
        return target_index.end();
    }

    template<typename Tag, typename SourceIterator>
    auto project(SourceIterator source_it) const noexcept -> typename index<Tag>::type::const_iterator
    {
        const T& element = *source_it;
        const T* element_ptr = &element;

        auto const& target_index = get<Tag>();

        for (auto it = target_index.begin(); it != target_index.end(); ++it)
        {
            if (&(*it) == element_ptr)
                return it;
        }

        return target_index.end();
    }
    
    // -------- Direct storage access --------
    std::vector<T>& storage() noexcept
    {
        return m_storage;
    }
    
    const std::vector<T>& storage() const noexcept
    {
        return m_storage;
    }
    
    // -------- Storage iterators --------
    iterator begin() noexcept
    {
        return m_storage.begin();
    }
    
    iterator end() noexcept
    {
        return m_storage.end();
    }
    
    const_iterator begin() const noexcept
    {
        return m_storage.begin();
    }
    
    const_iterator end() const noexcept
    {
        return m_storage.end();
    }
    
    // -------- Swap --------
    void swap(multi_index_container& other) noexcept
    {
        m_storage.swap(other.m_storage);
        // Note: Indices are tied to storage, need to update pointers
        m_indices.init_storage(&m_storage);
        other.m_indices.init_storage(&other.m_storage);
    }
    
private:
    template<typename Tag, size_t N>
    auto& get_impl() noexcept
    {
        if constexpr (std::is_same_v<typename std::tuple_element_t<N, decltype(m_indices.indices)>::tag_type, Tag>)
        {
            return std::get<N>(m_indices.indices);
        }
        else
        {
            return get_impl<Tag, N + 1>();
        }
    }
    
    template<typename Tag, size_t N>
    const auto& get_impl() const noexcept
    {
        if constexpr (std::is_same_v<typename std::tuple_element_t<N, decltype(m_indices.indices)>::tag_type, Tag>)
        {
            return std::get<N>(m_indices.indices);
        }
        else
        {
            return get_impl<Tag, N + 1>();
        }
    }
};

// ============================================================================
// INDEX TYPE SPECIALIZATIONS (for Boost-like interface)
// ============================================================================

// Generic specialization - should not be used
template<typename Container, typename Tag>
struct container_index {};

// Specialization for multi_index_container types
// This needs to be defined after multi_index_container is complete
template<typename T, typename IndexedBy, typename Tag>
struct container_index<multi_index_container<T, IndexedBy>, Tag>;

// Helper to get the actual type from get<Tag>()
namespace {
    template<typename Container, typename Tag>
    auto get_index_type(Container&) -> decltype(std::declval<Container>().template get<Tag>());
}

// Properly specialize container_index
template<typename T, typename IndexedBy, typename Tag>
struct container_index<multi_index_container<T, IndexedBy>, Tag>
{
    using type = std::remove_reference_t<std::remove_const_t<decltype(get_index_type<multi_index_container<T, IndexedBy>, Tag>(std::declval<multi_index_container<T, IndexedBy>&>()))>>;
};

} // namespace multi_index
} // namespace st

