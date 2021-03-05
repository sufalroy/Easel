#pragma once

#include "Easel/Graphics/API/Renderer.h"
#include "Easel/Graphics/API/Pipeline.h"
#include "Easel/Core/OS/Window.h"
#include "Easel/Maths/Maths.h"
#include "GLSwapchain.h"

namespace Easel {

	class Window;
	namespace Graphics {

		class GLContext;
		class CommandBuffer;
		class Shader;

		class EASEL_EXPORT GLRenderer : public Renderer {
		public:
			friend class Window;
			GLRenderer(uint32_t width, uint32_t height);
			~GLRenderer();

			static GLRenderer* Instance() {
				return static_cast<GLRenderer*>(s_Instance);
			}

			void Begin() override;
			void InitInternal() override;

			void BindDescriptorSetsInternal(Pipeline* pipeline, CommandBuffer* cmdBuffer, uint32_t dynamicOffset, std::vector<DescriptorSet*>& descriptorSets) override;
			void DrawInternal(CommandBuffer* commandBuffer, DrawType type, uint32_t count, DataType dataType, void* indices) const override;
			void DrawIndexedInternal(CommandBuffer* commandBuffer, DrawType type, uint32_t count, uint32_t start) const override;
			void SetRenderModeInternal(RenderMode mode);
			void OnResize(uint32_t width, uint32_t height) override;
			void PresentInternal() override;
			void PresentInternal(CommandBuffer* cmdBuffer) override;
			void SetDepthTestingInternal(bool enabled);
			void SetBlendInternal(bool enabled);
			void SetStencilTestInternal(bool enabled);
			void SetCullingInternal(bool enabled, bool front);
			void SetDepthMaskInternal(bool enabled);
			void SetViewportInternal(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
			void SetPixelPackType(PixelPackType type);

			void SetColourMaskInternal(bool r, bool g, bool b, bool a);

			void SetBlendFunctionInternal(RendererBlendFunction source, RendererBlendFunction destination);
			void SetBlendEquationInternal(RendererBlendFunction blendEquation);
			void SetStencilFunctionInternal(StencilType type, uint32_t ref, uint32_t mask);
			void SetStencilOpInternal(StencilType fail, StencilType zfail, StencilType zpass);

			static void ClearInternal(uint32_t buffer);

			Swapchain* GetSwapchainInternal() const override { return m_Swapchain; }

			const std::string& GetTitleInternal() const override;

			static void MakeDefault();

		protected:
			static Renderer* CreateFuncGL(uint32_t width, uint32_t height);

			std::string m_RendererTitle;
			GLContext* m_Context;
			GLSwapchain* m_Swapchain;
		};
	}
}
