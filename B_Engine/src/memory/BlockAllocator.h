#pragma once

#include <vector>
#include <utility>
#include <new> // Required for placement new
#include "../debug/Debug.h"

namespace Engine
{
	/// @defgroup Memory Memory Management
	/// @brief Custom allocators and memory pools.
	///
	/// A generic paged memory pool that allocates objects in contiguous blocks.
	/// Prevents pointer invalidation, stops memory fragmentation, and vastly improves CPU cache locality.
	/// @ingroup Memory
	template <typename T>
	class BlockAllocator
	{
	public:

		BlockAllocator(const BlockAllocator&) = delete;
		BlockAllocator& operator=(const BlockAllocator&) = delete;
		BlockAllocator(BlockAllocator&&) = delete;
		BlockAllocator& operator=(BlockAllocator&&) = delete;

		/// Allocates and constructs a new object of type T in contiguous memory.
		/// Perfectly forwards any arguments to T's constructor.
		template <typename... Args>
		T* Allocate(Args&&... args)
		{
			// 1. Try to recycle previously freed memory (O(1))
			if (!freeList.empty())
			{
				T* ptr = freeList.back();
				freeList.pop_back();
				// Placement new: Constructs the object directly into the recycled memory address
				return new(ptr) T(std::forward<Args>(args)...);
			}

			// 2. If the current page is full, allocate a new page
			if (currentOffset >= pageSize)
			{
				AllocateNewPage(pageSize);
			}

			// 3. Hand out the next available slot in the current page (O(1))
			T* ptr = pages.back() + currentOffset;
			currentOffset++;
			return new(ptr) T(std::forward<Args>(args)...);
		}

		/// Destroys the object and marks its memory slot as available for reuse.
		void Free(T* ptr)
		{
			if (ptr == nullptr) return;

			// Explicitly call the destructor to clean up resources (like components inside a Node)
			ptr->~T();

			// Add to the recycle bin
			freeList.push_back(ptr);
		}

	private:

		// Restricted creation to the Application core.
		friend class Application;
		friend class ComponentRegistry;

		/// Creates a new paged memory allocator.
//		/ @param initialCapacity The size of each memory page (chunk).
		BlockAllocator(size_t initialCapacity = 1000)
			: pageSize(initialCapacity), currentOffset(0)
		{
			// Custom size override warning
			if (initialCapacity != 1000)
			{
				ENGINE_WARN("BlockAllocator initialized with custom size: {}. Advanced usage: ensure this matches your scene bounds.", initialCapacity);
			}
			AllocateNewPage(pageSize);
		}

		~BlockAllocator()
		{
			// Note: This assumes all active objects have been gracefully Destroyed/Freed 
			// by the SceneManager before the pool itself is destroyed.
			for (T* page : pages)
			{
				::operator delete(page);
			}

			ENGINE_LOG("DELETED MEMORY BLOCK");
		}

		std::vector<T*> pages;      // Holds pointers to the raw memory blocks
		std::vector<T*> freeList;   // Holds pointers to recycled slots
		size_t pageSize;            // Number of elements per page
		size_t currentOffset;       // Next available uninitialized slot in the current page

		void AllocateNewPage(size_t capacity)
		{
			// ::operator new allocates raw memory without calling default constructors.
			// This is crucial because we want to delay construction until Allocate() is called.
			T* rawMemory = static_cast<T*>(::operator new(capacity * sizeof(T)));
			pages.push_back(rawMemory);
			currentOffset = 0;
		}
	};
}