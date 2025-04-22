#include "State_Intro.h"
#include "StateManager.h"
#include "Utils.h"
#include "Params.h"

State_Intro::State_Intro(StateManager* stateManager) :
	State(stateManager),
	m_titleText(m_font),
	m_instructionsText(m_font),
	m_showTextTimer(0)
{
}

void State_Intro::update(const float& deltaTime)
{
	m_showTextTimer -= deltaTime;
	if (m_showTextTimer < 0)
	{
		m_showText = !m_showText;
		m_showTextTimer = m_showTextDuration;
	}
}

void State_Intro::draw()
{
	sf::RenderWindow* window = m_stateManager->getContext()->m_windowManager->getRenderWindow();
	window->draw(m_titleText);
	if (m_showText)
		window->draw(m_instructionsText);
}

void State_Intro::onCreate()
{
	WindowManager* windowManager = m_stateManager->getContext()->m_windowManager;
	// set up intro text
	m_titleText.setFont(m_font);
	m_titleText.setString({ "SPACE INVADERS" });
	m_titleText.setCharacterSize(350);
	m_titleText.setFillColor(APP_COLOR);
	sf::FloatRect titleBounds = m_titleText.getLocalBounds();
	m_titleText.setOrigin({
		titleBounds.size.x / 2.f,
		titleBounds.position.y + titleBounds.size.y / 2.f}
	);
	m_titleText.setPosition(windowManager->getRenderWindow()->getView().getCenter());
	// set up instructions text
	m_instructionsText = m_titleText;
	m_instructionsText.setString({ "Start (SPACE)" });
	m_instructionsText.setCharacterSize(m_fontSize);
	sf::FloatRect instBounds = m_instructionsText.getLocalBounds();
	m_instructionsText.setOrigin({
		instBounds.size.x / 2.f,
		instBounds.size.y / 2.f}
	);
	m_instructionsText.setPosition({windowManager->getRenderWindow()->getView().getCenter().x, m_titleText.getPosition().y + 40});

	m_transparent = true;
	// load music
	m_stateManager->getContext()->m_soundManager->loadSoundProfile("profiles/soundProfiles/intro_state.sound");
}

void State_Intro::activate()
{
	m_stateManager->getContext()->m_controller->m_onSelect.addCallback("Intro_onContinue", std::bind(&State_Intro::onContinue, this));
	// play music
	m_stateManager->getContext()->m_soundManager->playMusic("intro_music");
	// initiate text flashing
	m_showTextTimer = m_showTextDuration;
}

void State_Intro::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onSelect.removeCallback("Intro_onContinue");
}

void State_Intro::onContinue()
{
	m_stateManager->switchTo(StateType::Game);
	m_stateManager->remove(StateType::Intro);
}
