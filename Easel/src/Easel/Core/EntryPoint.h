#pragma once

#include "Easel/Core/CoreSystem.h"

#ifdef EASEL_PLATFORM_WINDOWS

extern Easel::Application* Easel::CreateApplication();

int main(int argc, char** argv) {

	Easel::Internal::CoreSystem::Init(false);

	auto app = Easel::CreateApplication();

	app->Run();

	delete app;

	Easel::Internal::CoreSystem::Shutdown();
}

#endif // EASEL_PLATFORM_WINDOWS
