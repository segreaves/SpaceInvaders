#pragma once
#include <SFML/Graphics.hpp>
#include "Utils.h"

class StateManager;

class State
{
	friend class StateManager;
public:
	State(StateManager* stateManager) :
		m_stateManager(stateManager),
		m_transparent(false)
	{
		m_font.loadFromFile(Utils::getWorkingDirectory() + "assets/fonts/game_over.ttf");
	}

	virtual void update(const float& deltaTime) = 0;
	virtual void draw() = 0;

	virtual void onCreate() = 0;
	virtual void onDestroy() = 0;
	virtual void activate() = 0;
	virtual void deactivate() = 0;

	StateManager* getStateManager() { return m_stateManager; }
	void setTransparent(bool transparent) { m_transparent = transparent; }
	bool isTransparent() { return m_transparent; }
protected:
	StateManager* m_stateManager;
	bool m_transparent;
	sf::Font m_font;
};
