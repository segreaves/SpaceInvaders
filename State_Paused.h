#pragma once
#include "State.h"

class StateManager;

/// <summary>
/// Pauses the game and darkens the screen.
/// </summary>
class State_Paused : public State
{
public:
	State_Paused(StateManager* stateManager);

private:
	void update(const float& deltaTime) override;
	void draw() override;
	void onCreate() override;
	void activate() override;
	void deactivate() override;

	sf::RectangleShape m_panel;
	sf::Text m_pausedText;
};