#include "Precompiled.h"
#include "GLSwapchain.h"
#include "Easel/Graphics/API/Framebuffer.h"
#include "GLTexture.h"
#include "Easel/Core/OS/Window.h"

namespace Easel {

	namespace Graphics {

		GLSwapchain::GLSwapchain(uint32_t width, uint32_t height) {
			FramebufferInfo info{};
			info.width = width;
			info.height = height;
			info.attachments = nullptr;
		}

		GLSwapchain::~GLSwapchain() {
			for (auto& buffer : swapChainBuffers)
				delete buffer;
		}

		bool GLSwapchain::Init(bool vsync) {
			return true;
		}

		Texture* GLSwapchain::GetCurrentImage() {
			return nullptr; // swapChainBuffers[0];
		}

		uint32_t GLSwapchain::GetCurrentBufferId() const {
			return 0;
		}

		size_t GLSwapchain::GetSwapchainBufferCount() const {
			return 1;
		}

		void GLSwapchain::MakeDefault() {
			CreateFunc = CreateFuncGL;
		}

		Swapchain* GLSwapchain::CreateFuncGL(uint32_t width, uint32_t height) {
			return new GLSwapchain(width, height);
		}
	}
}
