#include "State_Intro.h"
#include "StateManager.h"
#include "Utilities.h"

State_Intro::State_Intro(StateManager* stateManager) : State(stateManager)
{
}

State_Intro::~State_Intro()
{
	onDestroy();
}

void State_Intro::update(const float& deltaTime)
{
}

void State_Intro::draw()
{
	sf::RenderWindow* window = m_stateManager->getContext()->m_windowManager->getRenderWindow();
	window->draw(m_panel);
	window->draw(m_text);
}

void State_Intro::onCreate()
{
	m_font.loadFromFile(Utilities::getWorkingDirectory() + "assets/fonts/game_over.ttf");
	m_text.setFont(m_font);
	m_text.setString({ "SPACE INVADERS" });
	m_text.setCharacterSize(150);
	sf::FloatRect textBounds = m_text.getLocalBounds();
	m_text.setOrigin(
		textBounds.left + textBounds.width / 2.0f,
		textBounds.top + textBounds.height / 2.0f
	);

	sf::Vector2u windowSize = m_stateManager->getContext()->m_windowManager->getRenderWindow()->getSize();
	m_text.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

	m_transparent = true;
	m_panel.setSize(sf::Vector2f(windowSize));
	m_panel.setPosition(0, 0);
	m_panel.setFillColor(sf::Color(0, 0, 0, 150));

	m_stateManager->getContext()->m_inputManager->addCallback(StateType::Intro, m_continueInput, &State_Intro::transition, this);
}

void State_Intro::onDestroy()
{
	m_stateManager->getContext()->m_inputManager->removeCallback(StateType::Intro, m_continueInput);
}

void State_Intro::activate()
{
}

void State_Intro::deactivate()
{
}

void State_Intro::transition(EventDetails* details)
{
	m_stateManager->switchTo(StateType::Menu);
	//m_stateManager->remove(StateType::Intro);
}