#pragma once
#include <SFML/Graphics.hpp>
#include "Utils.h"

class StateManager;

/// <summary>
/// Base class for all states.
/// </summary>
class State
{
	friend class StateManager;
public:
	State(StateManager* stateManager) :
		m_stateManager(stateManager),
		m_transparent(false)
	{
		bool isLoaded = m_font.openFromFile(Utils::getAssetsDirectory() + "fonts/game_over.ttf");

		if (!isLoaded) {
			// Handle the error, e.g., log an error message or throw an exception
			std::cerr << "Failed to load font from file." << std::endl;
		}
	}

	virtual ~State() = default;

	StateManager* getStateManager() { return m_stateManager; }
	void setTransparent(bool transparent) { m_transparent = transparent; }
	bool isTransparent() { return m_transparent; }
protected:
	virtual void update(const float& deltaTime) = 0;
	virtual void draw() = 0;

	virtual void onCreate() = 0;
	virtual void activate() = 0;
	virtual void deactivate() = 0;

	StateManager* m_stateManager;
	sf::View m_fullWindowView;
	bool m_transparent;
	sf::Font m_font;
	unsigned int m_fontSize = 100;
};
