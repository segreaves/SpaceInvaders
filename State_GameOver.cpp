#include "State_GameOver.h"
#include "StateManager.h"
#include "Params.h"

State_GameOver::State_GameOver(StateManager* stateManager) :
	State(stateManager)
{
	m_view.setViewport(sf::FloatRect(0, 0, 1, 1));
}

State_GameOver::~State_GameOver()
{
	onDestroy();
}

void State_GameOver::update(const float& deltaTime)
{
}

void State_GameOver::draw()
{
	sf::RenderWindow* window = m_stateManager->getContext()->m_windowManager->getRenderWindow();
	window->setView(m_view);
	window->draw(m_panel);
	window->draw(m_gameOverText);
	window->draw(m_optionsText);
}

void State_GameOver::onCreate()
{
	m_transparent = true;
	// set gray panel for background
	WindowManager* windowManager = m_stateManager->getContext()->m_windowManager;
	windowManager->getRenderWindow()->setView(m_view);
	m_panel.setSize(windowManager->getCurrentViewSpace().getSize());
	m_panel.setPosition(0, 0);
	m_panel.setFillColor(OVERLAY_COLOR);

	// game over text
	m_gameOverText.setString({ "GAME OVER" });
	m_gameOverText.setFont(m_font);
	m_gameOverText.setCharacterSize(150);
	m_gameOverText.setFillColor(APP_COLOR);
	m_gameOverText.setPosition(m_view.getSize().x / 2.0f, m_view.getSize().y / 2.0f);
	m_gameOverText.setOrigin(
		m_gameOverText.getLocalBounds().left + m_gameOverText.getLocalBounds().width / 2.0f,
		m_gameOverText.getLocalBounds().top + m_gameOverText.getLocalBounds().height / 2.0f
	);
	// options to tell player what to do
	m_optionsText.setString({ "Press ENTER to restart or ESCAPE to exit" });
	m_optionsText.setFont(m_font);
	m_optionsText.setCharacterSize(75);
	m_optionsText.setFillColor(APP_COLOR);
	m_optionsText.setPosition(m_view.getSize().x / 2.0f, m_view.getSize().y / 2.0f + 50);
	m_optionsText.setOrigin(
		m_optionsText.getLocalBounds().left + m_optionsText.getLocalBounds().width / 2.0f,
		m_optionsText.getLocalBounds().top + m_optionsText.getLocalBounds().height / 2.0f
	);
}

void State_GameOver::onDestroy()
{
}

void State_GameOver::activate()
{
	m_stateManager->getContext()->m_controller->m_onEnter.addCallback("Paused_onEnter", std::bind(&State_GameOver::OnEnter, this));
}

void State_GameOver::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onEnter.removeCallback("Paused_onEnter");
}

void State_GameOver::OnEnter()
{
	m_stateManager->switchTo(StateType::Game);
}
