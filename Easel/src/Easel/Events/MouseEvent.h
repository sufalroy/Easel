#pragma once

#include "Easel/Events/Event.h"
#include "Easel/Core/OS/MouseCodes.h"

#include <sstream>

namespace Easel {

	class EASEL_EXPORT MouseMovedEvent : public Event {
	public:
		MouseMovedEvent(const float x, const float y) : m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

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

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

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
		inline InputCode::MouseKey GetMouseButton() const { return m_Button; }

		EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)
	protected:
		MouseButtonEvent(const InputCode::MouseKey button) : m_Button(button) {}
		InputCode::MouseKey m_Button;
	};

	class EASEL_EXPORT MouseButtonPressedEvent : public MouseButtonEvent {
	public:
		MouseButtonPressedEvent(const InputCode::MouseKey button) : MouseButtonEvent(button) {}

		std::string ToString() const override {
			std::stringstream ss;
			ss << "MouseButtonPressedEvent: " << uint32_t(m_Button);
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonPressed)
	};

	class EASEL_EXPORT MouseButtonReleasedEvent : public MouseButtonEvent
	{
	public:
		MouseButtonReleasedEvent(const InputCode::MouseKey button) : MouseButtonEvent(button) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent: " << uint32_t(m_Button);
			return ss.str();
		}

		EVENT_CLASS_TYPE(MouseButtonReleased)
	};

	class EASEL_EXPORT MouseEnterEvent : public Event {
	public:
		MouseEnterEvent(const bool enter) : m_Entered(enter) {}

		inline bool GetEntered() const { return m_Entered; }

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

