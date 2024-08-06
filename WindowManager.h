#pragma once
#include <SFML/Graphics.hpp>
#include "InputManager.h"

class WindowManager
{
public:
	WindowManager(const std::string& title, const sf::Vector2u& size);
	~WindowManager();

	void update();
	void drawStart();
	void draw(sf::Drawable& drawable);
	void drawEnd();

	bool isOpen();
	bool isFocused();

	sf::RenderWindow* getRenderWindow();
	sf::Vector2u getWindowSize();
	InputManager* getInputManager();

	void toggleFullscreen(EventDetails* details = nullptr);
private:
	void createWindow();
	void closeWindow(EventDetails* details = nullptr);

	sf::RenderWindow m_window;
	sf::Vector2u m_windowSize;
	std::string m_windowTitle;
	bool m_isFocused;
	bool m_isFullscreen;
	bool m_isOpen;
	InputManager m_inputManager;
};
