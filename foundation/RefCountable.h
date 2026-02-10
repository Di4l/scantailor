/*
    Scan Tailor - Interactive post-processing tool for scanned pages.
    Copyright (C) 2007-2008  Joseph Artsimovich <joseph_a@mail.ru>

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

#ifndef REFCOUNTABLE_H_
#define REFCOUNTABLE_H_

#include <atomic>
#include <memory>

/**
 * \brief Reference-counted object base class using C++23 atomics.
 * 
 * Objects inheriting from this class can be managed by IntrusivePtr
 * with automatic memory management when reference count reaches zero.
 * 
 * The reference counter is stored INSIDE the object (intrusive), not in
 * a separate control block. This provides excellent cache locality and
 * is ideal for GUI patterns with frequent ref/unref operations.
 */
class RefCountable
{
public:
	RefCountable() noexcept = default;//: m_refCounter(0) {}
	
	// Non-copyable: reference counter is not shared between instances
	RefCountable(RefCountable const&) noexcept {}
	RefCountable& operator=(RefCountable const&) noexcept { return *this; }
	
	/**
	 * Increment reference count.
	 * Called by IntrusivePtr when taking ownership.
	 */
	void ref() const noexcept
	{
		m_refCounter.fetch_add(1, std::memory_order_relaxed);
	}
	
	/**
	 * Decrement reference count.
	 * When count reaches zero, deletes this object automatically.
	 * Called by IntrusivePtr when releasing ownership.
	 */
	void unref() const noexcept
	{
		int const prev = m_refCounter.fetch_sub(1, std::memory_order_release);
		if (prev == 1 && !m_beingDestroyed.exchange(true, std::memory_order_acq_rel))
		{
			std::atomic_thread_fence(std::memory_order_acquire);
			delete this;
		}
	}

protected:
	virtual ~RefCountable()
	{
		m_beingDestroyed.store(true, std::memory_order_release);
	}

private:
	mutable std::atomic<int> m_refCounter {0};
	mutable std::atomic<bool> m_beingDestroyed {false};
};

#endif
