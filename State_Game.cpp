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
	m_playerId(0),
	m_bulletIndex(0),
	m_remainingInvaders(0),
	m_kills(0)
{
	setHUDStyle();
	setWindowOutline();
}

void State_Game::update(const float& deltaTime)
{
	m_stateManager->getContext()->m_systemManager->update(deltaTime);
	updateHUD();
}

void State_Game::draw()
{
	WindowManager* windowManager = m_stateManager->getContext()->m_windowManager;
	windowManager->drawToGameplayView(m_background);
	m_stateManager->getContext()->m_systemManager->draw(windowManager);
	drawHUD();
}

void State_Game::onCreate()
{
	m_levelManager.setViewSpace(m_stateManager->getContext()->m_windowManager->getViewSpace());
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->setLevelManager(&m_levelManager);
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_PlayerControl>(SystemType::PlayerControl)->setLevelManager(&m_levelManager);
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_BulletControl>(SystemType::BulletControl)->setLevelManager(&m_levelManager);
	createPlayer();
	createBullets(100);
	createInvaders();
	createBunkers();
	loadNextLevel();
}

void State_Game::onDestroy()
{
}

void State_Game::activate()
{
	m_stateManager->getContext()->m_controller->m_onMove.addCallback("Game_onMove", std::bind(&State_Game::onPlayerMove, this, std::placeholders::_1));
	m_stateManager->getContext()->m_controller->m_onShoot.addCallback("Game_onShoot", std::bind(&State_Game::onPlayerShoot, this));
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->m_invaderDefeated.addCallback("Game_onInvaderDefeated", std::bind(&State_Game::onInvaderDefeated, this));
}

void State_Game::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onMove.removeCallback("Game_onMove");
	m_stateManager->getContext()->m_controller->m_onMove.removeCallback("Game_onShoot");
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_InvaderControl>(SystemType::InvaderControl)->m_invaderDefeated.removeCallback("Game_onInvaderDefeated");
}

void State_Game::loadNextLevel()
{
	m_levelManager.m_level++;
	m_levelManager.setInvaderSpawnPoints(m_invaderSize);
	m_remainingInvaders = m_levelManager.getTotalInvaders();
	m_stateManager->getContext()->m_actorManager->enableActor(m_playerId);
	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	for (auto& invaderId : m_invaders)
	{
		actorManager->enableActor(invaderId);
		Actor* invader = actorManager->getActor(invaderId);
		Comp_Control* controlComp = invader->getComponent<Comp_Control>(CompType::Control);
		controlComp->setMaxSpeed(m_levelManager.getInvaderStartSpeed() + (m_levelManager.m_level - 1) * m_levelManager.getLevelSpeedIncrease());
	}
	m_stateManager->getContext()->m_systemManager->start();
}

void State_Game::createPlayer()
{
	Bitmask mask;
	mask.set((unsigned int)CompType::Position);
	mask.set((unsigned int)CompType::Sprite);
	mask.set((unsigned int)CompType::Movement);
	mask.set((unsigned int)CompType::Control);
	mask.set((unsigned int)CompType::Collision);
	mask.set((unsigned int)CompType::Player);

	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	m_playerId = actorManager->createActor(mask, "player");
	Comp_Position* posComp = actorManager->getActor(m_playerId)->getComponent<Comp_Position>(CompType::Position);
	Comp_Movement* moveComp = actorManager->getActor(m_playerId)->getComponent<Comp_Movement>(CompType::Movement);
	Comp_Collision* colComp = actorManager->getActor(m_playerId)->getComponent<Comp_Collision>(CompType::Collision);
	Comp_Sprite* spriteComp = actorManager->getActor(m_playerId)->getComponent<Comp_Sprite>(CompType::Sprite);
	Comp_Control* controlComp = actorManager->getActor(m_playerId)->getComponent<Comp_Control>(CompType::Control);
	spriteComp->setSize(m_playerSize);
	colComp->setSize(spriteComp->getSize());
	controlComp->setMaxSpeed(2500);
	controlComp->setMaxAcceleration(50000);
	posComp->setPosition(m_levelManager.getPlayerSpawnPoint());
}

void State_Game::createBullets(unsigned int maxBullets)
{
	m_bullets.clear();

	Bitmask mask;
	mask.set((unsigned int)CompType::Position);
	mask.set((unsigned int)CompType::Sprite);
	mask.set((unsigned int)CompType::Movement);
	mask.set((unsigned int)CompType::Collision);
	mask.set((unsigned int)CompType::Bullet);

	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	for (unsigned int i = 0; i < maxBullets; i++)
	{
		int bulletId = actorManager->createActor(mask, "bullet");
		m_bullets.push_back(bulletId);
		Comp_Collision* colComp = actorManager->getActor(bulletId)->getComponent<Comp_Collision>(CompType::Collision);
		Comp_Sprite* spriteComp = actorManager->getActor(bulletId)->getComponent<Comp_Sprite>(CompType::Sprite);
		Comp_Movement* moveComp = actorManager->getActor(bulletId)->getComponent<Comp_Movement>(CompType::Movement);
		spriteComp->setSize(m_bulletSize);
		colComp->setSize(spriteComp->getSize());
		moveComp->setFrictionCoefficient(0.0f);
	}
}

