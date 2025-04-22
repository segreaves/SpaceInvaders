#include "State_GameOver.h"
#include "StateManager.h"
#include "Params.h"

State_GameOver::State_GameOver(StateManager* stateManager) :
	State(stateManager),
	m_gameOverText(m_font),
	m_optionsText(m_font)
{
}

void State_GameOver::update(const float& deltaTime)
{
	m_showTextTimer -= deltaTime;
	if (m_showTextTimer < 0)
	{
		m_showText = !m_showText;
		m_showTextTimer = m_showTextDuration;
	}
}

void State_GameOver::draw()
{
	sf::RenderWindow* window = m_stateManager->getContext()->m_windowManager->getRenderWindow();
	window->draw(m_panel);
	window->draw(m_gameOverText);
	if (m_showText)
		window->draw(m_optionsText);
}

void State_GameOver::onCreate()
{
	m_transparent = true;
	// set gray panel for background
	WindowManager* windowManager = m_stateManager->getContext()->m_windowManager;
	m_panel.setSize(windowManager->getCurrentViewSpace().size);
	m_panel.setPosition({0, 0});
	m_panel.setFillColor(OVERLAY_COLOR);

	// game over text
	m_gameOverText.setString({ "GAME OVER" });
	m_gameOverText.setFont(m_font);
	m_gameOverText.setCharacterSize(350);
	m_gameOverText.setFillColor(APP_COLOR);
	m_gameOverText.setPosition({windowManager->getRenderWindow()->getView().getCenter().x, windowManager->getRenderWindow()->getView().getCenter().y});
	m_gameOverText.setOrigin({
		m_gameOverText.getLocalBounds().position.x + m_gameOverText.getLocalBounds().size.x / 2.0f,
		m_gameOverText.getLocalBounds().position.y + m_gameOverText.getLocalBounds().size.y / 2.0f}
		);
	// player options text
	m_optionsText.setString({ "Restart (SPACE) - Exit (ESC)" });
	m_optionsText.setFont(m_font);
	m_optionsText.setCharacterSize(m_fontSize);
	m_optionsText.setFillColor(APP_COLOR);
	m_optionsText.setOrigin({
		m_optionsText.getLocalBounds().size.x / 2.0f,
		m_optionsText.getLocalBounds().size.y / 2.0f}
	);
	m_optionsText.setPosition({windowManager->getRenderWindow()->getView().getCenter().x, windowManager->getRenderWindow()->getView().getCenter().y + 40});
	// load game over sound
	m_stateManager->getContext()->m_soundManager->loadSoundProfile("profiles/soundProfiles/game_over_state.sound");
}

void State_GameOver::activate()
{
	m_stateManager->getContext()->m_controller->m_onSelect.addCallback("GameOver_onSelect", std::bind(&State_GameOver::OnSelect, this));
	// play  game over sound
	m_stateManager->getContext()->m_soundManager->playSound("game_over_sound");
	// initiate text flashing
	m_showTextTimer = m_showTextDuration;
}

void State_GameOver::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onSelect.removeCallback("GameOver_onSelect");
}

void State_GameOver::OnSelect()
{
	m_stateManager->switchTo(StateType::Game);
}
