#include "Precompiled.h"
#include "Easel/Graphics/API/BufferLayout.h"

#include "Easel/Graphics/API/GraphicsContext.h"

#ifdef EASEL_RENDER_API_OPENGL
#include "Platform/OpenGL/GLTypes.h"
#endif // EASEL_RENDER_API_OPENGL

namespace Easel {

	namespace Graphics {

		BufferLayout::BufferLayout() 
			: m_Size(0) {

		}

		void BufferLayout::Push(const std::string& name, Format format, uint32_t size, bool normalized) {
			
			m_Layout.push_back({name, format, m_Size, normalized});
			m_Size += size;
		}

		template<>
		void BufferLayout::Push<uint32_t>(const std::string& name, bool normalized) {

			Push(name, Format::R32_UINT, sizeof(uint32_t), normalized);
		}

		template<>
		void BufferLayout::Push<uint8_t>(const std::string& name, bool normalized) {

			Push(name, Format::R8_UINT, sizeof(uint8_t), normalized);
		}

		template<>
		void BufferLayout::Push<float>(const std::string& name, bool normalized) {
			
			Push(name, Format::R32_FLOAT, sizeof(float), normalized);
		}

		template<>
		void BufferLayout::Push<glm::vec2>(const std::string& name, bool normalized) {
			
			Push(name, Format::R32G32_FLOAT, sizeof(glm::vec2), normalized);
		}

		template<>
		void BufferLayout::Push<glm::vec3>(const std::string& name, bool normalized) {

			Push(name, Format::R32G32B32_FLOAT, sizeof(glm::vec3), normalized);
		}

		template<>
		void BufferLayout::Push<glm::vec4>(const std::string& name, bool normalized) {

			Push(name, Format::R32G32B32A32_FLOAT, sizeof(glm::vec4), normalized);
		}
	}
}

