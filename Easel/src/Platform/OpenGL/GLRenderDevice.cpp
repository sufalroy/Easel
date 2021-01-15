#include "Precompiled.h"
#include "GLRenderDevice.h"

namespace Easel::Graphics {

	void GLRenderDevice::Init() {

	}

	void GLRenderDevice::MakeDefault() {

		CreateFunc = CreateFuncGL;
	}

	RenderDevice* GLRenderDevice::CreateFuncGL()
	{
		return new GLRenderDevice();
	}

}

