#pragma once

#include "Easel/Core/Reference.h"
#include "Easel/Core/OS/Window.h"
#include "Easel/Utils/Timestep.h"

#include <cereal/types/vector.hpp>
#include <cereal/cereal.hpp>

namespace Easel {

	class Window;
	struct WindowProperties;
	class Event;
	class WindowCloseEvent;
	class WindowResizeEvent;

	namespace Graphics {

		enum class RenderAPI : uint32_t;
	}

	enum class AppState {
		Running,
		Loading,
		Closing
	};

	enum class EditorState {
		Paused,
		Play,
		Next,
		Preview
	};

	enum class AppType {
		Game,
		Editor
	};

	class EASEL_EXPORT Application {
	public:
		Application(const std::string& projectRoot, const std::string& projectName);
		virtual ~Application();

		virtual void Init();

		void Run();
		bool OnFrame();
		void OnUpdate(const Timestep& dt);
		void OnRender();
		void OnEvent(Event& e);
		void OnImGui();

		int Quit(bool pause = false, const std::string& reason = "");
		
		Window* GetWindow() const {
			return m_Window.get();
		}

		AppState GetState() const {
			return m_CurrentState;
		}

		EditorState GetEditorState() const {
			return m_EditorState;
		}

		void SetAppState(AppState state) {
			m_CurrentState = state;
		}

		void SetEditorState(EditorState state) {
			m_EditorState = state;
		}

		glm::vec2 GetWindowSize() const;

		static Application& Get() { return *s_Instance; }

		static void Release() {
			if (s_Instance)
				delete s_Instance;
			s_Instance = nullptr;
		}

		bool OnWindowResize(WindowResizeEvent& e);

		virtual void Serialise(const std::string& filePath);
		virtual void Deserialise(const std::string& filePath);

		template<typename Archive>
		void save(Archive& archive) const {

			int projectVersion = 1;

			archive(cereal::make_nvp("Project Version", projectVersion));
			//Version 1
			archive(cereal::make_nvp("RenderAPI", RenderAPI),
				cereal::make_nvp("Width", m_Window->GetWidth()),
				cereal::make_nvp("Height", m_Window->GetHeight()),
				cereal::make_nvp("Fullscreen", Fullscreen),
				cereal::make_nvp("VSync", VSync),
				cereal::make_nvp("ShowConsole", ShowConsole),
				cereal::make_nvp("Title", Title),
				cereal::make_nvp("FilePath", FilePath));
		}

		template<typename Archive>
		void load(Archive& archive)
		{
			int projectVersion = 0;
			archive(cereal::make_nvp("Project Version", projectVersion));

			if (projectVersion > 0)
			{
				archive(cereal::make_nvp("RenderAPI", RenderAPI),
					cereal::make_nvp("Width", Width),
					cereal::make_nvp("Height", Height),
					cereal::make_nvp("Fullscreen", Fullscreen),
					cereal::make_nvp("VSync", VSync),
					cereal::make_nvp("ShowConsole", ShowConsole),
					cereal::make_nvp("Title", Title),
					cereal::make_nvp("FilePath", FilePath));
			}
		}
	
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		//Start proj saving
		uint32_t Width, Height;
		bool Fullscreen;
		bool VSync;
		bool Borderless = false;
		bool ShowConsole = true;
		std::string Title;
		int RenderAPI;
		std::string FilePath;
		//

		UniqueRef<Window> m_Window;

		AppState m_CurrentState = AppState::Loading;
		EditorState m_EditorState = EditorState::Preview;
		AppType m_AppType = AppType::Game;

		static Application* s_Instance;
		
		NONCOPYABLE(Application)
	};

	// To be defined in client
	Application* CreateApplication();
}
