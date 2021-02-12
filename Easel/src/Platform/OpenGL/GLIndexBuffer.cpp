#include "Precompiled.h"
#include "GLIndexBuffer.h"

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

		GLIndexBuffer::GLIndexBuffer(uint16_t* data, uint32_t count, BufferUsage bufferUsage)
			: m_Count(count)
			, m_Usage(bufferUsage)
		{
			EASEL_PROFILE_FUNCTION();
			GLCall(glGenBuffers(1, &m_Handle));
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle));
			GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint16_t), data, BufferUsageToOpenGL(m_Usage)));
		}

		GLIndexBuffer::GLIndexBuffer(uint32_t* data, uint32_t count, BufferUsage bufferUsage)
			: m_Count(count)
			, m_Usage(bufferUsage)
		{
			EASEL_PROFILE_FUNCTION();
			GLCall(glGenBuffers(1, &m_Handle));
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle));
			GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, BufferUsageToOpenGL(m_Usage)));
		}

		GLIndexBuffer::~GLIndexBuffer() {
			EASEL_PROFILE_FUNCTION();
			GLCall(glDeleteBuffers(1, &m_Handle));
		}

		void GLIndexBuffer::Bind(CommandBuffer* commandBuffer) const {
			EASEL_PROFILE_FUNCTION();
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Handle));
		}

		void GLIndexBuffer::Unbind() const {
			EASEL_PROFILE_FUNCTION();
			GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		}

		void GLIndexBuffer::MakeDefault() {
			CreateFunc = CreateFuncGL;
			Create16Func = CreateFunc16GL;
		}

		IndexBuffer* GLIndexBuffer::CreateFuncGL(uint32_t* data, uint32_t count, BufferUsage bufferUsage) {
			return new GLIndexBuffer(data, count, bufferUsage);
		}

		IndexBuffer* GLIndexBuffer::CreateFunc16GL(uint16_t* data, uint32_t count, BufferUsage bufferUsage) {
			return new GLIndexBuffer(data, count, bufferUsage);
		}
	}
}

