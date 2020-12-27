#include "Precompiled.h"
#include "Easel/Core/Application.h"

#include "Easel/Core/VFS.h"
#include "Easel/Core/Engine.h"
#include "Easel/Core/OS/Input.h"
#include "Easel/Events/ApplicationEvent.h"

namespace Easel {

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& projectRoot, const std::string& projectName) {

		EASEL_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		/*
		const std::string root = ROOT_DIR;
		VFS::Get()->Mount("Meshes", root + projectRoot + "/res/meshes");
		VFS::Get()->Mount("Textures", root + projectRoot + "/res/textures");
		VFS::Get()->Mount("Sounds", root + projectRoot + "/res/sounds");
		VFS::Get()->Mount("Scripts", root + projectRoot + "/res/scripts");
		VFS::Get()->Mount("Scenes", root + projectRoot + "/res/scenes");
		VFS::Get()->Mount("EngineShaders", root + "/Easel/Shaders");
		*/

		Engine::Get();
	}

	Application::~Application() {

	}

	void Application::Init() {

		Input::Create();

		m_CurrentState = AppState::Running;
	}

	int Application::Quit(bool pause, const std::string& reason) {
		Engine::Release();
		Input::Release();

		if (pause) {
			EASEL_CORE_ERROR("{0}", reason);
		}

		return 0;
	}

	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);

		Input::GetInput()->OnEvent(e);
	}	

	void Application::Run() {
		while (true) {

		}

		Quit();
	}
	void Application::OnUpdate(const Timestep& dt) {

	}
}


