#pragma once

#include "Easel/Core/Base.h"
#include "Easel/Events/Event.h"
#include "Easel/Utils/Timestep.h"

#include <string>


namespace Easel {

	class Event;

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
		void OnUpdate(const Timestep& dt);
		void OnEvent(Event& e);

		int Quit(bool pause = false, const std::string& reason = "");
		

		_FORCE_INLINE_ static Application& Get() { return *s_Instance; }
		static void Release() {
			if (s_Instance)
				delete s_Instance;
			s_Instance = nullptr;
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
		
	private:
		AppState m_CurrentState = AppState::Loading;
		EditorState m_EditorState = EditorState::Preview;
		AppType m_AppType = AppType::Game;

		static Application* s_Instance;
		
		NONCOPYABLE(Application)
	};

	// To be defined in client
	Application* CreateApplication();
}
