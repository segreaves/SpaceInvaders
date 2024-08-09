#pragma once
#include <SFML/Graphics.hpp>
#include "InputEvent.h"
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

	InputEvent<void> m_onEscape;
	InputEvent<void> m_onShoot;
	InputEvent<void> m_onPause;
	InputEvent<void> m_onToggleFullscreen;
	InputEvent<sf::Vector2f> m_onMove;
	InputEvent<sf::Vector2i> m_onMouseClick;
private:
	bool m_hasFocus;
	WindowManager* m_windowManager;
};
