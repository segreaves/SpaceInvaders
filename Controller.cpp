#include "Controller.h"
#include "WindowManager.h"
#include <iostream>

Controller::Controller() :
	m_hasFocus(true),
	m_windowManager(nullptr)
{
}

void Controller::handleEvent(sf::Event event, const float& deltaTime)
{ 
	if (event.is<sf::Event::KeyPressed>())
	{
		if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Space)
			m_onSelect.dispatch();
		else if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)
			m_onEscape.dispatch();
		else if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::P)
		{
			// release the mouse on paused state
			m_captureMouse = !m_captureMouse;
			m_onPause.dispatch();
		}
		else if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::H)
			m_onToggleHelp.dispatch();
		else if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::S)
			m_onToggleSound.dispatch();
		else if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::M)
			m_onToggleMusic.dispatch();
		else if (event.getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::F5)
			m_onToggleFullscreen.dispatch();
	}
	else if (event.is<sf::Event::MouseButtonPressed>())
	{
		if (event.getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
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

void Controller::centerMouse()
{
	sf::RenderWindow* window = m_windowManager->getRenderWindow();
	sf::Mouse::setPosition(sf::Vector2i(window->getSize().x / 2, window->getSize().y / 2), *window);
}
