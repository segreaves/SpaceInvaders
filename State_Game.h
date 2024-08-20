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

	void loadNextLevel();
	void createPlayer();
	void createBullets(unsigned int maxBullets);
	void createInvaders();
	void onPlayerMove(sf::Vector2f xy);
	void onPlayerShoot();
private:
	int m_playerId;
	Level* m_level;
};
