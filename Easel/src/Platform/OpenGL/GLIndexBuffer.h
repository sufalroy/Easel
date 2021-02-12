#pragma once

#include "Easel/Graphics/API/IndexBuffer.h"
#include "GLDebug.h"

namespace Easel {

	namespace Graphics {

		class GLIndexBuffer : public IndexBuffer {
		public:
			GLIndexBuffer(uint16_t* data, uint32_t count, BufferUsage bufferUsage);
			GLIndexBuffer(uint32_t* data, uint32_t count, BufferUsage bufferUsage);
			~GLIndexBuffer();

			void Bind(CommandBuffer* commandBuffer) const override;
			void Unbind() const override;
			uint32_t GetCount() const override { return m_Count; };
			void SetCount(uint32_t m_index_count) override { m_Count = m_index_count; }

			static void MakeDefault();
		protected:
			static IndexBuffer* CreateFuncGL(uint32_t* data, uint32_t count, BufferUsage bufferUsage);
			static IndexBuffer* CreateFunc16GL(uint16_t* data, uint32_t count, BufferUsage bufferUsage);
		private:
			uint32_t m_Handle;
			uint32_t m_Count;
			BufferUsage m_Usage;
		};
	}
}
