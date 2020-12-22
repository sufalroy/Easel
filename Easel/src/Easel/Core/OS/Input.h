#pragma once
#include "Easel/Events/Event.h"
#include "Easel/Events/KeyEvent.h"
#include "Easel/Events/MouseEvent.h"

#include "Easel/Core/OS/KeyCodes.h"
#include "Easel/Core/OS/MouseCodes.h"

#include <glm/glm.hpp>

#define MAX_KEYS 1024
#define MAX_BUTTONS 32

namespace Easel {

	class Event;

	class EASEL_EXPORT Input {
	public:
		static Input* GetInput() { return s_Input; }

		static void Create();
		static void Release() { delete s_Input; }

		bool GetKeyPressed(KeyCode key)   const { return m_KeyPressed[int(key)]; }
		bool GetKeyHeld(KeyCode key)      const { return m_KeyHeld[int(key)]; }
		bool GetMouseClicked(MouseCode key) const { return m_MouseClicked[int(key)]; }
		bool GetMouseHeld(MouseCode key)    const { return m_MouseHeld[int(key)]; }

		void SetKeyPressed(KeyCode key, bool a) { m_KeyPressed[int(key)] = a; }
		void SetKeyHeld(KeyCode key, bool a) { m_KeyHeld[int(key)] = a; }
		void SetMouseClicked(MouseCode key, bool a) { m_MouseClicked[int(key)] = a; }
		void SetMouseHeld(MouseCode key, bool a) { m_MouseHeld[int(key)] = a; }

		void SetMouseOnScreen(bool onScreen) { m_MouseOnScreen = onScreen; }
		bool GetMouseOnScreen() const { return m_MouseOnScreen; }

		void StoreMousePosition(float xpos, float ypos) { m_MousePosition = glm::vec2(float(xpos), float(ypos)); }
		const glm::vec2& GetMousePosition() const { return m_MousePosition; }

		void SetWindowFocus(bool focus) { m_WindowFocus = focus; }
		bool GetWindowFocus() const { return m_WindowFocus; }

		void SetScrollOffset(float offset) { m_ScrollOffset = offset; }
		float GetScrollOffset() const { return m_ScrollOffset; }

		void Reset();
		void ResetPressed();
		void OnEvent(Event& e);

	protected:
		Input();
		virtual ~Input() = default;

		static Input* s_Input;

		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnKeyReleased(KeyReleasedEvent& e);
		bool OnMousePressed(MouseButtonPressedEvent& e);
		bool OnMouseReleased(MouseButtonReleasedEvent& e);
		bool OnMouseScrolled(MouseScrolledEvent& e);
		bool OnMouseMoved(MouseMovedEvent& e);
		bool OnMouseEnter(MouseEnterEvent& e);

		bool m_KeyPressed[MAX_KEYS];
		bool m_KeyHeld[MAX_KEYS];

		bool m_MouseHeld[MAX_BUTTONS];
		bool m_MouseClicked[MAX_BUTTONS];

		float m_ScrollOffset = 0.0f;

		bool m_MouseOnScreen;
		bool m_WindowFocus;

		glm::vec2 m_MousePosition;
	};
}