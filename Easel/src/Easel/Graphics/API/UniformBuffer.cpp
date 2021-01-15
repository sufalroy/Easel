#include "Precompiled.h"
#include "Easel/Graphics/API/UniformBuffer.h"

namespace Easel {

	namespace Graphics {

		UniformBuffer* (*UniformBuffer::CreateFunc)() = nullptr;
		UniformBuffer* (*UniformBuffer::CreateDataFunc)(uint32_t, const void*) = nullptr;

		UniformBuffer* UniformBuffer::Create() {

			EASEL_ASSERT(CreateFunc, "No UniformBuffer Create Function");

			return CreateFunc();
		}

		UniformBuffer* UniformBuffer::Create(uint32_t size, const void* data)
		{
			EASEL_ASSERT(CreateFunc, "No UniformBuffer Create Function");

			return CreateDataFunc(size, data);
		}
	}
}

