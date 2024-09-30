#include "State_Paused.h"
#include "StateManager.h"
#include "Params.h"

State_Paused::State_Paused(StateManager* stateManager) :
	State(stateManager)
{
	m_view.setViewport(sf::FloatRect(0, 0, 1, 1));
}

State_Paused::~State_Paused()
{
	onDestroy();
}

void State_Paused::update(const float& deltaTime)
{
}

void State_Paused::draw()
{
	sf::RenderWindow* window = m_stateManager->getContext()->m_windowManager->getRenderWindow();
	window->setView(m_view);
	window->draw(m_panel);
	for (auto& text : m_screenTexts)
		window->draw(text);
}

void State_Paused::onCreate()
{
	m_transparent = true;
	WindowManager* windowManager = m_stateManager->getContext()->m_windowManager;
	windowManager->getRenderWindow()->setView(m_view);
	m_panel.setSize(windowManager->getCurrentViewSpace().getSize());
	m_panel.setPosition(0, 0);
	m_panel.setFillColor(sf::Color(0, 0, 0, 150));

	sf::Text text;
	text.setString({ "PAUSED" });
	text.setFont(m_font);
	text.setCharacterSize(75);
	text.setFillColor(APP_COLOR);
	text.setPosition(sf::Vector2f(m_view.getSize().x / 2.0f, m_view.getSize().y / 2.0f));
	sf::FloatRect textBounds = text.getLocalBounds();
	text.setOrigin(
		textBounds.left + textBounds.width / 2.0f,
		textBounds.top + textBounds.height / 2.0f
	);
	m_screenTexts.emplace_back(text);
}

void State_Paused::onDestroy()
{
}

void State_Paused::activate()
{
	m_stateManager->getContext()->m_windowManager->getRenderWindow()->setView(m_view);
	m_stateManager->getContext()->m_controller->m_onPause.addCallback("Paused_onUnpause", std::bind(&StateManager::switchTo, m_stateManager, StateType::Game));
}

void State_Paused::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onPause.removeCallback("Paused_onUnpause");
}
