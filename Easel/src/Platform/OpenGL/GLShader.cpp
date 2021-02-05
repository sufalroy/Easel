#include "Precompiled.h"
#include "GLShader.h"

#include "GL.h"
#include "Easel/Core/VFS.h"
#include "Easel/Core/OS/FileSystem.h"
#include "Easel/Utils/StringUtils.h"

#include <glm/gtc/type_ptr.hpp>

enum root_signature_spaces {
	PUSH_CONSTANT_REGISTER_SPACE = 0,
	DYNAMIC_OFFSET_SPACE,
	DESCRIPTOR_TABLE_INITIAL_SPACE,
};

namespace Easel {

	namespace Graphics
	{
		bool IGNORE_LINES = false;
		static ShaderType s_Type = ShaderType::UNKNOWN;

		GLShader::GLShader(const std::string& filePath, bool loadSPV)
			: m_LoadSPV(loadSPV) {

            m_Name = StringUtils::GetFileName(filePath);
            m_Path = StringUtils::GetFileLocation(filePath);

			m_Source = VFS::Get()->ReadTextFile(filePath);

			Init();
		}

		GLShader::~GLShader() {

			Shutdown();

			for(auto& resource : m_Resources) {

				delete resource;
			}

			for(auto& structs : m_Structs) {

				delete structs;
			}

			for(auto& shader : m_UniformBuffers) {

				for(auto j : shader.second) {

					delete j;
				}
			}

			for(auto& shader : m_UserUniformBuffers) {
				delete shader.second;
			}
		}

		void GLShader::Init() {

			EASEL_PROFILE_FUNCTION();
			std::map<ShaderType, std::string>* sources = new std::map<ShaderType, std::string>();
			PreProcess(m_Source, sources);

			for(auto& file : *sources) {

				auto fileSize = FileSystem::GetFileSize(m_Path + file.second); //TODO: once process
				uint32_t* source = reinterpret_cast<uint32_t*>(FileSystem::ReadFile(m_Path + file.second));
				std::vector<unsigned int> spv(source, source + fileSize / sizeof(unsigned int));

				spirv_cross::CompilerGLSL* glsl = new spirv_cross::CompilerGLSL(std::move(spv));

				// The SPIR-V is now parsed, and we can perform reflection on it.
				spirv_cross::ShaderResources resources = glsl->get_shader_resources();

				// Get all sampled images in the shader.
				for(auto& resource : resources.sampled_images) {

					unsigned set = glsl->get_decoration(resource.id, spv::DecorationDescriptorSet);
					unsigned binding = glsl->get_decoration(resource.id, spv::DecorationBinding);

					// Modify the decoration to prepare it for GLSL.
					glsl->unset_decoration(resource.id, spv::DecorationDescriptorSet);

					// Some arbitrary remapping if we want.
					glsl->set_decoration(resource.id, spv::DecorationBinding, set * 16 + binding);
				}

				for(auto const& image : resources.separate_images) {

					auto set{glsl->get_decoration(image.id, spv::Decoration::DecorationDescriptorSet)};
					glsl->set_decoration(image.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set);
				}
				for(auto const& input : resources.subpass_inputs) {

					auto set{glsl->get_decoration(input.id, spv::Decoration::DecorationDescriptorSet)};
					glsl->set_decoration(input.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set);
				}
				for(auto const& uniform_buffer : resources.uniform_buffers) {

					auto set{glsl->get_decoration(uniform_buffer.id, spv::Decoration::DecorationDescriptorSet)};
					glsl->set_decoration(uniform_buffer.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set);
				}
				for(auto const& storage_buffer : resources.storage_buffers) {

					auto set{glsl->get_decoration(storage_buffer.id, spv::Decoration::DecorationDescriptorSet)};
					glsl->set_decoration(storage_buffer.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set);
				}
				for(auto const& storage_image : resources.storage_images) {

					auto set{glsl->get_decoration(storage_image.id, spv::Decoration::DecorationDescriptorSet)};
					glsl->set_decoration(storage_image.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set);
				}
				for(auto const& image : resources.sampled_images) {

					auto set{glsl->get_decoration(image.id, spv::Decoration::DecorationDescriptorSet)};
					glsl->set_decoration(image.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set); // Sampler offset done in spirv-cross
				}
				for(auto const& sampler : resources.separate_samplers) {

					auto set{glsl->get_decoration(sampler.id, spv::Decoration::DecorationDescriptorSet)};
					glsl->set_decoration(sampler.id, spv::Decoration::DecorationDescriptorSet, DESCRIPTOR_TABLE_INITIAL_SPACE + 2 * set + 1);
				}

				spirv_cross::CompilerGLSL::Options options;
				options.version = 410;
				options.es = false;
				options.vulkan_semantics = false;
				options.separate_shader_objects = false;
				options.enable_420pack_extension = false;
				glsl->set_common_options(options);

				// Compile to GLSL, ready to give to GL driver.
				std::string glslSource = glsl->compile();
				file.second = glslSource;
                
				m_pShaderCompilers.push_back(glsl);
			}

			Parse(sources);

			for(auto& source : *sources) {
				m_ShaderTypes.push_back(source.first);
			}

			GLShaderErrorInfo error;
			m_Handle = Compile(sources, error);

			if(!m_Handle) {

				EASEL_CORE_ERROR("{0} - {1}", error.message[error.shader], m_Name);
			}
			else {

				EASEL_CORE_INFO("Successfully compiled shader: {0}", m_Name);
			}

			//EASEL_ASSERT(m_Handle, "");

			ResolveUniforms();
			ValidateUniforms();
			CreateLocations();

			delete sources;
		}

