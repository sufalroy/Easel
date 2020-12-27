#include "Precompiled.h"
#include "Easel/Core/OS/Input.h"

namespace Easel {
	
	Input* Input::s_Input;

	void Input::Create() {
		s_Input = new Input();
	}

	Input::Input() : m_WindowFocus(true) {

		Reset();
	}

	void Input::Reset() {
		memset(m_KeyHeld, 0, MAX_KEYS);
		memset(m_KeyPressed, 0, MAX_KEYS);
		memset(m_MouseClicked, 0, MAX_BUTTONS);
		memset(m_MouseHeld, 0, MAX_BUTTONS);

		m_MouseOnScreen = true;
		m_WindowFocus = true;
		m_ScrollOffset = 0.0f;
	}

	void Input::ResetPressed() {
		memset(m_KeyPressed, 0, MAX_KEYS);
		memset(m_MouseClicked, 0, MAX_BUTTONS);
		m_ScrollOffset = 0;
	}

	void Input::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(Input::OnKeyPressed));
		dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(Input::OnKeyReleased));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(Input::OnMousePressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(Input::OnMouseReleased));
		dispatcher.Dispatch<MouseScrolledEvent>(BIND_EVENT_FN(Input::OnMouseScrolled));
		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(Input::OnMouseMoved));
		dispatcher.Dispatch<MouseEnterEvent>(BIND_EVENT_FN(Input::OnMouseEnter));
	}

	bool Input::OnKeyPressed(KeyPressedEvent& e) {
		SetKeyPressed(InputCode::Key(e.GetKeyCode()), e.GetRepeatCount() < 1);
		SetKeyHeld(InputCode::Key(e.GetKeyCode()), true);
		return false;
	}

	bool Input::OnKeyReleased(KeyReleasedEvent& e) {
		SetKeyPressed(InputCode::Key(e.GetKeyCode()), false);
		SetKeyHeld(InputCode::Key(e.GetKeyCode()), false);
		return false;
	}

	bool Input::OnMousePressed(MouseButtonPressedEvent& e) {
		SetMouseClicked(InputCode::MouseKey(e.GetMouseButton()), true);
		SetMouseHeld(InputCode::MouseKey(e.GetMouseButton()), true);
		return false;
	}

	bool Input::OnMouseReleased(MouseButtonReleasedEvent& e) {
		SetMouseClicked(InputCode::MouseKey(e.GetMouseButton()), false);
		SetMouseHeld(InputCode::MouseKey(e.GetMouseButton()), false);
		return false;
	}

	bool Input::OnMouseScrolled(MouseScrolledEvent& e) {
		SetScrollOffset(e.GetYOffset());
		return false;
	}

	bool Input::OnMouseMoved(MouseMovedEvent& e) {
		StoreMousePosition(e.GetX(), e.GetY());
		return false;
	}

	bool Input::OnMouseEnter(MouseEnterEvent& e) {
		SetMouseOnScreen(e.GetEntered());
		return false;
	}
}
