#include "Controller.h"
#include "WindowManager.h"
#include <iostream>

Controller::Controller() :
	m_hasFocus(true)
{
}

void Controller::update()
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			m_onMove.dispatch(sf::Vector2f(0.f, -1.f));
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			m_onMove.dispatch(sf::Vector2f(0.f, 1.f));
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			m_onMove.dispatch(sf::Vector2f(-1.f, 0.f));
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
			m_onMove.dispatch(sf::Vector2f(1.f, 0.f));
	}
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button(sf::Mouse::Left)))
	{
		sf::Vector2i mouseXY = sf::Mouse::getPosition(*m_windowManager->getRenderWindow());
		m_onMouseClick.dispatch(mouseXY);
	}
	if (sf::Joystick::isConnected(0))
	{
		float deadZone = 10.f;
		float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
		float y = sf::Joystick::getAxisPosition(0, sf::Joystick::Y);
		sf::Vector2f joystickXY(x, y);
		if (sqrt(pow(x, 2) + pow(y, 2)) <= deadZone)
		{
			joystickXY.x = 0.f;
			joystickXY.y = 0.f;
		}
		else
		{
			joystickXY.x = x / 100.f;
			joystickXY.y = y / 100.f;
		}
		m_onMove.dispatch(joystickXY);
	}
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
	else if (event.type == sf::Event::MouseButtonPressed)
	{
		/*if (event.mouseButton.button == sf::Mouse::Left)
			m_onShoot.dispatch();*/
	}
}

void Controller::setWindowManager(WindowManager* windowManager)
{
	m_windowManager = windowManager;
}

bool Controller::setFocus(const bool& focus)
{
	m_hasFocus = focus;
	return m_hasFocus;
}
