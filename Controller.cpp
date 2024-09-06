#include "Controller.h"
#include "WindowManager.h"
#include <iostream>

Controller::Controller() :
	m_windowManager(nullptr),
	m_hasFocus(true)
{
}

void Controller::update()
{
	// set mouse to center of screen
	sf::RenderWindow* window = m_windowManager->getRenderWindow();
	sf::Mouse::setPosition(sf::Vector2i(window->getSize().x / 2, window->getSize().y / 2), *window);
}

void Controller::handleEvent(sf::Event event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		if (event.key.code == sf::Keyboard::Space)
			m_onShoot.dispatch();
		else if (event.key.code == sf::Keyboard::Escape)
			m_onEscape.dispatch();
		else if (event.key.code == sf::Keyboard::P)
			m_onPause.dispatch();
		else if (event.key.code == sf::Keyboard::F5)
			m_onToggleFullscreen.dispatch();
	}
	else if (event.type == sf::Event::MouseMoved)
	{
		float moveAmount = event.mouseMove.x - m_screenCenter.x;
		m_onMove.dispatch(m_mouseSensitivity * sf::Vector2f(moveAmount, 0));
	}
	else if (event.type == sf::Event::MouseButtonPressed)
	{
		if (event.mouseButton.button == sf::Mouse::Left)
			m_onShoot.dispatch();
	}
}

void Controller::setWindowManager(WindowManager* windowManager)
{
	m_windowManager = windowManager;
	sf::Vector2f windowSize = sf::Vector2f(m_windowManager->getWindowSize());
	m_screenCenter = sf::Vector2f(windowSize.x / 2, windowSize.y / 2);
}

bool Controller::setFocus(const bool& focus)
{
	m_hasFocus = focus;
	return m_hasFocus;
}
