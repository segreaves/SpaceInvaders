#pragma once
#include "State.h"

class State_Intro : public State
{
public:
	State_Intro(StateManager* stateManager);

	void onContinue();
private:
	void update(const float& deltaTime) override;
	void draw() override;
	void onCreate() override;
	void activate() override;
	void deactivate() override;

	sf::View m_view;
	sf::Text m_titleText, m_instructionsText;
	std::string m_continueInput = "Intro_Continue";
};
