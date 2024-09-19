#include "State_Game.h"
#include "StateManager.h"
#include "Comp_Position.h"
#include "Comp_Movement.h"
#include "Comp_Sprite.h"
#include "Comp_Invader.h"
#include "LevelManager.h"
#include "Utils.h"

State_Game::State_Game(StateManager* stateManager) :
	State(stateManager),
	m_playerBulletIndex(0),
	m_remainingInvaders(0),
	m_kills(0)
{
	m_gameView.setViewport(sf::FloatRect(0.15f, 0, 0.7f, 1));
	m_hudView.setViewport(sf::FloatRect(0, 0, 1, 1));

	m_levelManager.setActorManager(stateManager->getContext()->m_actorManager);
	setHUDStyle();
	setWindowOutline();
}

void State_Game::update(const float& deltaTime)
{
	m_fps = 1.0f / deltaTime;
	m_stateManager->getContext()->m_systemManager->update(deltaTime);
	updateHUD();
}

void State_Game::draw()
{
	drawGame();
	drawHUD();
}

void State_Game::onCreate()
{
	m_levelManager.setViewSpace(getGameViewSpace());
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->setLevelManager(&m_levelManager);
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_PlayerControl>(SystemType::PlayerControl)->setLevelManager(&m_levelManager);
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_BulletControl>(SystemType::BulletControl)->setLevelManager(&m_levelManager);
	m_levelManager.createPlayer();
	m_levelManager.createPlayerBullets();
	m_levelManager.createInvaderBullets();
	m_levelManager.createShockwaves();
	m_levelManager.createInvaders(getGameViewSpace());
	m_levelManager.createBunkers(getGameViewSpace());
	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	actorManager->enableActor(m_levelManager.getPlayerId());
	for (auto& bunkerId : m_levelManager.getBunkerIds())
	{
		actorManager->enableActor(bunkerId);
		Comp_Position* posComp = actorManager->getActor(bunkerId)->getComponent<Comp_Position>(ComponentType::Position);
		posComp->setPosition(m_levelManager.getBunkerSpawn(bunkerId));
	}
	loadNextLevel();
}

void State_Game::onDestroy()
{
}

void State_Game::activate()
{
	m_stateManager->getContext()->m_controller->m_onMove.addCallback("Game_onMove", std::bind(&State_Game::onPlayerMove, this, std::placeholders::_1));
	m_stateManager->getContext()->m_controller->m_onShoot.addCallback("Game_onShoot", std::bind(&State_Game::onPlayerShoot, this));
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->m_invaderDefeated.addCallback("Game_onInvaderDefeated", std::bind(&State_Game::onInvaderDefeated, this, std::placeholders::_1));
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->m_invaderShot.addCallback("Game_onInvaderShoot", std::bind(&State_Game::onInvaderShoot, this, std::placeholders::_1));
}

void State_Game::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onMove.removeCallback("Game_onMove");
	m_stateManager->getContext()->m_controller->m_onMove.removeCallback("Game_onShoot");
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->m_invaderDefeated.removeCallback("Game_onInvaderDefeated");
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->m_invaderShot.removeCallback("Game_onInvaderShoot");
}

void State_Game::loadNextLevel()
{
	m_levelManager.m_level++;
	m_remainingInvaders = m_levelManager.getInvaderIds().size();
	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	for (auto& invaderId : m_levelManager.getInvaderIds())
	{
		Actor* invader = actorManager->getActor(invaderId);
		Comp_Control* controlComp = invader->getComponent<Comp_Control>(ComponentType::Control);
		Comp_Position* posComp = invader->getComponent<Comp_Position>(ComponentType::Position);
		Comp_Movement* moveComp = invader->getComponent<Comp_Movement>(ComponentType::Movement);
		Comp_Invader* invaderComp = invader->getComponent<Comp_Invader>(ComponentType::Invader);
		Comp_SpriteSheet* spriteComp = invader->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		spriteComp->resetFrameStep();
		spriteComp->setFrameTime(0);
		moveComp->setVelocity(sf::Vector2f(0, 0));
		float newSpeed = invaderComp->getBaseSpeed() + (m_levelManager.m_level - 1) * m_levelManager.getLevelSpeedIncrease();
		controlComp->setMaxSpeed(newSpeed);
		posComp->setPosition(m_levelManager.getInvaderSpawn(invaderId));
		invaderComp->setTarget(posComp->getPosition());
		actorManager->enableActor(invaderId);
	}
	updateInvaderFPS();
	m_stateManager->getContext()->m_systemManager->start();
}

void State_Game::onPlayerMove(sf::Vector2f xy)
{
	Comp_Control* controlComp = m_stateManager->getContext()->m_actorManager->getActor(m_levelManager.getPlayerId())->getComponent<Comp_Control>(ComponentType::Control);
	controlComp->setMovementInput(xy);
}

