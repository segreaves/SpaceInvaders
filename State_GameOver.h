#pragma once
#include "State.h"

class State_GameOver : public State
{
	public:
	State_GameOver(StateManager* stateManager);
	~State_GameOver();

	void update(const float& deltaTime);
	void draw();
	void onCreate();
	void onDestroy();
	void activate();
	void deactivate();
};