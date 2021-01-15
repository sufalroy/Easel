#include "Precompiled.h"
#include "Easel/Graphics/API/CommandBuffer.h"

namespace Easel {

	namespace Graphics {

		CommandBuffer* (*CommandBuffer::CreateFunc)() = nullptr;

		CommandBuffer* CommandBuffer::Create() {
			EASEL_ASSERT(CreateFunc, "No CommandBuffer Create Function");
			return CreateFunc();
		}
	}
}

