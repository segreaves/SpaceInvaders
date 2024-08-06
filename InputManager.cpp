#include "InputManager.h"
#include "Utilities.h"
#include <fstream>
#include <sstream>

InputManager::InputManager() :
	m_hasFocus(true)
{
	loadBindings();
}

void InputManager::update()
{
	if (!m_hasFocus) return;
	auto stateCallbacks = m_callbacks.find(m_currentState);
	if (stateCallbacks != m_callbacks.end())
	{
		for (auto bindingIt : m_bindings)
		{
			Binding* binding = bindingIt.second;
			auto callback = stateCallbacks->second.find(binding->m_name);
			if (callback != stateCallbacks->second.end())
				checkEvents(binding, callback->second);
		}
	}
	auto globalCallbacks = m_callbacks.find(StateType(0));
	if (globalCallbacks != m_callbacks.end())
	{
		for (auto bindingIt : m_bindings)
		{
			Binding* binding = bindingIt.second;
			auto callback = globalCallbacks->second.find(binding->m_name);
			if (callback != globalCallbacks->second.end())
				checkEvents(binding, callback->second);
		}
	}
}

void InputManager::handleEvent(sf::Event& event)
{
	if (!m_hasFocus) return;
	auto stateCallbacks = m_callbacks.find(m_currentState);
	if (stateCallbacks != m_callbacks.end())
	{
		for (auto bindingIt : m_bindings)
		{
			Binding* binding = bindingIt.second;
			if (binding->m_event.first != (EventType)event.type) continue;
			auto callback = stateCallbacks->second.find(binding->m_name);
			if (callback != stateCallbacks->second.end())
				checkWindowEvents(binding, event, callback->second);
		}
	}
	auto globalCallbacks = m_callbacks.find(StateType(0));
	if (globalCallbacks != m_callbacks.end())
	{
		for (auto bindingIt : m_bindings)
		{
			Binding* binding = bindingIt.second;
			auto callback = globalCallbacks->second.find(binding->m_name);
			if (callback != globalCallbacks->second.end())
				checkWindowEvents(binding, event, callback->second);
		}
	}
}

void InputManager::checkEvents(Binding* binding, std::function<void(EventDetails*)> func)
{
	EventType eventType = binding->m_event.first;
	EventInfo eventInfo = binding->m_event.second;
	if (eventType == EventType::Keyboard)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key(eventInfo.m_code)))
			func(&binding->m_details);
	}
	else if (eventType == EventType::Mouse)
	{
		if (sf::Mouse::isButtonPressed(sf::Mouse::Button(eventInfo.m_code)))
			func(&binding->m_details);
	}
	else if (eventType == EventType::Joystick)
	{
		if (sf::Joystick::isConnected(0))
		{
			float deadZone = 10.f;
			float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
			float y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
			if (sqrt(pow(x, 2) + pow(y, 2)) <= deadZone)
			{
				binding->m_details.m_joystick.x = 0.f;
				binding->m_details.m_joystick.y = 0.f;
			}
			else
			{
				binding->m_details.m_joystick.x = x;
				binding->m_details.m_joystick.y = y;
			}
			func(&binding->m_details);
		}
	}
}

void InputManager::checkWindowEvents(Binding* binding, sf::Event& event, std::function<void(EventDetails*)> func)
{
	EventType eventType = binding->m_event.first;
	EventInfo eventInfo = binding->m_event.second;
	if (eventType == EventType::KeyDown || eventType == EventType::KeyUp)
	{
		if (eventInfo.m_code == event.key.code)
			func(&binding->m_details);
	}
	else if (eventType == EventType::MouseButtonDown || eventType == EventType::MouseButtonUp)
	{
		if (eventInfo.m_code == event.mouseButton.button)
		{
			binding->m_details.m_mouse.x = event.mouseButton.x;
			binding->m_details.m_mouse.y = event.mouseButton.y;
			func(&binding->m_details);
		}
	}
}

bool InputManager::addBinding(Binding* binding)
{
	if (m_bindings.find(binding->m_name) != m_bindings.end()) return false;
	else return m_bindings.emplace(binding->m_name, binding).second;
}

bool InputManager::removeBinding(std::string name)
{
	auto binding = m_bindings.find(name);
	if (binding == m_bindings.end()) return false;
	delete binding->second;
	m_bindings.erase(binding);
	return true;
}

void InputManager::loadBindings()
{
	std::string delimiter = ":";
	std::ifstream file;
	std::string path = Utilities::getWorkingDirectory() + "assets/controls/keys.cfg.txt";
	file.open(path);
	if (!file.is_open())
	{
		std::cerr << "! InputManager: Failed loading " << path << std::endl;
		return;
	}
	std::string line;
	while (std::getline(file, line))
	{
		std::stringstream keystream(line);
		std::string callback;
		keystream >> callback;
		Binding* binding = new Binding(callback);
		while (!keystream.eof())
		{
			std::string keyValues;
			keystream >> keyValues;
			int start = 0;
			int end = keyValues.find(delimiter);
			if (end == std::string::npos)
			{
				delete binding;
				binding = nullptr;
				break;
			}
			EventType eventType = EventType(stoi(keyValues.substr(start, end - start)));
			int code = stoi(keyValues.substr(
					end + delimiter.length(),
					keyValues.find_first_not_of(delimiter, end + delimiter.length())));
			EventInfo eventInfo;
			eventInfo.m_code = code;
			binding->bindEvent(eventType, eventInfo);
		}
		if (!addBinding(binding)) { delete binding; }
		binding = nullptr;
	}
	file.close();
}

bool InputManager::removeCallback(StateType state, const std::string& name)
{
	auto stateIt = m_callbacks.find(state);
	if (stateIt == m_callbacks.end()) return false;
	auto callIt = stateIt->second.find(name);
	if (callIt == stateIt->second.end()) return false;
	stateIt->second.erase(name);
	return true;
}

bool InputManager::setFocus(const bool& focus)
{
	return m_hasFocus = focus;
}

void InputManager::setCurrentState(const StateType& state)
{
	m_currentState = state;
}
