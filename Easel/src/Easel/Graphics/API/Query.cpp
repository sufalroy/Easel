#include "Precompiled.h"
#include "Easel/Graphics/API/Query.h"

#ifdef EASEL_RENDER_API_OPENGL
#include "Platform/OpenGL/GLQuery.h"
#endif // EASEL_RENDER_API_OPENGL

#include "Easel/Graphics/API/GraphicsContext.h"

namespace Easel {

	namespace Graphics {

		Query* (*Query::CreateFunc)(QueryType) = nullptr;

		Query* Query::Create(QueryType type) {
			EASEL_ASSERT(CreateFunc, "No Query Create Function");
			return CreateFunc(type);
		}
	}
}

