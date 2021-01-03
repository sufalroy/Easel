#pragma once

#ifdef EASEL_PLATFORM_WINDOWS

#include "Easel/Core/CoreSystem.h"
#include "Platform/Windows/WindowsOS.h"

extern Easel::Application* Easel::CreateApplication();

int main(int argc, char** argv) {

	Easel::Internal::CoreSystem::Init(false);

	auto windowsOS = new Easel::WindowsOS();
	Easel::OS::SetInstance(windowsOS);

	windowsOS->Init();

	Easel::CreateApplication();

	windowsOS->Run();
	delete windowsOS;

	Easel::Internal::CoreSystem::Shutdown();
}

#endif // EASEL_PLATFORM_WINDOWS
