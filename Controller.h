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
	Trigger<void> m_onToggleFullscreen;
	Trigger<sf::Vector2f> m_onMove;
	Trigger<sf::Vector2i> m_onMouseClick;
private:
	bool m_hasFocus;
	WindowManager* m_windowManager;
};
