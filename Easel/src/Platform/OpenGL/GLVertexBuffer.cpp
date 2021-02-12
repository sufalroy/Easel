#include "Precompiled.h"
#include "GLVertexBuffer.h"
#include "GLPipeline.h"

#include "GL.h"

namespace Easel {

	namespace Graphics {

		static uint32_t BufferUsageToOpenGL(const BufferUsage usage) {
			switch (usage) {
			case BufferUsage::STATIC: return GL_STATIC_DRAW;
			case BufferUsage::DYNAMIC: return GL_DYNAMIC_DRAW;
			case BufferUsage::STREAM: return GL_STREAM_DRAW;
			}
			return 0;
		}

		GLVertexBuffer::GLVertexBuffer(BufferUsage usage)
			: m_Usage(usage)
			, m_Size(0)
		{
			EASEL_PROFILE_FUNCTION();
			GLCall(glGenBuffers(1, &m_Handle));
		}

		GLVertexBuffer::~GLVertexBuffer() {
			EASEL_PROFILE_FUNCTION();
			GLCall(glDeleteBuffers(1, &m_Handle));
		}

		void GLVertexBuffer::Resize(uint32_t size) {
			EASEL_PROFILE_FUNCTION();
			m_Size = size;

			GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_Handle));
			GLCall(glBufferData(GL_ARRAY_BUFFER, size, NULL, BufferUsageToOpenGL(m_Usage)));
		}

		void GLVertexBuffer::SetData(uint32_t size, const void* data) {
			EASEL_PROFILE_FUNCTION();
			m_Size = size;
			GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_Handle));
			GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, BufferUsageToOpenGL(m_Usage)));
		}

		void GLVertexBuffer::SetDataSub(uint32_t size, const void* data, uint32_t offset) {
			EASEL_PROFILE_FUNCTION();
			m_Size = size;
			GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_Handle));
			GLCall(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
		}

		void* GLVertexBuffer::GetPointerInternal() {
			EASEL_PROFILE_FUNCTION();
			void* result = nullptr;
			if (!m_Mapped) {
				GLCall(result = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
				m_Mapped = true;
			}
			else {
				EASEL_CORE_WARN("Vertex buffer already mapped");
			}

			return result;
		}

		void GLVertexBuffer::ReleasePointer() {
			EASEL_PROFILE_FUNCTION();
			if (m_Mapped) {
				GLCall(glUnmapBuffer(GL_ARRAY_BUFFER));
				m_Mapped = false;
			}
		}

		void GLVertexBuffer::Bind(CommandBuffer* commandBuffer, Pipeline* pipeline) {
			EASEL_PROFILE_FUNCTION();
			GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_Handle));
			((GLPipeline*)pipeline)->BindVertexArray();
		}

		void GLVertexBuffer::Unbind() {
			EASEL_PROFILE_FUNCTION();
			GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
		}

		void GLVertexBuffer::MakeDefault() {
			CreateFunc = CreateFuncGL;
		}

		VertexBuffer* GLVertexBuffer::CreateFuncGL(const BufferUsage& usage) {
			return new GLVertexBuffer(usage);
		}
	}
}
