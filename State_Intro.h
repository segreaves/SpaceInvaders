#pragma once
#include "State.h"
#include "InputManager.h"

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

	void transition(EventDetails* details);
private:
	sf::Font m_font;
	sf::Text m_text;
	sf::RectangleShape m_panel;
	std::string m_continueInput = "Intro_Continue";
};
