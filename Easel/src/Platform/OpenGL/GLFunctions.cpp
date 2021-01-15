#include "Precompiled.h"
#include "GLFunctions.h"
#include "GLContext.h"
#include "GLRenderDevice.h"


void Easel::Graphics::GL::MakeDefault() {

	GLContext::MakeDefault();
	GLRenderDevice::MakeDefault();
}
