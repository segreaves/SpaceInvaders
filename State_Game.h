#pragma once
#include "State.h"
#include "LevelManager.h"
#include "ActorManager.h"

class StateManager;
class LevelManager;

/// <summary>
/// Runs gameplay.
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
	void onPlayerMove(sf::Vector2f xy);
	void onPlayerShoot();
	void incrementBullet();
	void onInvaderDefeated();

	void updateHUD();
	void drawHUD();
	void setHUDStyle();

	int m_playerId;
	const sf::Vector2f m_playerSize = sf::Vector2f(50, 35);
	const sf::Vector2f m_bulletSize = sf::Vector2f(3, 15);
	const sf::Vector2f m_invaderSize = sf::Vector2f(50, 35);
	std::vector<ActorId> m_bullets;
	std::vector<ActorId> m_invaders;
	unsigned int m_remainingInvaders;
	unsigned int m_bulletIndex;
	LevelManager m_levelManager;
	unsigned int m_kills;
	// HUD elements
	unsigned int m_fontSize = 50;
	unsigned int m_hudPadding = 10;
	sf::Text m_scoreText;
	sf::Text m_levelText;
	sf::Text m_livesText;
	sf::Text m_killsText;
	sf::Font m_font;
};
