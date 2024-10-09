#pragma once
#include "State.h"

class StateManager;

class State_Paused : public State
{
public:
	State_Paused(StateManager* stateManager);
	~State_Paused();

	void update(const float& deltaTime);
	void draw();
	void onCreate();
	void onDestroy();
	void activate();
	void deactivate();
private:
	sf::RectangleShape m_panel;
	sf::Text m_pausedText;
};