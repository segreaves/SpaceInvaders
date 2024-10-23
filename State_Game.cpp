#include "State_Game.h"
#include "StateManager.h"
#include "Comp_Position.h"
#include "Comp_Movement.h"
#include "Comp_Invader.h"
#include "LevelManager.h"
#include "Utils.h"

State_Game::State_Game(StateManager* stateManager) :
	State(stateManager),
	m_levelManager(stateManager->getContext()->m_actorManager),
	m_hudUpdateTimer(0),
	m_deathTimer(0),
	m_playerDead(false),
	m_newGame(true)
{
}

void State_Game::update(const float& deltaTime)
{
	if (m_levelManager.getInvaderCount() <= 0)
		loadNextLevel();
	m_fps = 1.0f / deltaTime;
	// game update
	m_stateManager->getContext()->m_systemManager->update(deltaTime);
	// HUD update
	m_hudUpdateTimer += deltaTime;
	if (m_hudUpdateTimer >= m_hudUpdateInterval)
	{
		updateHUD();
		m_hudUpdateTimer -= m_hudUpdateInterval;
	}
	// if player dead, prepare to switch to Game Over screen
	if (m_playerDead)
	{
		m_deathTimer -= deltaTime;
		if (m_deathTimer < 0)
			gameOverScreen();
	}
}

void State_Game::draw()
{
	drawGame();
	drawHUD();
}

void State_Game::onCreate()
{
	m_gameView.setViewport(sf::FloatRect(0.15f, 0, 0.7f, 1));
	m_hudView.setViewport(sf::FloatRect(0, 0, 1, 1));

	m_stateManager->getContext()->m_systemManager->setLevelManager(&m_levelManager);
	setHUDStyle();
	setWindowOutline();
}

void State_Game::activate()
{
	m_stateManager->getContext()->m_controller->m_onPause.addCallback("Game_onPause", std::bind(&StateManager::switchTo, m_stateManager, StateType::Paused));
	m_stateManager->getContext()->m_controller->m_onMove.addCallback("Game_onMove", std::bind(&State_Game::onPlayerMove, this, std::placeholders::_1));
	m_stateManager->getContext()->m_controller->m_onShoot.addCallback("Game_onShoot", std::bind(&State_Game::onPlayerShoot, this));
	m_stateManager->getContext()->m_actorManager->m_actorDisabled.addCallback("Game_onActorDisabled", std::bind(&State_Game::onActorDisabled, this, std::placeholders::_1));
	if (m_newGame)
		newGame();
}

void State_Game::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onPause.removeCallback("Game_onPause");
	m_stateManager->getContext()->m_controller->m_onMove.removeCallback("Game_onMove");
	m_stateManager->getContext()->m_controller->m_onShoot.removeCallback("Game_onShoot");
	m_stateManager->getContext()->m_actorManager->m_actorDisabled.removeCallback("Game_onActorDisabled");
}

void State_Game::loadNextLevel()
{
	// increase level
	m_levelManager++;
	// reset invader count
	m_levelManager.resetInvaderCount();
	// enable (or re-enable) all actors
	for (auto& invaderId : m_levelManager.getInvaderIds())
		m_stateManager->getContext()->m_actorManager->enableActor(invaderId);
	// start (or re-start) all systems
	m_stateManager->getContext()->m_systemManager->start();
}

void State_Game::onPlayerMove(sf::Vector2f xy)
{
	unsigned int playerId = m_levelManager.getPlayerId();
	auto actor = m_stateManager->getContext()->m_actorManager->getActor(playerId);
	auto targetComp = actor->getComponent<Comp_Target>(ComponentType::Target);
	targetComp->setTarget(targetComp->getTarget() + xy);
}

void State_Game::onPlayerShoot()
{
	m_stateManager->getContext()->m_systemManager->addEvent(m_levelManager.getPlayerId(), (EventId)ActorEventType::Shoot);
}

void State_Game::onActorDisabled(unsigned int actorId)
{
	auto actor = m_stateManager->getContext()->m_actorManager->getActor(actorId);
	if (actor->getTag() == "invader")
		m_levelManager.onInvaderDefeated();
	else if (actor->getTag() == "player")
	{
		// enable player explosion particle system
		ActorId explosionId = m_levelManager.getPlayerExplosionId();
		auto explosionPos = m_stateManager->getContext()->m_actorManager->getActor(explosionId)->getComponent<Comp_Position>(ComponentType::Position);
		auto playerPos = m_stateManager->getContext()->m_actorManager->getActor(m_levelManager.getPlayerId())->getComponent<Comp_Position>(ComponentType::Position);
		explosionPos->setPosition(playerPos->getPosition());
		auto particlesComp = m_stateManager->getContext()->m_actorManager->getActor(explosionId)->getComponent<Comp_Particles>(ComponentType::Particles);
		particlesComp->getParticleSystem()->initialize();
		particlesComp->getParticleSystem()->setEmitterPosition(explosionPos->getPosition());
		particlesComp->getParticleSystem()->emitParticles();
		m_stateManager->getContext()->m_actorManager->enableActor(explosionId);
		m_playerDead = true;
		m_deathTimer = m_gameOverWaitTime;
	}
}

