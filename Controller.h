#pragma once
#include <SFML/Graphics.hpp>
#include "Trigger.h"
#include <unordered_map>
#include <functional>

class WindowManager;

class Controller
{
public:
	Controller();

	void update();
	void handleEvent(sf::Event event);

	void setWindowManager(WindowManager* windowManager);
	bool setFocus(const bool& focus);

	Trigger<void> m_onEscape;
	Trigger<void> m_onShoot;
	Trigger<void> m_onPause;
	Trigger<void> m_onSelect;
	Trigger<void> m_onToggleFullscreen;
	Trigger<void> m_onToggleHelp;
	Trigger<void> m_onToggleSound;
	Trigger<void> m_onToggleMusic;
	Trigger<sf::Vector2f> m_onMove;
private:
	bool m_hasFocus;
	bool m_captureMouse = true;
	WindowManager* m_windowManager;
	const float m_mouseSensitivity = 1;
	sf::Vector2f m_movementInput;
	sf::Vector2f m_screenCenter;
};
