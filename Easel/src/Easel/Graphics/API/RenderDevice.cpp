#include "Precompiled.h"
#include "Easel/Graphics/API/RenderDevice.h"

namespace Easel {

	namespace Graphics {

		RenderDevice* (*RenderDevice::CreateFunc)() = nullptr;

		RenderDevice* RenderDevice::s_Instance = nullptr;

		void RenderDevice::Create() {
			EASEL_ASSERT(CreateFunc, "No RenderDevice Create Function");

			s_Instance = CreateFunc();
		}

		void RenderDevice::Release() {
			
			delete s_Instance;
		}

	}
}