void State_Game::gameOverScreen()
{
	m_newGame = true;
	m_stateManager->remove(StateType::Paused);
	m_stateManager->switchTo(StateType::GameOver);
	m_levelManager.purge();
}

void State_Game::newGame()
{
	m_newGame = false;
	m_playerDead = false;
	m_stateManager->getContext()->m_actorManager->purge();
	m_levelManager.newGame(getGameViewSpace());
	m_stateManager->getContext()->m_actorManager->enableActor(m_levelManager.getPlayerId());
}

void State_Game::updateHUD()
{
	m_scoreText.setString("Score: " + std::to_string(0));
	m_levelText.setString("Level: " + std::to_string(m_levelManager.getLevel()));
	m_livesText.setString("Lives: " + std::to_string(m_levelManager.getPlayerLives()));
	m_killsText.setString("Kills: " + std::to_string(m_levelManager.getKills()));
	m_fpsText.setString("FPS: " + std::to_string(m_fps));
}

void State_Game::drawGame()
{
	m_stateManager->getContext()->m_windowManager->getRenderWindow()->setView(m_gameView);
	m_stateManager->getContext()->m_windowManager->getRenderWindow()->draw(m_background);
	m_stateManager->getContext()->m_systemManager->draw(m_stateManager->getContext()->m_windowManager);
}

void State_Game::drawHUD()
{
	WindowManager* windowManager = m_stateManager->getContext()->m_windowManager;
	windowManager->getRenderWindow()->setView(m_hudView);
	windowManager->getRenderWindow()->draw(m_scoreText);
	windowManager->getRenderWindow()->draw(m_levelText);
	windowManager->getRenderWindow()->draw(m_livesText);
	windowManager->getRenderWindow()->draw(m_killsText);
	windowManager->getRenderWindow()->draw(m_fpsText);
}

void State_Game::setHUDStyle()
{
	m_scoreText.setFont(m_font);
	m_scoreText.setCharacterSize(m_fontSize);
	m_scoreText.setPosition(m_hudPadding, m_hudPadding);
	m_scoreText.setFillColor(APP_COLOR);
	m_levelText.setFont(m_font);
	m_levelText.setCharacterSize(m_fontSize);
	m_levelText.setPosition(m_hudPadding, m_hudPadding + m_fontSize);
	m_levelText.setFillColor(APP_COLOR);
	m_livesText.setFont(m_font);
	m_livesText.setCharacterSize(m_fontSize);
	m_livesText.setPosition(m_hudPadding, m_hudPadding + 2 * m_fontSize);
	m_livesText.setFillColor(APP_COLOR);
	m_killsText.setFont(m_font);
	m_killsText.setCharacterSize(m_fontSize);
	m_killsText.setPosition(m_hudPadding, m_hudPadding + 3 * m_fontSize);
	m_killsText.setFillColor(APP_COLOR);
	m_fpsText.setFont(m_font);
	m_fpsText.setCharacterSize(m_fontSize);
	m_fpsText.setPosition(m_hudPadding, m_hudPadding + 4 * m_fontSize);
	m_fpsText.setFillColor(APP_COLOR);
}

void State_Game::setWindowOutline()
{
	float outlineThickness = 2;
	m_background.setSize(m_gameView.getSize() - sf::Vector2f(2 * outlineThickness, 2 * outlineThickness));
	m_background.setPosition(outlineThickness, outlineThickness);
	m_background.setFillColor(sf::Color::Transparent);
	m_background.setOutlineColor(APP_COLOR);
	m_background.setOutlineThickness(outlineThickness);
}

sf::FloatRect State_Game::getGameViewSpace()
{
	sf::Vector2f viewCenter = m_gameView.getCenter();
	sf::Vector2f viewSize = m_gameView.getSize();
	return sf::FloatRect(viewCenter - viewSize / 2.f, viewSize);
}

sf::FloatRect State_Game::getHUDViewSpace()
{
	sf::Vector2f viewCenter = m_hudView.getCenter();
	sf::Vector2f viewSize = m_hudView.getSize();
	return sf::FloatRect(viewCenter - viewSize / 2.f, viewSize);
}
