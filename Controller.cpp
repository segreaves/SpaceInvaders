#include "Controller.h"
#include "WindowManager.h"

Controller::Controller() :
	m_windowManager(nullptr),
	m_hasFocus(true)
{
}

void Controller::update()
{
	sf::Vector2f movementInput(0.f, 0.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
		movementInput += sf::Vector2f(-1.f, 0.f);
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
		movementInput += sf::Vector2f(1.f, 0.f);
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button(sf::Mouse::Left)))
	{
		sf::Vector2i mouseXY = sf::Mouse::getPosition(*m_windowManager->getRenderWindow());
		m_onMouseClick.dispatch(mouseXY);
	}
	if (sf::Joystick::isConnected(0))
	{
		float deadZone = 10.f;
		float x = sf::Joystick::getAxisPosition(0, sf::Joystick::X);
		sf::Vector2f joystickXY(x, 0);
		if (abs(x) <= deadZone)
			joystickXY.x = 0.f;
		else
			joystickXY.x = x / 100.f;
		movementInput += joystickXY;
	}
	m_onMove.dispatch(movementInput);
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
