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

	void handleEvent(sf::Event event, const float& deltaTime);

	void setWindowManager(WindowManager* windowManager);
	bool setFocus(const bool& focus);
	void centerMouse();

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
	sf::Vector2f m_movementInput;
	sf::Vector2f m_screenCenter;
};
