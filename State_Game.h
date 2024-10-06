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

	void update(const float& deltaTime) override;
	void draw() override;
	void onCreate() override;
	void onDestroy() override;
	void activate() override;
	void deactivate() override;
private:
	void loadNextLevel();
	void onPlayerMove(sf::Vector2f xy);
	void onPlayerShoot();
	void onActorDisabled(unsigned int actorId);
	void gameOverScreen();

	void updateHUD();
	void drawGame();
	void drawHUD();
	void setHUDStyle();
	void setWindowOutline();
	sf::FloatRect getGameViewSpace();
	sf::FloatRect getHUDViewSpace();

	LevelManager m_levelManager;
	float m_currentInvaderSpeed;
	sf::View m_gameView;
	sf::View m_hudView;
	std::vector<ActorId> m_bunkers;
	unsigned int m_playerLives;
	unsigned int m_playerBulletIndex;
	unsigned int m_invaderBulletIndex;
	unsigned int m_shockwaveIndex;
	unsigned int m_fps;
	unsigned int m_lives;
	sf::RectangleShape m_background;
	// HUD elements
	const float m_hudUpdateInterval = 0.2;
	float m_hudUpdateTimer;
	unsigned int m_fontSize = 50;
	unsigned int m_hudPadding = 10;
	sf::Text m_scoreText;
	sf::Text m_levelText;
	sf::Text m_livesText;
	sf::Text m_killsText;
	sf::Text m_fpsText;
};
