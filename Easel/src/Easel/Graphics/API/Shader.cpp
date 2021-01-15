#include "Precompiled.h"
#include "Easel/Graphics/API/Shader.h"

#include "Easel/Graphics/API/GraphicsContext.h"
#include "Easel/Core/VFS.h"

namespace Easel {

	namespace Graphics {

		Shader* (*Shader::CreateFunc)(const std::string&) = nullptr;

		const Shader* Shader::s_CurrentlyBound = nullptr;

		Shader* Shader::CreateFromFile(const std::string& filepath) {
			EASEL_ASSERT(CreateFunc, "No Shader Create Function");
			return CreateFunc(filepath);
		}

	}
}
