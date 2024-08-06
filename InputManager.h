#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>

enum class EventType
{
	KeyDown = sf::Event::KeyPressed,
	KeyUp = sf::Event::KeyReleased,
	MouseButtonDown = sf::Event::MouseButtonPressed,
	MouseButtonUp = sf::Event::MouseButtonReleased,
	WindowResized = sf::Event::Resized,
	GainedFocus = sf::Event::GainedFocus,
	LostFocus = sf::Event::LostFocus,
	MouseLeft = sf::Event::MouseLeft,
	Closed = sf::Event::Closed,
	JoystickButtonDown = sf::Event::JoystickButtonPressed,
	Keyboard = sf::Event::Count + 1,
	Mouse,
	Joystick
};

struct EventInfo
{
	EventInfo() { m_code = 0; }
	EventInfo(int code) { m_code = code; }
	int m_code;
};

struct EventDetails
{
	EventDetails(const std::string name) { m_name = name; }
	void initialize()
	{
		m_mouse = sf::Vector2i(0, 0);
		m_joystick = sf::Vector2f(0, 0);
		m_keyCode = -1;
	}
	std::string m_name;
	sf::Vector2i m_mouse;
	sf::Vector2f m_joystick;
	int m_keyCode;
};

struct Binding
{
	Binding(const std::string name) :
		m_active(false),
		m_name(name),
		m_details(name) {}
	void bindEvent(const EventType& type, EventInfo info = EventInfo())
	{
		m_event = std::pair<EventType, EventInfo>(type, info);
	}
	bool m_active;
	std::string m_name;
	std::pair<EventType, EventInfo> m_event;
	EventDetails m_details;
};

enum class StateType;
class WindowManager;

class InputManager
{
public:
	InputManager();

	void update();
	void handleEvent(sf::Event& event);
	void checkEvents(Binding* binding, std::function<void(EventDetails*)> func);
	void checkWindowEvents(Binding* binding, sf::Event& event, std::function<void(EventDetails*)> func);

	bool setFocus(const bool& focus);
	void setCurrentState(const StateType& state);

	bool addBinding(Binding* binding);
	bool removeBinding(const std::string name);
	void loadBindings();

	template<class T>
	bool addCallback(StateType state, const std::string& name, void(T::*func)(EventDetails*), T* instance)
	{
		auto it = m_callbacks.emplace(state, std::unordered_map<std::string, std::function<void(EventDetails*)>>()).first;
		auto temp = std::bind(func, instance, std::placeholders::_1);
		return it->second.emplace(name, temp).second;
	}
	bool removeCallback(StateType state, const std::string& name);
private:
	bool m_hasFocus;
	std::unordered_map<std::string, Binding*> m_bindings;
	std::unordered_map<StateType, std::unordered_map<std::string, std::function<void(EventDetails*)>>> m_callbacks;
	StateType m_currentState;
};
