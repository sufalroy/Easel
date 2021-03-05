#pragma once

#include "Easel/Core/Base.h"
#include "Easel/Graphics/API/DescriptorSet.h"
#include "Easel/Maths/Maths.h"

namespace Easel {

	namespace Graphics {

		struct EASEL_EXPORT BufferElement {
			std::string name;
			Format format;
			uint32_t offset = 0;
			bool normalized = false;
		};

		class EASEL_EXPORT BufferLayout {
		private:
			uint32_t m_Size;
			std::vector<BufferElement> m_Layout;

		public:
			BufferLayout();

			template<typename T>
			void Push(const std::string& name, bool normalized = false) {
				EASEL_ASSERT(false, "Unknown type!");
			}

			inline const std::vector<BufferElement>& GetLayout() const {
				return m_Layout;
			}

			inline uint32_t GetStride() const {
				return m_Size;
			}

		private:
			void Push(const std::string& name, Format format, uint32_t size, bool normalized);
		};

		template<>
		void EASEL_EXPORT BufferLayout::Push<float>(const std::string& name, bool normalized);
		template<>
		void EASEL_EXPORT BufferLayout::Push<uint32_t>(const std::string& name, bool normalized);
		template<>
		void EASEL_EXPORT BufferLayout::Push<uint8_t>(const std::string& name, bool normalized);
		template<>
		void EASEL_EXPORT BufferLayout::Push<Maths::Vector2>(const std::string& name, bool normalized);
		template<>
		void EASEL_EXPORT BufferLayout::Push<Maths::Vector3>(const std::string& name, bool normalized);
		template<>
		void EASEL_EXPORT BufferLayout::Push<Maths::Vector4>(const std::string& name, bool normalized);
		template<>
		void EASEL_EXPORT BufferLayout::Push<Maths::IntVector3>(const std::string& name, bool normalized);
		template<>
		void EASEL_EXPORT BufferLayout::Push<Maths::IntVector4>(const std::string& name, bool normalized);
	}
}
