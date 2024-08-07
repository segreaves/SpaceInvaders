#include "State_Menu.h"
#include "StateManager.h"
#include "Utilities.h"

State_Menu::State_Menu(StateManager* stateManager) :
	State(stateManager)
{
}

State_Menu::~State_Menu()
{
	m_stateManager->getContext()->m_inputManager->removeCallback(StateType::Menu, m_continueInput);
}

void State_Menu::update(const float& deltaTime)
{
}

void State_Menu::draw()
{
	sf::RenderWindow* window = m_stateManager->getContext()->m_windowManager->getRenderWindow();
	window->draw(m_panel);
	window->draw(m_text);
}

void State_Menu::onCreate()
{
	m_font.loadFromFile(Utilities::getWorkingDirectory() + "assets/fonts/game_over.ttf");
	m_text.setFont(m_font);
	m_text.setString({ "Menu" });
	m_text.setCharacterSize(150);
	sf::FloatRect textBounds = m_text.getLocalBounds();
	m_text.setOrigin(
		textBounds.left + textBounds.width / 2.0f,
		textBounds.top + textBounds.height / 2.0f
	);

	sf::Vector2u windowSize = m_stateManager->getContext()->m_windowManager->getRenderWindow()->getSize();
	m_text.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

	m_transparent = false;
	m_panel.setSize(sf::Vector2f(windowSize));
	m_panel.setPosition(0, 0);
	m_panel.setFillColor(sf::Color(0, 0, 0, 150));

	m_stateManager->getContext()->m_inputManager->addCallback(StateType::Menu, m_continueInput, &State_Menu::transition, this);
}

void State_Menu::onDestroy()
{
}

void State_Menu::activate()
{
}

void State_Menu::deactivate()
{
}

void State_Menu::transition(EventDetails* details)
{
	m_stateManager->switchTo(StateType::Game);
	//m_stateManager->remove(StateType::Menu);
}
