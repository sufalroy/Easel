#include "Precompiled.h"
#include "Easel/Core/Application.h"

#include "Easel/Core/VFS.h"
#include "Easel/Core/Engine.h"

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
		VFS::Get()->Mount("CoreShaders", root + "/EaselEngine/res/EngineShaders");
		*/

		Engine::Get();
	}

	Application::~Application() {

	}

	void Application::OnEvent(Event& e) {
		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			if (e.Handled())
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer* layer) {
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer) {
		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Close() {

	}

	void Application::Run() {
		while (true) {

		}
	}
}


