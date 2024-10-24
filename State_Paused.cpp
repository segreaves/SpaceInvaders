#include "State_Paused.h"
#include "StateManager.h"
#include "Params.h"

State_Paused::State_Paused(StateManager* stateManager) :
	State(stateManager)
{
}

void State_Paused::update(const float& deltaTime)
{
}

void State_Paused::draw()
{
	sf::RenderWindow* window = m_stateManager->getContext()->m_windowManager->getRenderWindow();
	window->draw(m_panel);
	window->draw(m_pausedText);
}

void State_Paused::onCreate()
{
	m_transparent = true;

	// set gray panel for background
	WindowManager* windowManager = m_stateManager->getContext()->m_windowManager;
	m_panel.setSize(windowManager->getCurrentViewSpace().getSize());
	m_panel.setPosition(0, 0);
	m_panel.setFillColor(OVERLAY_COLOR);

	m_pausedText.setString({ "PAUSED" });
	m_pausedText.setFont(m_font);
	m_pausedText.setCharacterSize(75);
	m_pausedText.setFillColor(APP_COLOR);
	m_pausedText.setPosition(windowManager->getCurrentViewSpace().getSize().x / 2.0f, windowManager->getCurrentViewSpace().getSize().y / 2.0f);
	m_pausedText.setOrigin(
		m_pausedText.getLocalBounds().left + m_pausedText.getLocalBounds().width / 2.0f,
		m_pausedText.getLocalBounds().top + m_pausedText.getLocalBounds().height / 2.0f
	);
}

void State_Paused::activate()
{
	m_stateManager->getContext()->m_controller->m_onPause.addCallback("Paused_onUnpause", std::bind(&StateManager::switchTo, m_stateManager, StateType::Game));
}

void State_Paused::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onPause.removeCallback("Paused_onUnpause");
}
