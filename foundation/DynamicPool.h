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

#ifndef DYNAMIC_POOL_H_
#define DYNAMIC_POOL_H_

#include "NonCopyable.h"
#include <list>
#include <memory>
#include <stddef.h>

/**
 * \brief Allocates objects from the heap.
 *
 * There is no way of freeing the allocated objects
 * besides destroying the whole pool.
 */
template<typename T>
class DynamicPool
{
	DECLARE_NON_COPYABLE(DynamicPool)
public:
	DynamicPool() {}

	~DynamicPool();

	/**
	 * \brief Allocates a sequence of objects.
	 *
	 * If T is a POD type, the returned objects are uninitialized,
	 * otherwise they are default-constructed.
	 */
	T* alloc(size_t num_elements);
private:
	enum { OVERALLOCATION_FACTOR = 3 }; /**< Allocate 3 times the requested size. */
	enum { OVERALLOCATION_LIMIT = 256 }; /**< Don't overallocate too much. */
	
	// NOTE: Previously used boost::intrusive::list for zero-copy node storage.
	// Can be re-implemented as intrusive_list in future for performance optimization.
	struct Chunk
	{
		std::unique_ptr<T[]> storage;
		T* pData;
		size_t remainingElements;

		Chunk() : pData(0), remainingElements(0) {}

		void init(std::unique_ptr<T[]>& data, size_t size) {
			storage = std::move(data);
			pData = storage.get();
			remainingElements = size;
		}
	};

	// NOTE: Was boost::intrusive::list. Now using std::list with std::unique_ptr.
	// If performance becomes critical, consider implementing as intrusive_list again.
	typedef std::list<std::unique_ptr<Chunk>> ChunkList;

	static size_t adviseChunkSize(size_t num_elements);

	ChunkList m_chunkList;
};


template<typename T>
DynamicPool<T>::~DynamicPool()
{
	// NOTE: unique_ptr destruye automaticamente los chunks
}

template<typename T>
T*
DynamicPool<T>::alloc(size_t num_elements)
{	
	Chunk* chunk = nullptr;

	if (!m_chunkList.empty()) {
		chunk = m_chunkList.back().get();
		if (chunk->remainingElements < num_elements) {
			chunk = nullptr;
		}
	}

	if (!chunk) {
		// Create a new chunk.
		size_t const chunk_size = adviseChunkSize(num_elements);
		std::unique_ptr<T[]> data(new T[chunk_size]);
		std::unique_ptr<Chunk> new_chunk = std::make_unique<Chunk>();
		Chunk* chunk_ptr = new_chunk.get();
		m_chunkList.push_back(std::move(new_chunk));
		chunk_ptr->init(data, chunk_size);
		chunk = chunk_ptr;
	}

	// Allocate from chunk.
	T* data = chunk->pData;
	chunk->pData += num_elements;
	chunk->remainingElements -= num_elements;
	return data;
}

template<typename T>
size_t
DynamicPool<T>::adviseChunkSize(size_t num_elements)
{
	size_t factor = OVERALLOCATION_LIMIT / num_elements;
	if (factor > (size_t)OVERALLOCATION_FACTOR) {
		factor = OVERALLOCATION_FACTOR;
	}

	return num_elements * (factor + 1);
}

#endif
