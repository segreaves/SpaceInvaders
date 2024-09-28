#include "State_Intro.h"
#include "StateManager.h"
#include "Utils.h"
#include "Params.h"

State_Intro::State_Intro(StateManager* stateManager) :
	State(stateManager)
{
	m_view.setViewport(sf::FloatRect(0, 0, 1, 1));
}

State_Intro::~State_Intro()
{
	onDestroy();
}

void State_Intro::update(const float& deltaTime) {}

void State_Intro::draw()
{
	sf::RenderWindow* window = m_stateManager->getContext()->m_windowManager->getRenderWindow();
	window->draw(m_panel);
	window->draw(m_text);
}

void State_Intro::onCreate()
{
	m_font.loadFromFile(Utils::getWorkingDirectory() + "assets/fonts/game_over.ttf");
	m_text.setFont(m_font);
	m_text.setString({ "SPACE INVADERS" });
	m_text.setCharacterSize(150);
	m_text.setFillColor(APP_COLOR);
	sf::FloatRect textBounds = m_text.getLocalBounds();
	m_text.setOrigin(
		textBounds.left + textBounds.width / 2.0f,
		textBounds.top + textBounds.height / 2.0f
	);

	//sf::Vector2u windowSize = m_stateManager->getContext()->m_windowManager->getRenderWindow()->getSize();
	WindowManager* windowManager = m_stateManager->getContext()->m_windowManager;
	windowManager->getRenderWindow()->setView(m_view);
	sf::Vector2f windowSize = windowManager->getCurrentViewSpace().getSize();
	m_text.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f);

	m_transparent = true;
	m_panel.setSize(sf::Vector2f(windowSize));
	m_panel.setPosition(0, 0);
	m_panel.setFillColor(sf::Color(0, 0, 0, 255));
}

void State_Intro::onDestroy()
{
}

void State_Intro::activate()
{
	m_stateManager->getContext()->m_controller->m_onShoot.addCallback("Intro_onContinue", std::bind(&State_Intro::onContinue, this));
	m_stateManager->getContext()->m_windowManager->getRenderWindow()->setView(m_view);
}

void State_Intro::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onShoot.removeCallback("Intro_onContinue");
}

void State_Intro::onContinue()
{
	m_stateManager->switchTo(StateType::Game);
	m_stateManager->remove(StateType::Intro);
}
