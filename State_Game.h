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
	void createPlayer();
	void createBullets(unsigned int maxBullets);
	void createInvaders();
	void createBunkers();
	void onPlayerMove(sf::Vector2f xy);
	void onPlayerShoot();
	void incrementBullet();
	void onInvaderDefeated();

	void updateHUD();
	void drawGame();
	void drawHUD();
	void setHUDStyle();
	void setWindowOutline();
	sf::FloatRect getGameViewSpace();
	sf::FloatRect getHUDViewSpace();

	sf::View m_gameView;
	sf::View m_hudView;
	const sf::Vector2f m_bulletSize = sf::Vector2f(3, 3);
	int m_playerId;
	std::vector<ActorId> m_bullets;
	std::vector<ActorId> m_bunkers;
	unsigned int m_remainingInvaders;
	unsigned int m_bulletIndex;
	LevelManager m_levelManager;
	unsigned int m_fps;
	unsigned int m_kills;
	sf::RectangleShape m_background;
	// HUD elements
	unsigned int m_fontSize = 50;
	unsigned int m_hudPadding = 10;
	sf::Text m_scoreText;
	sf::Text m_levelText;
	sf::Text m_livesText;
	sf::Text m_killsText;
	sf::Text m_fpsText;
	sf::Font m_font;
};