void State_Game::createInvaders()
{
	Bitmask mask;
	mask.set((unsigned int)CompType::Position);
	mask.set((unsigned int)CompType::Sprite);
	mask.set((unsigned int)CompType::Movement);
	mask.set((unsigned int)CompType::Collision);
	mask.set((unsigned int)CompType::Control);
	mask.set((unsigned int)CompType::Invader);
	
	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	int totalInvaders = m_levelManager.getTotalInvaders();
	for (int i = 0; i < totalInvaders; i++)
	{
		int invaderId = actorManager->createActor(mask, "invader");
		m_invaders.push_back(invaderId);
		Comp_Collision* colComp = actorManager->getActor(invaderId)->getComponent<Comp_Collision>(CompType::Collision);
		Comp_Sprite* spriteComp = actorManager->getActor(invaderId)->getComponent<Comp_Sprite>(CompType::Sprite);
		Comp_Movement* moveComp = actorManager->getActor(invaderId)->getComponent<Comp_Movement>(CompType::Movement);
		Comp_Control* controlComp = actorManager->getActor(invaderId)->getComponent<Comp_Control>(CompType::Control);
		spriteComp->setSize(m_invaderSize);
		colComp->setSize(m_invaderSize);
		controlComp->setMaxAcceleration(25000);
		moveComp->setFrictionCoefficient(15.0f);
	}
}

void State_Game::createBunkers()
{
	m_levelManager.setBunkerSpawnPoints();
	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	for (auto& spawnPoint : m_levelManager.getBunkerSpawnPoints())
	{
		Bitmask mask;
		mask.set((unsigned int)CompType::Position);
		mask.set((unsigned int)CompType::Collision);
		mask.set((unsigned int)CompType::Sprite);
		mask.set((unsigned int)CompType::Bunker);

		int bunkerId = actorManager->createActor(mask, "bunker");
		Comp_Position* posComp = actorManager->getActor(bunkerId)->getComponent<Comp_Position>(CompType::Position);
		Comp_Collision* colComp = actorManager->getActor(bunkerId)->getComponent<Comp_Collision>(CompType::Collision);
		Comp_Sprite* spriteComp = actorManager->getActor(bunkerId)->getComponent<Comp_Sprite>(CompType::Sprite);
		posComp->setPosition(spawnPoint);
		spriteComp->setSize(m_levelManager.getBunkerSize());
		colComp->setSize(m_levelManager.getBunkerSize());
		actorManager->enableActor(bunkerId);
	}
}

void State_Game::onPlayerMove(sf::Vector2f xy)
{
	Comp_Control* controlComp = m_stateManager->getContext()->m_actorManager->getActor(m_playerId)->getComponent<Comp_Control>(CompType::Control);
	controlComp->setMovementInput(xy);
}

void State_Game::onPlayerShoot()
{
	m_stateManager->getContext()->m_actorManager->enableActor(m_bullets[m_bulletIndex]);
	Message msg((MessageType)ActorMessageType::Shoot);
	msg.m_sender = m_playerId;
	msg.m_receiver = m_bullets[m_bulletIndex];
	msg.m_xy.x = 0;
	msg.m_xy.y = -1;
	m_stateManager->getContext()->m_systemManager->getMessageHandler()->dispatch(msg);
	incrementBullet();
}

void State_Game::incrementBullet()
{
	m_bulletIndex = ++m_bulletIndex % m_bullets.size();
}

void State_Game::onInvaderDefeated()
{
	m_kills++;
	if (--m_remainingInvaders == 0)
		loadNextLevel();
}

void State_Game::updateHUD()
{
	m_scoreText.setString("Score: " + std::to_string(0));
	m_levelText.setString("Level: " + std::to_string(m_levelManager.m_level));
	m_livesText.setString("Lives: " + std::to_string(3));
	m_killsText.setString("Kills: " + std::to_string(m_kills));
}

void State_Game::drawHUD()
{

	m_stateManager->getContext()->m_windowManager->drawToHudView(m_scoreText);
	m_stateManager->getContext()->m_windowManager->drawToHudView(m_levelText);
	m_stateManager->getContext()->m_windowManager->drawToHudView(m_livesText);
	m_stateManager->getContext()->m_windowManager->drawToHudView(m_killsText);
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
}

void State_Game::setWindowOutline()
{
	m_background.setSize(m_stateManager->getContext()->m_windowManager->getGameplayViewSpace().getSize() - sf::Vector2f(2, 2));
	m_background.setPosition(1, 1);
	m_background.setFillColor(sf::Color::Black);
	m_background.setOutlineColor(sf::Color::White);
	m_background.setOutlineThickness(1);
}