		void GLShader::Shutdown() const {
			EASEL_PROFILE_FUNCTION();
			GLCall(glDeleteProgram(m_Handle));
		}

		bool GLShader::CreateLocations() {

			EASEL_PROFILE_FUNCTION();
			for(auto& compiler : m_pShaderCompilers) {

				const spirv_cross::ShaderResources shaderResources = compiler->get_shader_resources();

				for(const auto& itUniform : shaderResources.uniform_buffers) {

					if(compiler->get_type(itUniform.type_id).basetype == spirv_cross::SPIRType::Struct) {

						SetUniformLocation(itUniform.name.c_str());
					}
				}
			}
			return true;
		}

		GLuint HashValue(const char* szString) {

			const char* c = szString;
			GLuint dwHashValue = 0x00000000;

			while(*c) {

				dwHashValue = (dwHashValue << 5) - dwHashValue + (*c == '/' ? '\\' : *c);
				c++;
			}

			return dwHashValue ? dwHashValue : 0xffffffff;
		}

		void GLShader::BindUniformBuffer(GLUniformBuffer* buffer, uint32_t slot, const std::string& name) {

			GLuint nameInt = HashValue(name.c_str());
			const auto& itLocation = m_uniformBlockLocations.find(nameInt); //

			//auto loc = glGetUniformBlockIndex(m_Handle, name.c_str());
			GLCall(glUniformBlockBinding(m_Handle, itLocation->second, slot));
		}

		bool GLShader::SetUniformLocation(const char* szName) {
			EASEL_PROFILE_FUNCTION();
			GLuint name = HashValue(szName);

			if(m_uniformBlockLocations.find(name) == m_uniformBlockLocations.end()) {
				GLuint location = glGetUniformBlockIndex(m_Handle, szName);

				if(location != GL_INVALID_INDEX) {
					m_names[name] = szName;
					m_uniformBlockLocations[name] = location;
					glUniformBlockBinding(m_Handle, location, location);
					return true;
				}
			}

			return false;
		}

		void GLShader::PreProcess(const std::string& source, std::map<ShaderType, std::string>* sources) {
			EASEL_PROFILE_FUNCTION();
			s_Type = ShaderType::UNKNOWN;
			std::vector<std::string> lines = StringUtils::GetLines(source);
			ReadShaderFile(lines, sources);
		}

