#pragma once
#include <SFML/Graphics.hpp>
#include "Controller.h"

class WindowManager
{
public:
	WindowManager(const std::string& title, const sf::Vector2u& size);
	~WindowManager();

	void update(float deltaTime);
	void clear();
	void display();

	bool isOpen();
	bool isFocused();

	sf::RenderWindow* getRenderWindow();
	sf::Vector2u getWindowSize();
	sf::FloatRect getCurrentViewSpace();
	Controller* getController();

	void toggleFullscreen();
private:
	void createWindow();
	void closeWindow();

	sf::RenderWindow m_window;
	sf::Vector2u m_windowSize;
	std::string m_windowTitle;
	bool m_isFocused;
	bool m_isFullscreen;
	bool m_isOpen;
	Controller m_controller;
	const unsigned int m_framerate = 60;
};
