#pragma once

#include "Easel/Events/Event.h"
#include "Easel/Core/OS/MouseCodes.h"

#include <sstream>

namespace Easel {

	class EASEL_EXPORT MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(const float x, const float y) : m_MouseX(x), m_MouseY(y) {}

		_FORCE_INLINE_ float GetX() const { return m_MouseX; }
		_FORCE_INLINE_ float GetY() const { return m_MouseY; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseMovedEvent: " << m_MouseX << ", " << m_MouseY;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseMoved)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_MouseX, m_MouseY;
	};

	class EASEL_EXPORT MouseScrolledEvent : public Event {
	public:
		MouseScrolledEvent(const float xOffset, const float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		_FORCE_INLINE_ float GetXOffset() const { return m_XOffset; }
		_FORCE_INLINE_ float GetYOffset() const { return m_YOffset; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseScrolledEvent: " << GetXOffset() << ", " << GetYOffset();
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseScrolled)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	private:
		float m_XOffset, m_YOffset;
	};

	class EASEL_EXPORT MouseButtonEvent : public Event {
	public:
		_FORCE_INLINE_ MouseCode GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
	protected:
		MouseButtonEvent(const MouseCode button) : m_Button(button) {}
		MouseCode m_Button;
	};

	class EASEL_EXPORT MouseButtonPressedEvent : public MouseButtonEvent {
	public:
		MouseButtonPressedEvent(const MouseCode button) : MouseButtonEvent(button) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class EASEL_EXPORT MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const MouseCode button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << m_Button;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

	class EASEL_EXPORT MouseEnterEvent : public Event {
	public:
		MouseEnterEvent(const bool enter) : m_Entered(enter) {}

		_FORCE_INLINE_ bool GetEntered() const { return m_Entered; }

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseEnterEvent: " << m_Entered;
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseEntered)
		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)
	protected:
		bool m_Entered;
	};
}

