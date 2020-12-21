#include "Precompiled.h"
#include "Easel/Core/OS/Memory.h"
#include "Easel/Core/OS/Allocators/BinAllocator.h"
#include "Easel/Core/OS/Allocators/DefaultAllocator.h"
#include "Easel/Core/OS/Allocators/StbAllocator.h"

namespace Easel {

	Allocator* const Memory::MemoryAllocator = new DefaultAllocator();

	void* Memory::AlignedAlloc(size_t size, size_t alignment) {
		void* data;
#if defined(EASEL_PLATFORM_WINDOWS)
		data = _aligned_malloc(size, alignment);
#else
		int res = posix_memalign(&data, alignment, size);
		if (res != 0)
			data = nullptr;
#endif // defined(EASEL_PLATFORM_WINDOWS)

		return data;
	}

	void Memory::AlignedFree(void* data) {
#if defined(EASEL_PLATFORM_WINDOWS)
		_aligned_free(data);
#else
		free(data);
#endif // defined(EASEL_PLATFORM_WINDOWS)

	}

	void* Memory::NewFunc(std::size_t size, const char* file, int line) {
		if (MemoryAllocator)
			return MemoryAllocator->Malloc(size, file, line);
		else
			return malloc(size);
	}

	void Memory::DeleteFunc(void* p) {
		if (MemoryAllocator)
			return MemoryAllocator->Free(p);
		else
			return free(p);
	}

	void Memory::LogMemoryInformation() {
		if (MemoryAllocator)
			return MemoryAllocator->Print();
	}
}

#ifdef CUSTOM_MEMORY_ALLOCATOR

	void* operator new(std::size_t size)
	{
		void* result = Easel::Memory::NewFunc(size, __FILE__, __LINE__);
		if (result == nullptr)
		{
			throw std::bad_alloc();
		}
	
		return result;
	}
	
	void* operator new[](std::size_t size)
	{
		void* result = Easel::Memory::NewFunc(size, __FILE__, __LINE__);
		if (result == nullptr)
		{
			throw std::bad_alloc();
		}

		return result;
	}
	
	void operator delete(void* p) throw()
	{
		Easel::Memory::DeleteFunc(p);
	}
	
	void operator delete[](void* p) throw()
	{
		Easel::Memory::DeleteFunc(p);
	}
#endif
