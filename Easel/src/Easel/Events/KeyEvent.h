#pragma once

#include "Easel/Events/Event.h"
#include "Easel/Core/OS/KeyCodes.h"

#include <sstream>

namespace Easel {

	class EASEL_EXPORT KeyEvent : public Event {
	public:
		inline InputCode::Key GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(const InputCode::Key keycode) : m_KeyCode(keycode) {}
		InputCode::Key m_KeyCode;
	};

	class EASEL_EXPORT KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(const InputCode::Key keycode, int repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) {}
		
		inline int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyPressedEvent: " << uint32_t(m_KeyCode) << " (" << m_RepeatCount << "repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int m_RepeatCount;
	};

	class EASEL_EXPORT KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(const InputCode::Key keycode) : KeyEvent(keycode) {}
		
		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << uint32_t(m_KeyCode);
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class EASEL_EXPORT KeyTypedEvent : public KeyEvent {
	public:
		KeyTypedEvent(const InputCode::Key keycode, char character) 
			: KeyEvent(keycode), Character(character) {

		}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyTypedEvent: " << uint32_t(m_KeyCode);
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
		char Character;
	};
}
