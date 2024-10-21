#pragma once
#include "State.h"

class State_GameOver : public State
{
public:
	State_GameOver(StateManager* stateManager);

private:
	void update(const float& deltaTime) override;
	void draw() override;
	void onCreate() override;
	void activate() override;
	void deactivate() override;

	void OnEnter();
	sf::RectangleShape m_panel;
	sf::Text m_gameOverText;
	sf::Text m_optionsText;
};