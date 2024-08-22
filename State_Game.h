#pragma once
#include "State.h"

class Level;

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

	int m_playerId;
	const sf::Vector2f m_playerSize = sf::Vector2f(50, 35);
	const sf::Vector2f m_bulletSize = sf::Vector2f(3, 15);
	const sf::Vector2f m_invaderSize = sf::Vector2f(50, 35);
	Level* m_level;
};
