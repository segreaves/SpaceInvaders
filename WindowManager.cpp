#include "WindowManager.h"
#include "Params.h"

WindowManager::WindowManager(const std::string& title, const sf::Vector2u& size)
{
	m_windowTitle = title;
	m_windowSize = size;
	m_isFullscreen = true;
	m_isOpen = true;
	createWindow();
	m_controller.setWindowManager(this);
	m_controller.m_onEscape.addCallback("WindowManager_closeWindow", std::bind(&WindowManager::closeWindow, this));
	m_controller.m_onToggleFullscreen.addCallback("WindowManager_toggleFullscreen", std::bind(&WindowManager::toggleFullscreen, this));
}

WindowManager::~WindowManager()
{
	m_window.close();
}

void WindowManager::update(float deltaTime)
{

	while (const std::optional event = m_window.pollEvent())
	{
		if (event->getIf<sf::Event::Closed>())
			closeWindow();
		else if (event->getIf<sf::Event::FocusGained>())
			m_isFocused = m_controller.setFocus(true);
		else if (event->getIf<sf::Event::FocusLost>())
			m_isFocused = m_controller.setFocus(false);
		else
			m_controller.handleEvent(*event, deltaTime);
	}
}

void WindowManager::clear()
{
	m_window.clear(BGD_COLOR);
}

void WindowManager::display()
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

sf::FloatRect WindowManager::getCurrentViewSpace()
{
	sf::Vector2f viewCenter = m_window.getView().getCenter();
	sf::Vector2f viewSize = m_window.getView().getSize();
	return sf::FloatRect(viewCenter - viewSize / 2.f, viewSize);
}

Controller* WindowManager::getController()
{
	return &m_controller;
}

void WindowManager::toggleFullscreen()
{
	m_isFullscreen = !m_isFullscreen;
	m_window.close();
	createWindow();
}

void WindowManager::createWindow()
{
	sf::VideoMode videoMode({m_windowSize.x, m_windowSize.y});
	m_window.create(videoMode, m_windowTitle, sf::Style::Default, m_isFullscreen ? sf::State::Fullscreen : sf::State::Windowed);
	m_window.setFramerateLimit(m_framerate);
}

void WindowManager::closeWindow()
{
	m_isOpen = false;
}
