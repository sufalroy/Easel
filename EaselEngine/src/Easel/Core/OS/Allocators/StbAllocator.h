#pragma once
#include "Easel/Core/OS/Allocators/Allocator.h"

namespace Easel {
	
	class StbAllocator : public Allocator {
	public:
		void* Malloc(size_t size, const char* file, int line) override;
		void Free(void* location) override;
		void Print() override;
	};
}
