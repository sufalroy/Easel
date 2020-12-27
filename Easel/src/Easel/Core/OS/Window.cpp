#include "Precompiled.h"
#include "Easel/Core/OS/Window.h"

namespace Easel {

	Window* (*Window::CreateFunc)(const WindowProperties&) = NULL;

	Window* Window::Create(const WindowProperties& properties) {
		EASEL_ASSERT(CreateFunc, "No Windows Create Function");
		return CreateFunc(properties);
	}

	bool Window::Initialise(const WindowProperties& properties) {
		return HasInitialised();
	}
}
