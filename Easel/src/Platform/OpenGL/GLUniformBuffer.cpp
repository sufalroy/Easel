#include "Precompiled.h"
#include "GLUniformBuffer.h"
#include "GL.h"
#include "GLDebug.h"
#include "GLShader.h"

namespace Easel
{
	namespace Graphics {

		GLUniformBuffer::GLUniformBuffer() {
			EASEL_PROFILE_FUNCTION();
			glGenBuffers(1, &m_Handle);
		}

		GLUniformBuffer::~GLUniformBuffer() {
			EASEL_PROFILE_FUNCTION();
			GLCall(glDeleteBuffers(1, &m_Handle));
		}

		void GLUniformBuffer::Init(uint32_t size, const void* data) {
			EASEL_PROFILE_FUNCTION();
			m_Data = (uint8_t*)data;
			m_Size = size;
			glBindBuffer(GL_UNIFORM_BUFFER, m_Handle);
			glBufferData(GL_UNIFORM_BUFFER, m_Size, m_Data, GL_DYNAMIC_DRAW);
		}

		void GLUniformBuffer::SetData(uint32_t size, const void* data) {
			EASEL_PROFILE_FUNCTION();
			m_Data = (uint8_t*)data;
			GLvoid* p = nullptr;
			
			glBindBuffer(GL_UNIFORM_BUFFER, m_Handle);
			
			if(size != m_Size) {
				EASEL_PROFILE_SCOPE("glMapBuffer");
				p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
				m_Size = size;
				
				memcpy(p, m_Data, m_Size);
				glUnmapBuffer(GL_UNIFORM_BUFFER);
			}
			else {
				EASEL_PROFILE_SCOPE("glBufferSubData");
				glBufferSubData(GL_UNIFORM_BUFFER, 0, size, m_Data );
			}
		}

		void GLUniformBuffer::SetDynamicData(uint32_t size, uint32_t typeSize, const void* data) {
			EASEL_PROFILE_FUNCTION();
			m_Data = (uint8_t*)data;
			m_Size = size;
			m_Dynamic = true;
			m_DynamicTypeSize = typeSize;
			
			GLvoid* p = nullptr;
			
			glBindBuffer(GL_UNIFORM_BUFFER, m_Handle);
			
			if(size != m_Size) {
				EASEL_PROFILE_SCOPE("glMapBuffer");
				p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
				m_Size = size;
				
				memcpy(p, m_Data, m_Size);
				glUnmapBuffer(GL_UNIFORM_BUFFER);
			}
			else {
				EASEL_PROFILE_SCOPE("glBufferSubData");
				glBufferSubData(GL_UNIFORM_BUFFER, 0, m_Size, m_Data );
			}
		}

		void GLUniformBuffer::Bind(uint32_t slot, GLShader* shader, std::string& name) {
			EASEL_PROFILE_FUNCTION();
			GLCall(glBindBufferBase(GL_UNIFORM_BUFFER, slot, m_Handle));
			shader->BindUniformBuffer(this, slot, name);
			//uint32_t location = glGetUniformBlockIndex(shader->GetHandle(), name.c_str());
			//GLCall(glUniformBlockBinding(shader->GetHandle(), location, slot));
		}

		void GLUniformBuffer::MakeDefault() {
			CreateFunc = CreateFuncGL;
			CreateDataFunc = CreateDataFuncGL;
		}

		UniformBuffer* GLUniformBuffer::CreateDataFuncGL(uint32_t size, const void* data) {
			//TODO
			return new GLUniformBuffer();
		}

		UniformBuffer* GLUniformBuffer::CreateFuncGL() {
			return new GLUniformBuffer();
		}
	}
}
