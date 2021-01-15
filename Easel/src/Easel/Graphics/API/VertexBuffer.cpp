#include "Precompiled.h"
#include "Easel/Graphics/API/VertexBuffer.h"

namespace Easel {

	namespace Graphics {

		VertexBuffer* (*VertexBuffer::CreateFunc)(const BufferUsage&) = nullptr;
		
		VertexBuffer* VertexBuffer::Create(const BufferUsage& usage) {
			EASEL_ASSERT(CreateFunc, "No VertexBuffer Create Function");
			return CreateFunc(usage);
		}
	}
}
