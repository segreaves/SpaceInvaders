#include "WindowManager.h"

WindowManager::WindowManager(const std::string& title, const sf::Vector2u& size)
{
	m_windowTitle = title;
	m_windowSize = size;
	m_isFullscreen = true;
	m_isOpen = true;
	m_hudView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
	m_gameplayView.setViewport(sf::FloatRect(0.2f, 0.f, 0.6f, 1.f));
	createWindow();
	m_controller.setWindowManager(this);
	m_controller.m_onEscape.addCallback("WindowManager_closeWindow", std::bind(&WindowManager::closeWindow, this));
	m_controller.m_onToggleFullscreen.addCallback("WindowManager_toggleFullscreen", std::bind(&WindowManager::toggleFullscreen, this));
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
			m_isFocused = m_controller.setFocus(true);
		else if (event.type == sf::Event::LostFocus)
			m_isFocused = m_controller.setFocus(false);
		else
			m_controller.handleEvent(event);
	}
	m_controller.update();
	// set mouse to center of screen
	sf::Mouse::setPosition(sf::Vector2i(m_windowSize.x / 2, m_windowSize.y / 2), m_window);
}

void WindowManager::drawStart()
{
	m_window.clear(sf::Color(0, 50, 50, 255));
	m_window.setView(m_gameplayView);
}

void WindowManager::drawToGameplayView(sf::Drawable& drawable)
{
	m_window.setView(m_gameplayView);
	m_window.draw(drawable);
}

void WindowManager::drawToHudView(sf::Drawable& drawable)
{
	m_window.setView(m_hudView);
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

sf::FloatRect WindowManager::getGameplayViewSpace()
{
	sf::Vector2f viewCenter = m_gameplayView.getCenter();
	sf::Vector2f viewSize = m_gameplayView.getSize();
	return sf::FloatRect(viewCenter - viewSize / 2.f, viewSize);
}

sf::FloatRect WindowManager::getHudViewSpace()
{
	sf::Vector2f viewCenter = m_hudView.getCenter();
	sf::Vector2f viewSize = m_hudView.getSize();
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
