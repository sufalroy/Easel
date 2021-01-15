#pragma once

#include "Easel/Core/Reference.h"

#include "Easel/Graphics/API/Renderer.h"
#include "Easel/Graphics/API/BufferLayout.h"

namespace Easel {

	namespace Graphics {

		class Shader;
		class RenderPass;
		class CommandBuffer;
		class DescriptorSet;
		struct VertexInputDescription;
		struct DescriptorLayoutInfo;
		struct DescriptorPoolInfo;

		enum class CullMode {
			FRONT,
			BACK,
			FRONTANDBACK,
			NONE
		};

		enum class PolygonMode {
			FILL,
			LINE,
			POINT
		};

		struct PipelineInfo {
			Ref<RenderPass> renderpass;
			Ref<Shader> shader;
			BufferLayout vertexBufferLayout;

			CullMode cullMode = CullMode::BACK;
			PolygonMode polygonMode = PolygonMode::FILL;
			DrawType drawType = DrawType::TRIANGLE;

			bool transparencyEnabled;
			bool depthBiasEnabled;
		};

		class EASEL_EXPORT Pipeline {
		public:
			static Pipeline* Create(const PipelineInfo& pipelineInfo);
			static Ref<Pipeline> Get(const PipelineInfo& pipelineInfo);
			static void ClearCache();
			static void DeleteUnusedCache();

			virtual ~Pipeline() = default;

			virtual void Bind(CommandBuffer* cmdBuffer) = 0;

			virtual DescriptorSet* GetDescriptorSet() const = 0;
			virtual Shader* GetShader() const = 0;

		protected:
			static Pipeline* (*CreateFunc)(const PipelineInfo&);
		};
	}
}
