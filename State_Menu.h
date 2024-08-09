#pragma once
#include "State.h"

class State_Menu : public State
{
public:
	State_Menu(StateManager* stateManager);
	~State_Menu();

	void update(const float& deltaTime);
	void draw();
	void onCreate();
	void onDestroy();
	void activate();
	void deactivate();

	void onContinue();
private:
	sf::Font m_font;
	sf::Text m_text;
	sf::RectangleShape m_panel;
};