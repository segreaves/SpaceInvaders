#pragma once
#include "State.h"

class State_Intro : public State
{
public:
	State_Intro(StateManager* stateManager);
	~State_Intro();

	void update(const float& deltaTime);
	void draw();
	void onCreate();
	void onDestroy();
	void activate();
	void deactivate();

	void onContinue();
private:
	sf::View m_view;
	sf::Font m_font;
	sf::Text m_text;
	std::string m_continueInput = "Intro_Continue";
};
