#include "WindowManager.h"

WindowManager::WindowManager(const std::string& title, const sf::Vector2u& size)
{
	m_windowTitle = title;
	m_windowSize = size;
	m_isFullscreen = false;
	m_isOpen = true;
	createWindow();
	m_inputManager.addCallback(StateType(0), "Key_Escape", &WindowManager::closeWindow, this);
	m_inputManager.addCallback(StateType(0), "Fullscreen_toggle", &WindowManager::toggleFullscreen, this);
}

WindowManager::~WindowManager()
{
	m_window.close();
}

void WindowManager::update()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			closeWindow();
		else if (event.type == sf::Event::GainedFocus)
			m_isFocused = m_inputManager.setFocus(true);
		else if (event.type == sf::Event::LostFocus)
			m_isFocused = m_inputManager.setFocus(false);
		else
			m_inputManager.handleEvent(event);
	}
	m_inputManager.update();
}

void WindowManager::drawStart()
{
	m_window.clear();
}

void WindowManager::draw(sf::Drawable& drawable)
{
	m_window.draw(drawable);
}

void WindowManager::drawEnd()
{
	m_window.display();
}

bool WindowManager::isOpen()
{
	return m_isOpen;
}

bool WindowManager::isFocused()
{
	return m_isFocused;
}

sf::RenderWindow* WindowManager::getRenderWindow()
{
	return &m_window;
}

sf::Vector2u WindowManager::getWindowSize()
{
	return m_windowSize;
}

sf::FloatRect WindowManager::getViewSpace()
{
	sf::Vector2f viewCenter = m_window.getView().getCenter();
	sf::Vector2f viewSize = m_window.getView().getSize();
	return sf::FloatRect(viewCenter - viewSize / 2.f, viewSize);
}

InputManager* WindowManager::getInputManager()
{
	return &m_inputManager;
}

void WindowManager::toggleFullscreen(EventDetails* details)
{
	m_isFullscreen = !m_isFullscreen;
	m_window.close();
	createWindow();
}

void WindowManager::createWindow()
{
	sf::VideoMode videoMode(m_windowSize.x, m_windowSize.y);
	m_window.create(videoMode, m_windowTitle, m_isFullscreen ? sf::Style::Fullscreen : sf::Style::Default);
}

void WindowManager::closeWindow(EventDetails* details)
{
	m_isOpen = false;
}
