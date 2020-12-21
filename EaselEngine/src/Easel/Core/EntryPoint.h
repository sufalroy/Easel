#pragma once

#include "Easel/Core/Base.h"
#include "Easel/Core/Log.h"
#include "Easel/Core/Application.h"

#ifdef EASEL_PLATFORM_WINDOWS

extern Easel::Application* Easel::CreateApplication();

int main(int argc, char** argv) {

	Easel::Debug::Log::OnInit();
	auto app = Easel::CreateApplication();

	app->Run();

	delete app;
	Easel::Debug::Log::OnRelease();
}

#endif // EASEL_PLATFORM_WINDOWS
