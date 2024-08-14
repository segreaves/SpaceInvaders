#pragma once
#include "State.h"

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

	void onPlayerMove(sf::Vector2f xy);
private:
	int m_playerId;
};
