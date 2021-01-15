#include "Precompiled.h"
#include "Easel/Core/Application.h"

#include "Easel/Core/Engine.h"

#include "Easel/Graphics/API/GraphicsContext.h"

#include "Easel/Utils/LoadImage.h"
#include "Easel/Core/OS/Input.h"
#include "Easel/Core/OS/Window.h"
#include "Easel/Core/OS/OS.h"
#include "Easel/Core/VFS.h"
#include "Easel/Core/OS/FileSystem.h"

#include "Easel/Events/ApplicationEvent.h"

#include <cereal/archives/json.hpp>
#include <imgui/imgui.h>

namespace Easel {

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& projectRoot, const std::string& projectName) {
		EASEL_PROFILE_FUNCTION();
		EASEL_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		FilePath = projectRoot + projectName + std::string(".esproj");

		const std::string root = ROOT_DIR;
		VFS::Get()->Mount("Meshes", root + projectRoot + "/res/meshes");
		VFS::Get()->Mount("Textures", root + projectRoot + "/res/textures");
		VFS::Get()->Mount("Sounds", root + projectRoot + "/res/sounds");
		VFS::Get()->Mount("Scripts", root + projectRoot + "/res/scripts");
		VFS::Get()->Mount("Scenes", root + projectRoot + "/res/scenes");
		VFS::Get()->Mount("EngineShaders", root + "/Easel/Shaders");

		Deserialise(FilePath);

		Engine::Get();

		WindowProperties  windowProperties;
		windowProperties.Width = Width;
		windowProperties.Height = Height;
		windowProperties.RenderAPI = RenderAPI;
		windowProperties.Fullscreen = Fullscreen;
		windowProperties.Borderless = Borderless;
		windowProperties.ShowConsole = ShowConsole;
		windowProperties.Title = Title;
		windowProperties.VSync = VSync;

		m_Window = UniqueRef<Window>(Window::Create(windowProperties));

		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
	}

	Application::~Application() {
		EASEL_PROFILE_FUNCTION();
	}

	void Application::Init() {
		EASEL_PROFILE_FUNCTION();
		if (!m_Window->HasInitialised())
			Quit(true, "Window failed to initialise!");

		Input::Create();

		m_CurrentState = AppState::Running;
	}

	int Application::Quit(bool pause, const std::string& reason) {
		EASEL_PROFILE_FUNCTION();
		Serialise(FilePath);
		Engine::Release();
		Input::Release();

		m_Window.reset();

		if (pause) {
			EASEL_CORE_ERROR("{0}", reason);
		}

		return 0;
	}

	glm::vec2 Application::GetWindowSize() const {

		return glm::vec2(static_cast<float>(m_Window->GetWidth()), static_cast<float>(m_Window->GetHeight()));
	}

	bool Application::OnFrame() {
		EASEL_PROFILE_FUNCTION();
		return false;
	}

	void Application::OnRender() {
		EASEL_PROFILE_FUNCTION();
	}

	void Application::OnUpdate(const Timestep& dt) {
		EASEL_PROFILE_FUNCTION();
	}
	

	void Application::OnEvent(Event& e) {
		EASEL_PROFILE_FUNCTION();
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

		Input::GetInput()->OnEvent(e);
	}

	void Application::Run() {
		while (true) {

		}

		Quit();
	}


	bool Application::OnWindowResize(WindowResizeEvent& e) {
		EASEL_PROFILE_FUNCTION();
		return false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_CurrentState = AppState::Closing;
		return true;
	}
	
	void Application::OnImGui() {
		EASEL_PROFILE_FUNCTION();
	}

	void Application::Serialise(const std::string& filePath)
	{
		EASEL_PROFILE_FUNCTION();
		{
			std::stringstream storage;
			{
				// output finishes flushing its contents when it goes out of scope
				cereal::JSONOutputArchive output{ storage };
				output(*this);
			}
			auto fullPath = ROOT_DIR + filePath;
			FileSystem::WriteTextFile(fullPath, storage.str());
		}
	}

	void Application::Deserialise(const std::string& filePath)
	{
		EASEL_PROFILE_FUNCTION();
		{
			auto fullPath = ROOT_DIR + filePath;
			if (!FileSystem::FileExists(fullPath))
			{
				EASEL_CORE_INFO("No saved Project file found {0}", fullPath);
				{
					//Set Default values
					RenderAPI = 0;
					Width = 1200;
					Height = 800;
					VSync = false;
					Title = "Game";
					ShowConsole = false;
					Fullscreen = false;
				}
				return;
			}

			std::string data = FileSystem::ReadTextFile(fullPath);
			std::istringstream istr;
			istr.str(data);
			cereal::JSONInputArchive input(istr);
			input(*this);
		}
	}
}