		void GLShader::ReadShaderFile(std::vector<std::string> lines, std::map<ShaderType, std::string>* shaders) {
			EASEL_PROFILE_FUNCTION();
			for(uint32_t i = 0; i < lines.size(); i++) {

				std::string str = std::string(lines[i]);
				str = StringUtils::StringReplace(str, '\t');

				if(IGNORE_LINES) {

					if(StringUtils::StartsWith(str, "#end")) {
						IGNORE_LINES = false;
					}
				}
				else if(StringUtils::StartsWith(str, "#shader")) {

					if(StringUtils::StringContains(str, "vertex")) {
						s_Type = ShaderType::VERTEX;
						std::map<ShaderType, std::string>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, std::string>(s_Type, ""));
					}
					else if(StringUtils::StringContains(str, "geometry")) {
						s_Type = ShaderType::GEOMETRY;
						std::map<ShaderType, std::string>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, std::string>(s_Type, ""));
					}
					else if(StringUtils::StringContains(str, "fragment")) {
						s_Type = ShaderType::FRAGMENT;
						std::map<ShaderType, std::string>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, std::string>(s_Type, ""));
					}
					else if(StringUtils::StringContains(str, "tess_cont")) {
						s_Type = ShaderType::TESSELLATION_CONTROL;
						std::map<ShaderType, std::string>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, std::string>(s_Type, ""));
					}
					else if(StringUtils::StringContains(str, "tess_eval")) {
						s_Type = ShaderType::TESSELLATION_EVALUATION;
						std::map<ShaderType, std::string>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, std::string>(s_Type, ""));
					}
					else if(StringUtils::StringContains(str, "compute")) {
						s_Type = ShaderType::COMPUTE;
						std::map<ShaderType, std::string>::iterator it = shaders->begin();
						shaders->insert(it, std::pair<ShaderType, std::string>(s_Type, ""));
					}
					else if(StringUtils::StringContains(str, "end")) {
						s_Type = ShaderType::UNKNOWN;
					}
				}
				else if(StringUtils::StartsWith(str, "#include")) {
					std::string rem = "#include ";
					std::string file = std::string(str);
					if(strstr(file.c_str(), rem.c_str())) {
						std::string::size_type j = file.find(rem);
						if(j != std::string::npos)
							file.erase(j, rem.length());
						file = StringUtils::StringReplace(file, '\"');
						EASEL_CORE_WARN("Including file \'{0}\' into shader.", file);
						VFS::Get()->ReadTextFile(file);
						ReadShaderFile(StringUtils::GetLines(VFS::Get()->ReadTextFile(file)), shaders);
					}
				}
				else if(StringUtils::StartsWith(str, "#if")) {
					std::string rem = "#if ";
					std::string def = std::string(str);
					if(strstr(def.c_str(), rem.c_str())) {
						std::string::size_type j = def.find(rem);
						if(j != std::string::npos)
							def.erase(j, rem.length());
						def = StringUtils::StringReplace(def, '\"');

						if(def == "0") {
							IGNORE_LINES = true;
						}
					}
				}
				else if(s_Type != ShaderType::UNKNOWN) {
					shaders->at(s_Type).append(lines[i]);
					///shaders->at(s_Type).append("\n");
				}
			}
		}

		uint32_t GLShader::Compile(std::map<ShaderType, std::string>* sources, GLShaderErrorInfo& info) {
			EASEL_PROFILE_FUNCTION();
			GLCall(uint32_t program = glCreateProgram());

			std::vector<GLuint> shaders;

			std::string glVersion;

#ifndef EASEL_PLATFORM_MOBILE
			glVersion = "#version 410 core \n";
#else
			glVersion = "#version 300 es \n precision highp float; \n precision highp int; \n";
#endif
			for(auto source : *sources) {
				//source.second.insert(0, glVersion);
				//EASEL_CORE_INFO(source.second);
				shaders.push_back(CompileShader(source.first, source.second, program, info));
			}

			for(unsigned int shader : shaders)
				glAttachShader(program, shader);

			GLCall(glLinkProgram(program));

			GLint result;
			GLCall(glGetProgramiv(program, GL_LINK_STATUS, &result));
			if(result == GL_FALSE) {
				GLint length;
				GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
				std::vector<char> error(length);
				GLCall(glGetProgramInfoLog(program, length, &length, error.data()));
				std::string errorMessage(error.data(), length);
				int32_t lineNumber = -1;
				sscanf(error.data(), "%*s %*d:%d", &lineNumber);
				info.shader = 3;
				info.message[info.shader] += "Failed to link shader!\n";
				info.line[info.shader] = 0;
				info.message[info.shader] += errorMessage;

				EASEL_CORE_ERROR(info.message[info.shader]);
				return 0;
			}

			GLCall(glValidateProgram(program));

			for(int z = 0; z < shaders.size(); z++)
				glDetachShader(program, shaders[z]);

			for(int z = 0; z < shaders.size(); z++)
				glDeleteShader(shaders[z]);

			return program;
		}

		GLenum TypeToGL(ShaderType type) {

			switch(type) {

			case ShaderType::VERTEX:
				return GL_VERTEX_SHADER;
			case ShaderType::FRAGMENT:
				return GL_FRAGMENT_SHADER;
#ifndef LUMOS_PLATFORM_MOBILE
			case ShaderType::GEOMETRY:
				return GL_GEOMETRY_SHADER;
			case ShaderType::TESSELLATION_CONTROL:
				return GL_TESS_CONTROL_SHADER;
			case ShaderType::TESSELLATION_EVALUATION:
				return GL_TESS_EVALUATION_SHADER;
			case ShaderType::COMPUTE:
				return GL_COMPUTE_SHADER;
#endif
			default:
				EASEL_CORE_ERROR("Unsupported Shader Type");
				return GL_VERTEX_SHADER;
			}
		}

		std::string TypeToString(ShaderType type) {

			switch(type) {

			case ShaderType::VERTEX:
				return "GL_VERTEX_SHADER";
			case ShaderType::FRAGMENT:
				return "GL_FRAGMENT_SHADER";
			case ShaderType::GEOMETRY:
				return "GL_GEOMETRY_SHADER";
			case ShaderType::TESSELLATION_CONTROL:
				return "GL_TESS_CONTROL_SHADER";
			case ShaderType::TESSELLATION_EVALUATION:
				return "GL_TESS_EVALUATION_SHADER";
			case ShaderType::COMPUTE:
				return "GL_COMPUTE_SHADER";
			case ShaderType::UNKNOWN:
				return "UNKOWN_SHADER";
			}
			return "N/A";
		}

		GLuint GLShader::CompileShader(ShaderType type, std::string source, uint32_t program, GLShaderErrorInfo& info) {

			EASEL_PROFILE_FUNCTION();
			const char* cstr = source.c_str();

			GLCall(GLuint shader = glCreateShader(TypeToGL(type)));
			GLCall(glShaderSource(shader, 1, &cstr, NULL));
			GLCall(glCompileShader(shader));

			GLint result;
			GLCall(glGetShaderiv(shader, GL_COMPILE_STATUS, &result));
			if(result == GL_FALSE) {

				GLint length;
				GLCall(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
				std::vector<char> error(length);
				GLCall(glGetShaderInfoLog(shader, length, &length, error.data()));
				std::string errorMessage(error.data(), length);
				int32_t lineNumber;
				sscanf(error.data(), "%*s %*d:%d", &lineNumber);
				info.shader = static_cast<uint32_t>(type);
				info.message[info.shader] += "Failed to compile " + TypeToString(type) + " shader!\n";

				info.line[info.shader] = lineNumber;
				info.message[info.shader] += errorMessage;
				GLCall(glDeleteShader(shader));

				EASEL_CORE_ERROR(info.message[info.shader]);
				return 0;
			}
			return shader;
		}

		void GLShader::Bind() const {
			if(s_CurrentlyBound != this) {
				GLCall(glUseProgram(m_Handle));
				s_CurrentlyBound = this;
			}
		}

		void GLShader::Unbind() const {
			GLCall(glUseProgram(0));
			s_CurrentlyBound = nullptr;
		}

		void GLShader::Parse(std::map<ShaderType, std::string>* sources) {

			for (auto& source : *sources) {

				m_UniformBuffers[source.first].push_back(new GLShaderUniformBufferDeclaration("Global", static_cast<uint32_t>(source.first)));

				const char* token;
				const char* str;

				str = source.second.c_str();
				while ((token = StringUtils::FindToken(str, "struct")))
					ParseUniformStruct(StringUtils::GetBlock(token, &str), source.first);

				str = source.second.c_str();
				while ((token = StringUtils::FindToken(str, "uniform")))
					ParseUniform(StringUtils::GetStatement(token, &str), source.first);
			}
		}

		void GLShader::ParseUniform(const std::string& statement, ShaderType type) {

			EASEL_PROFILE_FUNCTION();
			std::vector<std::string> tokens = StringUtils::Tokenize(statement);
			uint32_t index = 0;

			index++; // "uniform"
			const std::string typeString = tokens[index++];
			std::string name = tokens[index++];
			// Strip ; from name if present
			if (const char* s = strstr(name.c_str(), ";"))
				name = std::string(name.c_str(), s - name.c_str());

			std::string n(name);
			int32_t count = 1;
			const char* namestr = n.c_str();
			if (const char* s = strstr(namestr, "[")) {

				name = std::string(namestr, s - namestr);

				const char* end = strstr(namestr, "]");
				std::string c(s + 1, end - s);
				count = atoi(c.c_str());
			}

			if (IsTypeStringResource(typeString)) {
				ShaderResourceDeclaration* declaration = new GLShaderResourceDeclaration(GLShaderResourceDeclaration::StringToType(typeString), name, count);
				m_Resources.push_back(declaration);
			}
			else {
				if (count > 1) {

					for (int countID = 0; countID < count; countID++) {

						const GLShaderUniformDeclaration::Type t = GLShaderUniformDeclaration::StringToType(typeString, count);
						GLShaderUniformDeclaration* declaration;

						name += "[" + StringUtils::ToString(countID) + "]";
						bool ISStruct = false;

						if (t == GLShaderUniformDeclaration::Type::NONE) {
							ShaderStruct* s = FindStruct(typeString);
							EASEL_ASSERT(s, "");
							declaration = new GLShaderUniformDeclaration(s, name, count);

							ISStruct = true;
						}
						else {
							declaration = new GLShaderUniformDeclaration(t, name, count);
						}

						if (StringUtils::StartsWith(name, "sys_")) {
							static_cast<GLShaderUniformBufferDeclaration*>(m_UniformBuffers[type].front())->PushUniform(declaration);
						}
						else {
							if (m_UserUniformBuffers[type] == nullptr)
								m_UserUniformBuffers[type] = new GLShaderUniformBufferDeclaration("", 0);

							if (ISStruct) {
								for (uint32_t id = 0; id < static_cast<uint32_t>(count); id++) {
									GLShaderUniformDeclaration* test = new GLShaderUniformDeclaration(*declaration);
									test->SetName(name + "[" + StringUtils::ToString(id) + "]");
									test->m_Struct = new ShaderStruct(*declaration->m_Struct);
									m_UserUniformBuffers[type]->PushUniform(test);
								}
							}
							else {
								m_UserUniformBuffers[type]->PushUniform(declaration);
							}
						}

						if (countID > 9)
							name = name.substr(0, name.size() - 4);
						else
							name = name.substr(0, name.size() - 3);
					}
				}
				else {
					const GLShaderUniformDeclaration::Type t = GLShaderUniformDeclaration::StringToType(typeString, count);
					GLShaderUniformDeclaration* declaration;

					bool isStruct = false;
					if (t == GLShaderUniformDeclaration::Type::NONE) {
						///Shaderstruct* s = FindStruct(typeString);
						//EASEL_CORE_ASSERT(s, "");
						//declaration = new GLShaderUniformDeclaration(s, name, count);

						ShaderStruct* s = FindStruct(typeString);
						if (s) {
							declaration = new GLShaderUniformDeclaration(s, name, count);

							isStruct = true;
						}
						else {
							return;
						}
					}
					else {
						declaration = new GLShaderUniformDeclaration(t, name, count);
					}

					if (StringUtils::StartsWith(name, "sys_")) {
						static_cast<GLShaderUniformBufferDeclaration*>(m_UniformBuffers[type].front())->PushUniform(declaration);
					}
					else {
						if (m_UserUniformBuffers[type] == nullptr)
							m_UserUniformBuffers[type] = new GLShaderUniformBufferDeclaration("", 0);

						if (isStruct) {
							GLShaderUniformDeclaration* test = new GLShaderUniformDeclaration(*declaration);
							test->SetName(name);
							test->m_Struct = new ShaderStruct(*declaration->m_Struct);
							m_UserUniformBuffers[type]->PushUniform(test);
						}
						else {
							m_UserUniformBuffers[type]->PushUniform(declaration);
						}
					}
				}
			}
		}

		void GLShader::ParseUniformStruct(const std::string& block, ShaderType shaderType) {
			std::vector<std::string> tokens = StringUtils::Tokenize(block);

			uint32_t index = 0;
			index++; // struct
			std::string name = tokens[index++];
			ShaderStruct* uniformStruct = new ShaderStruct(name);
			index++; // {
			while (index < tokens.size())
			{
				if (tokens[index] == "}")
					break;

				const std::string type = tokens[index++];
				name = tokens[index++];

				// Strip ; from name if present
				if (const char* s = strstr(name.c_str(), ";"))
					name = std::string(name.c_str(), s - name.c_str());

				uint32_t count = 1;
				const char* namestr = name.c_str();
				if (const char* s = strstr(namestr, "[")) {
					name = std::string(namestr, s - namestr);

					const char* end = strstr(namestr, "]");
					std::string c(s + 1, end - s);
					count = atoi(c.c_str());
				}

				ShaderUniformDeclaration* field = new GLShaderUniformDeclaration(GLShaderUniformDeclaration::StringToType(type, count), name, count);
				uniformStruct->AddField(field);
			}
			m_Structs.push_back(uniformStruct);
		}

		bool GLShader::IsTypeStringResource(const std::string& type) {
			if (type == "sampler2D")
				return true;
			if (type == "samplerCube")
				return true;
			if (type == "sampler2DShadow")
				return true;
			if (type == "sampler2DArrayShadow")
				return true;
			return false;
		}

		void GLShader::ResolveUniforms() {
			EASEL_PROFILE_FUNCTION();
			Bind();

			for (auto shader : m_UniformBuffers) {
				
				for (auto j : shader.second) {

					GLShaderUniformBufferDeclaration* decl = static_cast<GLShaderUniformBufferDeclaration*>(j);
					const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
					for (uint32_t k = 0; k < uniforms.size(); k++) {

						GLShaderUniformDeclaration* uniform = static_cast<GLShaderUniformDeclaration*>(uniforms[k]);
						if (uniform->GetType() == GLShaderUniformDeclaration::Type::STRUCT) {

							const ShaderStruct& s = uniform->GetShaderUniformStruct();
							const auto& fields = s.GetFields();
							if (uniform->GetCount() == 1) {
								for (uint32_t l = 0; l < fields.size(); l++) {
									GLShaderUniformDeclaration* field = static_cast<GLShaderUniformDeclaration*>(fields[l]);
									field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
								}
							}
							else {
								for (uint32_t i = 0; i < uniform->GetCount(); i++) {
									for (uint32_t l = 0; l < fields.size(); l++) {
										GLShaderUniformDeclaration* field = static_cast<GLShaderUniformDeclaration*>(fields[l]);
										field->m_Location = GetUniformLocation(uniform->m_Name + "[" + std::to_string(i) + "]" + "." + field->m_Name);
									}
								}
							}
						}
						else {
							uniform->m_Location = GetUniformLocation(uniform->m_Name);
						}
					}
				}
			}

			for (const auto& shader : m_UserUniformBuffers) {
				GLShaderUniformBufferDeclaration* decl = shader.second;
				if (decl) {
					const ShaderUniformList& uniforms = decl->GetUniformDeclarations();
					for (uint32_t j = 0; j < uniforms.size(); j++) {
						GLShaderUniformDeclaration* uniform = static_cast<GLShaderUniformDeclaration*>(uniforms[j]);
						if (uniform->GetType() == GLShaderUniformDeclaration::Type::STRUCT) {
							const ShaderStruct& s = uniform->GetShaderUniformStruct();
							const auto& fields = s.GetFields();

							if (uniform->GetCount() == 1) {
								for (uint32_t k = 0; k < fields.size(); k++) {
									GLShaderUniformDeclaration* field = static_cast<GLShaderUniformDeclaration*>(fields[k]);
									field->m_Location = GetUniformLocation(uniform->m_Name + "." + field->m_Name);
								}
							}
							else {
								for (uint32_t i = 0; i < uniform->GetCount(); i++) {
									for (uint32_t k = 0; k < fields.size(); k++) {
										GLShaderUniformDeclaration* field = static_cast<GLShaderUniformDeclaration*>(fields[k]);
										field->m_Location = GetUniformLocation(uniform->m_Name + "[" + std::to_string(i) + "]" + "." + field->m_Name);
									}
								}
							}
						}
						else {
							uniform->m_Location = GetUniformLocation(uniform->m_Name);
						}
					}
				}
			}

			uint32_t sampler = 0;
			for (uint32_t i = 0; i < m_Resources.size(); i++) {
				GLShaderResourceDeclaration* resource = static_cast<GLShaderResourceDeclaration*>(m_Resources[i]);
				uint32_t location = GetUniformLocation(resource->m_Name);
				if (resource->GetCount() == 1) {
					resource->m_Register = sampler;
					SetUniform1i(location, sampler++);
				}
				else if (resource->GetCount() > 1) {
					resource->m_Register = 0;
					uint32_t count = resource->GetCount();
					int32_t* samplers = new int32_t[count];
					for (uint32_t s = 0; s < count; s++)
						samplers[s] = s;
					SetUniform1iv(resource->GetName(), samplers, count);
					delete[] samplers;
				}
			}
			Unbind();
		}

		void GLShader::ValidateUniforms() {

		}

		bool GLShader::IsSystemUniform(ShaderUniformDeclaration* uniform) {
			EASEL_PROFILE_FUNCTION();
			return StringUtils::StartsWith(uniform->GetName(), "sys_");
		}

		GLint GLShader::GetUniformLocation(const std::string& name) const {
			EASEL_PROFILE_FUNCTION();
			GLCall(const GLint result = glGetUniformLocation(m_Handle, name.c_str()));
			//if (result == -1)
			//	EASEL_CORE_WARN("{0} : could not find uniform {1} in shader!",m_Name,name);

			return result;
		}

		void GLShader::SetUniformStruct(GLShaderUniformDeclaration* uniform, uint8_t* data, int32_t offset) const {
			EASEL_PROFILE_FUNCTION();
			const ShaderStruct& s = uniform->GetShaderUniformStruct();
			const auto& fields = s.GetFields();
			for (uint32_t k = 0; k < fields.size(); k++) {
				GLShaderUniformDeclaration* field = static_cast<GLShaderUniformDeclaration*>(fields[k]);
				ResolveAndSetUniformField(*field, data, offset, uniform->GetCount());
				offset += field->m_Size;
			}
		}

		ShaderUniformDeclaration* GLShader::FindUniformDeclaration(const std::string& name, const ShaderUniformBufferDeclaration* buffer) {
			EASEL_PROFILE_FUNCTION();
			const ShaderUniformList& uniforms = buffer->GetUniformDeclarations();
			for (uint32_t i = 0; i < uniforms.size(); i++) {
				if (uniforms[i]->GetName() == name)
					return uniforms[i];
			}
			return nullptr;
		}

		ShaderUniformDeclaration* GLShader::FindUniformDeclaration(const std::string& name) {
			EASEL_PROFILE_FUNCTION();
			ShaderUniformDeclaration* result = nullptr;

			for (auto shader : m_UniformBuffers) {
				for (uint32_t i = 0; i < shader.second.size(); i++) {
					result = FindUniformDeclaration(name, shader.second[i]);
					if (result)
						return result;
				}
			}

			for (const auto& shader : m_UserUniformBuffers) {
				result = FindUniformDeclaration(name, shader.second);
				if (result)
					return result;
			}

			return result;
		}

		void GLShader::SetUserUniformBuffer(ShaderType type, uint8_t* data, uint32_t size) {
			EASEL_PROFILE_FUNCTION();
			ResolveAndSetUniforms(m_UserUniformBuffers[type], data, size);
		}

		ShaderStruct* GLShader::FindStruct(const std::string& name) {
			EASEL_PROFILE_FUNCTION();
			for (ShaderStruct* s : m_Structs) {
				if (s->GetName() == name)
					return s;
			}
			return nullptr;
		}

		void GLShader::ResolveAndSetUniforms(ShaderUniformBufferDeclaration* buffer, uint8_t* data, uint32_t size) const {
			EASEL_PROFILE_FUNCTION();
			const ShaderUniformList& uniforms = buffer->GetUniformDeclarations();
			for (uint32_t i = 0; i < uniforms.size(); i++) {
				GLShaderUniformDeclaration* uniform = static_cast<GLShaderUniformDeclaration*>(uniforms[i]);

				ResolveAndSetUniform(uniform, data, size, uniform->GetCount());
			}
		}

		void GLShader::ResolveAndSetUniform(GLShaderUniformDeclaration* uniform, uint8_t* data, uint32_t size, uint32_t count) const {
			EASEL_PROFILE_FUNCTION();
			if (uniform->GetLocation() == -1) {
				//EASEL_CORE_ERROR( "Couldnt Find Uniform In Shader: " + uniform->GetName());
				return;
			}

			const uint32_t offset = uniform->GetOffset();
			switch (uniform->GetType()) {

			case GLShaderUniformDeclaration::Type::FLOAT32:
				SetUniform1f(uniform->GetLocation(), *reinterpret_cast<float*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::INT32:
				SetUniform1i(uniform->GetLocation(), *reinterpret_cast<int32_t*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::UINT:
				SetUniform1ui(uniform->GetLocation(), *reinterpret_cast<uint32_t*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::INT:
				SetUniform1i(uniform->GetLocation(), *reinterpret_cast<int*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::VEC2:
				SetUniform2f(uniform->GetLocation(), *reinterpret_cast<glm::vec2*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::VEC3:
				SetUniform3f(uniform->GetLocation(), *reinterpret_cast<glm::vec3*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::VEC4:
				SetUniform4f(uniform->GetLocation(), *reinterpret_cast<glm::vec4*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::MAT3:
				SetUniformMat3(uniform->GetLocation(), *reinterpret_cast<glm::mat3*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::MAT4:
				SetUniformMat4(uniform->GetLocation(), *reinterpret_cast<glm::mat4*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::MAT4ARRAY:
				SetUniformMat4Array(uniform->GetLocation(), count, *reinterpret_cast<glm::mat4*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::STRUCT:
				SetUniformStruct(uniform, data, offset);
				break;
			default:
				EASEL_ASSERT(false, "Unknown type!");
			}
		}

		void GLShader::SetUniform(const std::string& name, uint8_t* data) {
			EASEL_PROFILE_FUNCTION();
			ShaderUniformDeclaration* uniform = FindUniformDeclaration(name);
			if (!uniform) {
				EASEL_CORE_ERROR("Cannot find uniform in {0} shader with name '{1}'", m_Name, name);
				return;
			}
			ResolveAndSetUniform(static_cast<GLShaderUniformDeclaration*>(uniform), data, 0, uniform->GetCount());
		}

		void GLShader::ResolveAndSetUniformField(const GLShaderUniformDeclaration& field, uint8_t* data, int32_t offset, uint32_t count) const {
			EASEL_PROFILE_FUNCTION();
			//EASEL_ASSERT(field.GetLocation() < 0, "Couldnt Find Uniform In Shader: " + field.GetName());

			switch (field.GetType()) {

			case GLShaderUniformDeclaration::Type::FLOAT32:
				SetUniform1f(field.GetLocation(), *reinterpret_cast<float*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::INT32:
				SetUniform1i(field.GetLocation(), *reinterpret_cast<int32_t*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::UINT:
				SetUniform1ui(field.GetLocation(), *reinterpret_cast<uint32_t*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::VEC2:
				SetUniform2f(field.GetLocation(), *reinterpret_cast<glm::vec2*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::VEC3:
				SetUniform3f(field.GetLocation(), *reinterpret_cast<glm::vec3*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::VEC4:
				SetUniform4f(field.GetLocation(), *reinterpret_cast<glm::vec4*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::MAT3:
				SetUniformMat3(field.GetLocation(), *reinterpret_cast<glm::mat3*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::MAT4:
				SetUniformMat4(field.GetLocation(), *reinterpret_cast<glm::mat4*>(&data[offset]));
				break;
			case GLShaderUniformDeclaration::Type::MAT4ARRAY:
				SetUniformMat4Array(field.GetLocation(), count, *reinterpret_cast<glm::mat4*>(&data[offset]));
				break;
			default:
				EASEL_ASSERT(false, "Unknown type!");
			}
		}

		void GLShader::SetUniform1f(const std::string& name, float value) const {
			EASEL_PROFILE_FUNCTION();
			SetUniform1f(GetUniformLocation(name), value);
		}

		void GLShader::SetUniform1fv(const std::string& name, float* value, int32_t count) const {
			EASEL_PROFILE_FUNCTION();
			SetUniform1fv(GetUniformLocation(name), value, count);
		}

		void GLShader::SetUniform1i(const std::string& name, int32_t value) const {
			EASEL_PROFILE_FUNCTION();
			SetUniform1i(GetUniformLocation(name), value);
		}

		void GLShader::SetUniform1ui(const std::string& name, uint32_t value) const {
			EASEL_PROFILE_FUNCTION();
			SetUniform1ui(GetUniformLocation(name), value);
		}

		void GLShader::SetUniform1iv(const std::string& name, int32_t* value, int32_t count) const {
			EASEL_PROFILE_FUNCTION();
			SetUniform1iv(GetUniformLocation(name), value, count);
		}

		void GLShader::SetUniform2f(const std::string& name, const glm::vec2& vector) const {
			EASEL_PROFILE_FUNCTION();
			SetUniform2f(GetUniformLocation(name), vector);
		}

		void GLShader::SetUniform3f(const std::string& name, const glm::vec3& vector) const {
			EASEL_PROFILE_FUNCTION();
			SetUniform3f(GetUniformLocation(name), vector);
		}

		void GLShader::SetUniform4f(const std::string& name, const glm::vec4& vector) const {
			EASEL_PROFILE_FUNCTION();
			SetUniform4f(GetUniformLocation(name), vector);
		}

		void GLShader::SetUniformMat4(const std::string& name, const glm::mat4& matrix) const {
			EASEL_PROFILE_FUNCTION();
			SetUniformMat4(GetUniformLocation(name), matrix);
		}

		void GLShader::SetUniform1f(uint32_t location, float value) {
			GLCall(glUniform1f(location, value));
		}

		void GLShader::SetUniform1fv(uint32_t location, float* value, int32_t count) {
			GLCall(glUniform1fv(location, count, value));
		}

		void GLShader::SetUniform1i(uint32_t location, int32_t value) {
			GLCall(glUniform1i(location, value));
		}

		void GLShader::SetUniform1ui(uint32_t location, uint32_t value) {
			GLCall(glUniform1ui(location, value));
		}

		void GLShader::SetUniform1iv(uint32_t location, int32_t* value, int32_t count) {
			GLCall(glUniform1iv(location, count, value));
		}

		void GLShader::SetUniform2f(uint32_t location, const glm::vec2& vector) {
			GLCall(glUniform2f(location, vector.x, vector.y));
		}

		void GLShader::SetUniform3f(uint32_t location, const glm::vec3& vector) {
			GLCall(glUniform3f(location, vector.x, vector.y, vector.z));
		}

		void GLShader::SetUniform4f(uint32_t location, const glm::vec4& vector) {
			GLCall(glUniform4f(location, vector.x, vector.y, vector.z, vector.w));
		}

		void GLShader::SetUniformMat3(uint32_t location, const glm::mat3& matrix) {
			GLCall(glUniformMatrix3fv(location, 1, GL_FALSE /*GLTRUE*/, glm::value_ptr(matrix))); // &matrix.values[0]));
		}

		void GLShader::SetUniformMat4(uint32_t location, const glm::mat4& matrix) {
			GLCall(glUniformMatrix4fv(location, 1, GL_FALSE /*GLTRUE*/, glm::value_ptr(matrix)));
		}

		void GLShader::SetUniformMat4Array(uint32_t location, uint32_t count, const glm::mat4& matrix) {
			GLCall(glUniformMatrix4fv(location, count, GL_FALSE /*GLTRUE*/, glm::value_ptr(matrix)));
		}

		Shader* GLShader::CreateFuncGL(const std::string& filePath) {
			std::string physicalPath;
			Easel::VFS::Get()->ResolvePhysicalPath(filePath, physicalPath);
			GLShader* result = new GLShader(physicalPath);
			result->m_Path = filePath;
			return result;
		}

		void GLShader::MakeDefault()
		{
			CreateFunc = CreateFuncGL;
		}
	}
}
