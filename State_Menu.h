#pragma once
#include "State.h"

class State_Menu : public State
{
public:
	State_Menu(StateManager* stateManager);

	void onContinue();
private:
	void update(const float& deltaTime) override;
	void draw() override;
	void onCreate() override;
	void activate() override;
	void deactivate() override;

	sf::Font m_font;
	sf::Text m_text;
	sf::RectangleShape m_panel;
};