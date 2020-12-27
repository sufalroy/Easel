#include "Precompiled.h"
#include "Easel/Graphics/API/GraphicsContext.h"

#ifdef EASEL_RENDER_API_OPENGL
#include "Platform/OpenGL/GLContext.h"
#include "Platform/OpenGL/GLFunctions.h"
#endif // EASEL_RENDER_API_OPENGL

namespace Easel {

	namespace Graphics {

		GraphicsContext* (*GraphicsContext::CreateFunc)(const WindowProperties&, Window*) = nullptr;

		GraphicsContext* GraphicsContext::s_Context = nullptr;
		RenderAPI GraphicsContext::s_RenderAPI;

		void GraphicsContext::Create(const WindowProperties& properties, Window* window) {
			EASEL_ASSERT(CreateFunc, "No GraphicsContext Create Function");

			s_Context = CreateFunc(properties, window);
		}

		void GraphicsContext::Release() {
			delete s_Context;
		}

		GraphicsContext::~GraphicsContext() {

		}

		void GraphicsContext::SetRenderAPI(RenderAPI api) {

			s_RenderAPI = api;

			switch (s_RenderAPI) {

#ifdef EASEL_RENDER_API_OPENGL
			case RenderAPI::OPENGL:
				Graphics::GL::MakeDefault();
				break;
#endif // EASEL_RENDER_API_OPENGL

#ifdef EASEL_RENDER_API_VULKAN
			case RenderAPI::VULKAN:
				Graphics::Vulkan::MakeDefault();
				break;
#endif // EASEL_RENDER_API_VULKAN

#ifdef EASEL_RENDER_API_DIRECT3D
			case RenderAPI::DIRECT3D:
				Graphics::DIRECT3D::MakeDefault();
				break;
#endif // EASEL_RENDER_API_DIRECT3D

			default: break;
			}
		}
	}
}
