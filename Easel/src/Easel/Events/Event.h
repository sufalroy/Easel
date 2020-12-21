#pragma once

#include "Easel/Core/Base.h"

#include <string>
#include <functional>

namespace Easel {

	enum class EventType {
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled, MouseEntered
	};

	enum EventCategory {
		None = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput       = BIT(1),
		EventCategoryKeyboard    = BIT(2),
		EventCategoryMouse       = BIT(3),
		EventCategoryMouseButton = BIT(4),
	};

#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#define EVENT_CLASS_TYPE(type)	static EventType GetStaticType() {return EventType::type; }\
								virtual EventType GetEventType() const override {return GetStaticType();}\
								virtual const char* GetName() const override {return #type;}

#define EVENT_CLASS_CATEGORY(category) virtual uint32_t GetCategoryFlags() const override { return category; }

	class EASEL_EXPORT Event {
	public:
		virtual ~Event() = default;
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual uint32_t GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		_FORCE_INLINE_ bool IsInCategory(EventCategory category) {
			return GetCategoryFlags() & category;
		}

		_FORCE_INLINE_ bool Handled() const { return m_Handled; }

	protected:
		bool m_Handled = false;
	};

	class EventDispatcher {
	public:
		EventDispatcher(Event& event) : m_Event(event) 
		{
		}

		template<typename T, typename F>
		bool Dispatch(const F& func) {
			if (m_Event.GetEventType() == T::GetStaticType()) {

				m_Event.Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};

	_FORCE_INLINE_ std::ostream& operator<<(std::ostream& os, const Event& e) {
		return os << e.ToString();
	}
}
