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
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			closeWindow();
		else if (event.type == sf::Event::GainedFocus)
			m_isFocused = m_controller.setFocus(true);
		else if (event.type == sf::Event::LostFocus)
			m_isFocused = m_controller.setFocus(false);
		else
			m_controller.handleEvent(event);
	}
	m_controller.update();
}

void WindowManager::drawStart()
{
	m_window.clear(BGD_COLOR);
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
	sf::VideoMode videoMode(m_windowSize.x, m_windowSize.y);
	m_window.create(videoMode, m_windowTitle, m_isFullscreen ? sf::Style::Fullscreen : sf::Style::Default);
}

void WindowManager::closeWindow()
{
	m_isOpen = false;
}
