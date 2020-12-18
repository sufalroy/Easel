#pragma once

#include "Easel/Events/Event.h"
#include "Easel/Core/OS/KeyCodes.h"

#include <sstream>

namespace Easel {

	class EASEL_EXPORT KeyEvent : public Event {
	public:
		_FORCE_INLINE_ KeyCode GetKeyCode() const { return m_KeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(const KeyCode keycode) : m_KeyCode(keycode) {}
		KeyCode m_KeyCode;
	};

	class EASEL_EXPORT KeyPressedEvent : public KeyEvent {
	public:
		KeyPressedEvent(const KeyCode keycode, uint16_t repeatCount)
			: KeyEvent(keycode), m_RepeatCount(repeatCount) {}
		
		_FORCE_INLINE_ uint16_t GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount << "repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		uint16_t m_RepeatCount;
	};

	class EASEL_EXPORT KeyReleasedEvent : public KeyEvent {
	public:
		KeyReleasedEvent(const KeyCode keycode) : KeyEvent(keycode) {}
		
		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class EASEL_EXPORT KeyTypedEvent : public KeyEvent {
	public:
		KeyTypedEvent(const KeyCode keycode) : KeyEvent(keycode) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "KeyTypedEvent: " << m_KeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}
