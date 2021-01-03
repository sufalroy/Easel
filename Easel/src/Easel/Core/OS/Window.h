#pragma once

#include "Easel/Events/Event.h"

#include <glm/glm.hpp>

namespace Easel {

	struct EASEL_EXPORT WindowProperties {
		WindowProperties(uint32_t width = 1200, uint32_t height = 800, int renderAPI = 0, const std::string& title = "Game", bool fullscreen = false, bool vSync = true, bool borderless = false, const std::string& filepath = "")
			: Width(width)
			, Height(height)
			, Title(title)
			, Fullscreen(fullscreen)
			, VSync(vSync)
			, Borderless(borderless)
			, RenderAPI(renderAPI)
			, FilePath(filepath)
		{
		}

		uint32_t Width, Height;
		bool Fullscreen;
		bool VSync;
		bool Borderless;
		bool ShowConsole = true;
		std::string Title;
		int RenderAPI;
		std::string FilePath;
	};

	class EASEL_EXPORT Window {
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		static Window* Create(const WindowProperties& properties);
		virtual ~Window() = default;
		bool Initialise(const WindowProperties& properties);

		bool HasInitialised() const {
			return m_Init;
		};

		virtual void SetExit(bool exit) = 0;
		virtual bool GetExit() const = 0;
		
		_FORCE_INLINE_ void SetHasResized(bool resized) {
			m_HasResized = resized;
		}
		_FORCE_INLINE_ bool GetHasResized() const {
			return m_HasResized;
		}

		virtual void ToggleVSync() = 0;
		virtual void SetVSync(bool set) = 0;
		virtual void SetWindowTitle(const std::string& title) = 0;
		virtual void OnUpdate() = 0;
		virtual void SetBorderlessWindow(bool borderless) = 0;
		virtual void* GetHandle() { return nullptr; };
		virtual float GetScreenRatio() const = 0;
		virtual void HideMouse(bool hide) {};
		virtual void SetMousePosition(const glm::vec2& pos) {};
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void UpdateCursorImGui() = 0;
		virtual void SetIcon(const std::string& filePath, const std::string& smallIconFilePath = "") = 0;

		virtual std::string GetTitle() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

	protected:
		static Window* (*CreateFunc)(const WindowProperties&);

		Window() = default;

		bool m_Init = false;
		glm::vec2 m_Position = {0.0f, 0.0f};
		bool m_VSync = false;
		bool m_HasResized = false;
	};
}
