#pragma once
#include "State.h"
#include "LevelManager.h"
#include "ActorManager.h"

class StateManager;
class LevelManager;

/// <summary>
/// Runs gameplay.
/// Receives player input and transmits to the handler systems.
/// Counts number of invaders and resets when it reaches zero.
/// Communicates with the LevelManager to distribute invaders in a grid and
/// query their spawn points when resetting positions.
/// </summary>
class State_Game : public State
{
public:
	State_Game(StateManager* stateManager);
private:
	void update(const float& deltaTime) override;
	void draw() override;
	void onCreate() override;
	void activate() override;
	void deactivate() override;

	void loadNextLevel();
	void onPlayerMove(sf::Vector2f xy);
	void onPlayerShoot();
	void gameOverScreen();
	void newGame();

	void updateHUD();
	void drawGame();
	void drawHUD();
	void setHUDStyle();
	void setWindowOutline();
	sf::FloatRect getGameViewSpace();
	sf::FloatRect getHUDViewSpace();

	LevelManager m_levelManager;
	sf::View m_gameView;
	sf::View m_hudView;
	bool m_newGame;
	unsigned int m_fps;
	sf::RectangleShape m_background;
	// HUD elements
	float m_hudUpdateTimer;
	const float m_hudUpdateInterval = 0.2f;
	int m_fontSize = 50;
	float m_hudPadding = 10.f;
	sf::Text m_scoreText;
	sf::Text m_levelText;
	sf::Text m_livesText;
	sf::Text m_killsText;
	sf::Text m_fpsText;
};
