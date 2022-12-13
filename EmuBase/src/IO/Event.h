#pragma once

#include "KeyCodes.h"

namespace Emu {

	enum class EventType
	{
		None = -1,
		WindowClose, WindowResize, WindowFocus, WindowLostFoucus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	enum EventCategory
	{
		None					 = 0,
		EventCategoryApplication = 1 << 0,
		EventCategoryInput		 = 1 << 1,
		EventCategoryKeyboard	 = 1 << 2,
		EventCategoryMouse		 = 1 << 3,
		EventCategoryMouseButton = 1 << 4
	};

#define EVENT_CLASS_TYPE(type)  static EventType GetStaticType() { return EventType::type; }\
								virtual EventType getEventType() const override { return GetStaticType(); }\
								virtual const char* getName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int getCategoryFlags() const override { return category; }

#define BIND_EVENT_FUNC(func)  [this](auto&&... args) -> decltype(auto) { return this->func(std::forward<decltype(args)>(args)...); }

	class Event
	{
	public:
		virtual ~Event() = default;

		virtual EventType getEventType() const = 0;
		virtual const char* getName() const = 0;
		virtual int getCategoryFlags() const = 0;
		virtual std::string toString() const { return getName(); }

		bool isInCategory(EventCategory category)
		{
			return getCategoryFlags() & category;
		}

	public:
		bool handled = false;

		friend class EventDispatcher;

	private:
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: mEvent(event) {}

		//F deduced by compiler
		template<typename T, typename F>
		bool dispatch(const F& func)
		{
			if (mEvent.getEventType() == T::GetStaticType())
			{
				mEvent.handled = func(static_cast<T&>(mEvent));
				return true;
			}
			return false;
		}

	private:
		Event& mEvent;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.toString();
	}

	/*
		Event Specialisations
	*/

	class KeyEvent : public Event
	{
	public:
		KeyCode getKeyCode() const { return mKeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

	protected:
		KeyEvent(const KeyCode keycode)
			: mKeyCode(keycode) {}

		KeyCode mKeyCode;
	};

	class KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode, bool repeat = false)
			: KeyEvent(keycode), mIsRepeat(repeat) {}

		bool isRepeated() const { return mIsRepeat; }

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << mKeyCode << " (" << mIsRepeat << " repeat(s))";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)

	private:
		bool mIsRepeat;
	};

	class KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {}

		std::string toString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << mKeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_CLASS_TYPE(WindowClose)
		EVENT_CLASS_CATEGORY(EventCategoryApplication)
	};
}