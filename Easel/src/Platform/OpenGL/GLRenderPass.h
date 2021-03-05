#pragma once

#include "Easel/Graphics/API/RenderPass.h"
#include "Easel/Maths/Maths.h"

namespace Easel {

	namespace Graphics {

		class GLRenderPass : public RenderPass {
		public:
			GLRenderPass(const RenderPassInfo& renderpassCI);
			~GLRenderPass();

			bool Init(const RenderPassInfo& renderpassCI);
			void BeginRenderpass(CommandBuffer* commandBuffer, const Maths::Vector4& clearColour, Framebuffer* frame, SubPassContents contents, uint32_t width, uint32_t height, bool beginCommandBuffer = true) const override;
			void EndRenderpass(CommandBuffer* commandBuffer, bool endCommandBuffer = true) override;
			int GetAttachmentCount() const override { return m_ClearCount; };

			static void MakeDefault();
		protected:
			static RenderPass* CreateFuncGL(const RenderPassInfo& renderpassCI);
		private:
			bool m_Clear = true;
			int m_ClearCount = 0;
		};
	}
}
