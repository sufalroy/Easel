#pragma once
#include "Easel/Graphics/API/RenderDevice.h"

namespace Easel {

	namespace Graphics {

		class GLRenderDevice : public RenderDevice {
		public:
			GLRenderDevice() = default;
			~GLRenderDevice() = default;

			void Init() override;

			static void MakeDefault();
		protected:
			static RenderDevice* CreateFuncGL();
		};
	}
}