void State_Game::onPlayerShoot()
{
	const int bulletId = m_levelManager.getPlayerBulletIds()[m_playerBulletIndex];
	m_stateManager->getContext()->m_actorManager->enableActor(bulletId);
	Comp_Bullet* bulletComp = m_stateManager->getContext()->m_actorManager->getActor(bulletId)->getComponent<Comp_Bullet>(ComponentType::Bullet);
	Message msg((MessageType)ActorMessageType::Shoot);
	msg.m_sender = m_levelManager.getPlayerId();
	msg.m_receiver = bulletId;
	msg.m_xy.x = 0;
	msg.m_xy.y = -1;
	m_stateManager->getContext()->m_systemManager->getMessageHandler()->dispatch(msg);
	incrementPlayerBullet();
}

void State_Game::onInvaderShoot(int invaderId)
{
	const int bulletId = m_levelManager.getInvaderBulletIds()[m_invaderBulletIndex];
	m_stateManager->getContext()->m_actorManager->enableActor(bulletId);
	Comp_Bullet* bulletComp = m_stateManager->getContext()->m_actorManager->getActor(bulletId)->getComponent<Comp_Bullet>(ComponentType::Bullet);
	Message msg((MessageType)ActorMessageType::Shoot);
	msg.m_sender = invaderId;
	msg.m_receiver = bulletId;
	msg.m_xy.x = 0;
	msg.m_xy.y = 1;
	m_stateManager->getContext()->m_systemManager->getMessageHandler()->dispatch(msg);
	incrementInvaderBullet();
}

void State_Game::incrementPlayerBullet()
{
	m_playerBulletIndex = ++m_playerBulletIndex % m_levelManager.getPlayerBulletIds().size();
}

void State_Game::incrementInvaderBullet()
{
	m_invaderBulletIndex = ++m_invaderBulletIndex % m_levelManager.getInvaderBulletIds().size();
}

void State_Game::incrementShockwave()
{
	m_shockwaveIndex = ++m_shockwaveIndex % m_levelManager.getShockwaveIds().size();
}

void State_Game::onInvaderDefeated(const int& invaderId)
{
	m_kills++;
	if (--m_remainingInvaders == 0)
		loadNextLevel();
	else
	{
		Comp_Position* posComp = m_stateManager->getContext()->m_actorManager->getActor(invaderId)->getComponent<Comp_Position>(ComponentType::Position);
		const int shockwaveId = m_levelManager.getShockwaveIds()[m_shockwaveIndex];
		m_stateManager->getContext()->m_actorManager->enableActor(shockwaveId);
		Message msg((MessageType)ActorMessageType::Explode);
		msg.m_receiver = shockwaveId;
		msg.m_sender = invaderId;
		msg.m_xy.x = posComp->getPosition().x;
		msg.m_xy.y = posComp->getPosition().y;
		m_stateManager->getContext()->m_systemManager->getMessageHandler()->dispatch(msg);
		incrementShockwave();
		updateInvaderFPS();
	}
}

void State_Game::updateInvaderFPS()
{
	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	for (auto& invaderId : m_levelManager.getInvaderIds())
	{
		Actor* invader = actorManager->getActor(invaderId);
		Comp_Movement* moveComp = invader->getComponent<Comp_Movement>(ComponentType::Movement);
		Comp_Invader* invaderComp = invader->getComponent<Comp_Invader>(ComponentType::Invader);
		Comp_Control* controlComp = invader->getComponent<Comp_Control>(ComponentType::Control);
		Comp_SpriteSheet* spriteComp = invader->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		spriteComp->setFPS(spriteComp->getDefaultFPS() * controlComp->getMaxSpeed() / invaderComp->getBaseSpeed());
	}
}

void State_Game::updateHUD()
{
	m_scoreText.setString("Score: " + std::to_string(0));
	m_levelText.setString("Level: " + std::to_string(m_levelManager.m_level));
	m_livesText.setString("Lives: " + std::to_string(3));
	m_killsText.setString("Kills: " + std::to_string(m_kills));
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
	m_font.loadFromFile(Utils::getWorkingDirectory() + "assets/fonts/game_over.ttf");
	m_scoreText.setFont(m_font);
	m_scoreText.setCharacterSize(m_fontSize);
	m_scoreText.setPosition(m_hudPadding, m_hudPadding);
	m_levelText.setFont(m_font);
	m_levelText.setCharacterSize(m_fontSize);
	m_levelText.setPosition(m_hudPadding, m_hudPadding + m_fontSize);
	m_livesText.setFont(m_font);
	m_livesText.setCharacterSize(m_fontSize);
	m_livesText.setPosition(m_hudPadding, m_hudPadding + 2 * m_fontSize);
	m_killsText.setFont(m_font);
	m_killsText.setCharacterSize(m_fontSize);
	m_killsText.setPosition(m_hudPadding, m_hudPadding + 3 * m_fontSize);
	m_fpsText.setFont(m_font);
	m_fpsText.setCharacterSize(m_fontSize);
	m_fpsText.setPosition(m_hudPadding, m_hudPadding + 4 * m_fontSize);
}

void State_Game::setWindowOutline()
{
	m_background.setSize(m_gameView.getSize() - sf::Vector2f(2, 2));
	m_background.setPosition(1, 1);
	m_background.setFillColor(sf::Color::Black);
	m_background.setOutlineColor(sf::Color::White);
	m_background.setOutlineThickness(1);
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
