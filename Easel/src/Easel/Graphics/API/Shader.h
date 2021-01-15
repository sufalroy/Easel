#pragma once

#include "Easel/Graphics/API/ShaderUniform.h"
#include "Easel/Graphics/API/ShaderResource.h"

#include <unordered_map>

#define SHADER_VERTEX_INDEX 0
#define SHADER_UV_INDEX 1
#define SHADER_MASK_UV_INDEX 2
#define SHADER_TID_INDEX 3
#define SHADER_MID_INDEX 4
#define SHADER_COLOR_INDEX 5

namespace Easel {

	namespace Graphics {

		enum class ShaderType : int {
			VERTEX,
			FRAGMENT,
			GEOMETRY,
			TESSELLATION_CONTROL,
			TESSELLATION_EVALUATION,
			COMPUTE,
			UNKNOWN
		};

		struct ShaderEnumClassHash {
			template<typename T>
			std::size_t operator()(T t) const {
				return static_cast<std::size_t>(t);
			}
		};

		template<typename Key>
		using HashType = typename std::conditional<std::is_enum<Key>::value, ShaderEnumClassHash, std::hash<Key>>::type;

		template<typename Key, typename T>
		using UnorderedMap = std::unordered_map<Key, T, HashType<Key>>;

		class EASEL_EXPORT Shader {
		public:
			static const Shader* s_CurrentlyBound;

		public:
			virtual void Bind() const = 0;
			virtual void Unbind() const = 0;

			virtual ~Shader() = default;
			
			virtual const std::vector<ShaderType> GetShaderTypes() const = 0;
			virtual const std::string& GetName() const = 0;
			virtual const std::string& GetFilePath() const = 0;

			virtual void* GetHandle() const = 0;

		public:
			static Shader* CreateFromFile(const std::string& filepath);

		protected:
			static Shader* (*CreateFunc)(const std::string&);
		};
	}
}
