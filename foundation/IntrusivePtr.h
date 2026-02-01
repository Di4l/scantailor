/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C)  Joseph Artsimovich <joseph.artsimovich@gmail.com>

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

#ifndef INTRUSIVEPTR_H_
#define INTRUSIVEPTR_H_

#include <utility>
#include <type_traits>
#include <compare>

/**
 * \brief Intrusive smart pointer with reference counting.
 * 
 * Manages lifetime of objects derived from RefCountable.
 * The reference counter is stored IN the object (intrusive), providing
 * excellent cache locality - ideal for GUI patterns and tight loops.
 * 
 * Usage:
 *   IntrusivePtr<MyObject> ptr(new MyObject);
 *   if (ptr) { ... }           // Safe boolean context
 *   ptr->method();             // Arrow operator
 *   *ptr;                      // Dereference
 *   IntrusivePtr<Base> b = ptr; // Implicit conversion of compatible types
 */
template<typename T>
class IntrusivePtr
{
public:
	// ============ Constructors & Destructors ============
	
	constexpr IntrusivePtr() noexcept : m_pObj(nullptr) {}
	
	explicit IntrusivePtr(T* obj) noexcept : m_pObj(obj)
	{
		if (m_pObj) {
			intrusive_ref(*m_pObj);
		}
	}
	
	// Copy constructor
	IntrusivePtr(IntrusivePtr const& other) noexcept : m_pObj(other.m_pObj)
	{
		if (m_pObj) {
			intrusive_ref(*m_pObj);
		}
	}
	
	// Converting copy constructor (for derived types)
	template<typename OT>
	IntrusivePtr(IntrusivePtr<OT> const& other) noexcept : m_pObj(other.get())
	{
		if (m_pObj) {
			intrusive_ref(*m_pObj);
		}
	}
	
	// Move constructor
	IntrusivePtr(IntrusivePtr&& other) noexcept : m_pObj(other.release())
	{
	}
	
	// Converting move constructor
	template<typename OT>
	IntrusivePtr(IntrusivePtr<OT>&& other) noexcept : m_pObj(other.release())
	{
	}
	
	~IntrusivePtr() noexcept
	{
		if (m_pObj) {
			intrusive_unref(*m_pObj);
		}
	}
	
	// ============ Assignment ============
	
	IntrusivePtr& operator=(IntrusivePtr const& rhs) noexcept
	{
		IntrusivePtr(rhs).swap(*this);
		return *this;
	}
	
	template<typename OT>
	IntrusivePtr& operator=(IntrusivePtr<OT> const& rhs) noexcept
	{
		IntrusivePtr(rhs).swap(*this);
		return *this;
	}
	
	IntrusivePtr& operator=(IntrusivePtr&& rhs) noexcept
	{
		reset(rhs.release());
		return *this;
	}
	
	template<typename OT>
	IntrusivePtr& operator=(IntrusivePtr<OT>&& rhs) noexcept
	{
		reset(rhs.release());
		return *this;
	}
	
	// ============ Access ============
	
	T& operator*() const noexcept { return *m_pObj; }
	
	T* operator->() const noexcept { return m_pObj; }
	
	T* get() const noexcept { return m_pObj; }
	
	// ============ Modifiers ============
	
	void reset(T* obj = nullptr) noexcept
	{
		IntrusivePtr(obj).swap(*this);
	}
	
	T* release() noexcept
	{
		T* obj = m_pObj;
		m_pObj = nullptr;
		return obj;
	}
	
	void swap(IntrusivePtr& other) noexcept
	{
		T* tmp = m_pObj;
		m_pObj = other.m_pObj;
		other.m_pObj = tmp;
	}
	
	// ============ Comparisons ============
	
	// Safe boolean context (C++11 explicit operator bool)
	explicit operator bool() const noexcept { return m_pObj != nullptr; }
	
	bool operator!() const noexcept { return m_pObj == nullptr; }
	
	// Equality
	friend bool operator==(IntrusivePtr const& lhs, IntrusivePtr const& rhs) noexcept
	{
		return lhs.get() == rhs.get();
	}
	
	friend bool operator!=(IntrusivePtr const& lhs, IntrusivePtr const& rhs) noexcept
	{
		return lhs.get() != rhs.get();
	}
	
	// Relational
	friend bool operator<(IntrusivePtr const& lhs, IntrusivePtr const& rhs) noexcept
	{
		return lhs.get() < rhs.get();
	}
	
	friend bool operator>(IntrusivePtr const& lhs, IntrusivePtr const& rhs) noexcept
	{
		return lhs.get() > rhs.get();
	}
	
	friend bool operator<=(IntrusivePtr const& lhs, IntrusivePtr const& rhs) noexcept
	{
		return lhs.get() <= rhs.get();
	}
	
	friend bool operator>=(IntrusivePtr const& lhs, IntrusivePtr const& rhs) noexcept
	{
		return lhs.get() >= rhs.get();
	}
	
	// Three-way comparison (C++20)
	friend auto operator<=>(IntrusivePtr const& lhs, IntrusivePtr const& rhs) noexcept
	{
		return lhs.get() <=> rhs.get();
	}

private:
	T* m_pObj;
	
	template<typename OT>
	friend class IntrusivePtr;
};

/**
 * \brief Default intrusive reference increment.
 * 
 * Specialize this function to customize ref counting for specific types.
 */
template<typename T>
inline void intrusive_ref(T& obj) noexcept
{
	obj.ref();
}

/**
 * \brief Default intrusive reference decrement.
 * 
 * Specialize this function to customize ref counting for specific types.
 */
template<typename T>
inline void intrusive_unref(T& obj) noexcept
{
	obj.unref();
}

/**
 * \brief Swap two IntrusivePtr objects.
 */
template<typename T>
inline void swap(IntrusivePtr<T>& lhs, IntrusivePtr<T>& rhs) noexcept
{
	lhs.swap(rhs);
}

#endif
