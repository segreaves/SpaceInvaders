#include "State_Game.h"
#include "StateManager.h"
#include "Comp_Position.h"
#include "Comp_Sprite.h"
#include "Level.h"

State_Game::State_Game(StateManager* stateManager) :
	State(stateManager),
	m_playerId(0),
	m_level(nullptr)
{
}

void State_Game::update(const float& deltaTime)
{
	Context* context = m_stateManager->getContext();
	context->m_systemManager->update(deltaTime);
}

void State_Game::draw()
{
	Context* context = m_stateManager->getContext();
	context->m_systemManager->draw(context->m_windowManager);
}

void State_Game::onCreate()
{
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_AIControl>(SystemType::AIControl)->setViewSpace(m_stateManager->getContext()->m_windowManager->getViewSpace());
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_PlayerControl>(SystemType::PlayerControl)->setViewSpace(m_stateManager->getContext()->m_windowManager->getViewSpace());
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_BulletControl>(SystemType::BulletControl)->setViewSpace(m_stateManager->getContext()->m_windowManager->getViewSpace());
	loadNextLevel();
}

void State_Game::onDestroy()
{
	delete m_level;
	m_level = nullptr;
}

void State_Game::activate()
{
	m_stateManager->getContext()->m_controller->m_onMove.addCallback("Game_onMove", std::bind(&State_Game::onPlayerMove, this, std::placeholders::_1));
	m_stateManager->getContext()->m_controller->m_onShoot.addCallback("Game_onShoot", std::bind(&State_Game::onPlayerShoot, this));
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_AIControl>(SystemType::AIControl)->m_invadersDefeated.addCallback("Game_onInvadersDefeated", std::bind(&State_Game::loadNextLevel, this));
}

void State_Game::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onMove.removeCallback("Game_onMove");
	m_stateManager->getContext()->m_controller->m_onMove.removeCallback("Game_onShoot");
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_AIControl>(SystemType::AIControl)->m_invadersDefeated.removeCallback("Game_onInvadersDefeated");
}

void State_Game::loadNextLevel()
{
	if (m_level)
		delete m_level;
	m_level = new Level(m_stateManager->getContext());
	createPlayer();
	createBullets(100);
	createInvaders();
	m_stateManager->getContext()->m_systemManager->start();
}

void State_Game::createPlayer()
{
	Bitmask mask;
	mask.set((unsigned int)CompType::Position);
	mask.set((unsigned int)CompType::Sprite);
	mask.set((unsigned int)CompType::Movable);
	mask.set((unsigned int)CompType::Control);
	mask.set((unsigned int)CompType::Collision);
	mask.set((unsigned int)CompType::Player);

	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	m_playerId = actorManager->createActor(mask, "player");
	Actor* player = actorManager->getActor(m_playerId);
	// set player in center-bottom of the view space
	sf::FloatRect viewSpace = m_stateManager->getContext()->m_windowManager->getViewSpace();
	Comp_Position* posComp = player->getComponent<Comp_Position>(CompType::Position);
	Comp_Collision* colComp = player->getComponent<Comp_Collision>(CompType::Collision);
	Comp_Sprite* spriteComp = player->getComponent<Comp_Sprite>(CompType::Sprite);
	spriteComp->setSize(m_playerSize);
	colComp->setSize(spriteComp->getSize());
	posComp->setPosition(m_level->getPlayerSpawnPoint(viewSpace));
	actorManager->enableActor(m_playerId);
}

void State_Game::createBullets(unsigned int maxBullets)
{
	Bitmask mask;
	mask.set((unsigned int)CompType::Position);
	mask.set((unsigned int)CompType::Sprite);
	mask.set((unsigned int)CompType::Movable);
	mask.set((unsigned int)CompType::Collision);
	mask.set((unsigned int)CompType::Control);
	mask.set((unsigned int)CompType::Bullet);

	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	for (unsigned int i = 0; i < maxBullets; i++)
	{
		int bulletId = actorManager->createActor(mask, "bullet");
		Actor* bullet = actorManager->getActor(bulletId);
		Comp_Collision* colComp = m_stateManager->getContext()->m_actorManager->getActor(bulletId)->getComponent<Comp_Collision>(CompType::Collision);
		Comp_Sprite* spriteComp = m_stateManager->getContext()->m_actorManager->getActor(bulletId)->getComponent<Comp_Sprite>(CompType::Sprite);
		spriteComp->setSize(m_bulletSize);
		colComp->setSize(spriteComp->getSize());
		Comp_Movable* moveComp = bullet->getComponent<Comp_Movable>(CompType::Movable);
		moveComp->setFrictionCoefficient(0.0f);
		m_stateManager->getContext()->m_systemManager->getSystem<Sys_BulletControl>(SystemType::BulletControl)->addBullet(bullet);
	}
}

void State_Game::createInvaders()
{
	Bitmask mask;
	mask.set((unsigned int)CompType::Position);
	mask.set((unsigned int)CompType::Sprite);
	mask.set((unsigned int)CompType::Movable);
	mask.set((unsigned int)CompType::Collision);
	mask.set((unsigned int)CompType::Control);
	mask.set((unsigned int)CompType::AI);

	std::vector<sf::Vector2f> spawnPoints = m_level->getInvaderSpawnPoints(m_stateManager->getContext()->m_windowManager->getViewSpace(), m_invaderSize);
	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	for (auto& spawnPoint : spawnPoints)
	{
		int invaderId = actorManager->createActor(mask, "invader");
		Actor* invader = actorManager->getActor(invaderId);
		// arrange invaders in grids
		Comp_Position* posComp = invader->getComponent<Comp_Position>(CompType::Position);
		Comp_Collision* colComp = invader->getComponent<Comp_Collision>(CompType::Collision);
		Comp_Sprite* spriteComp = invader->getComponent<Comp_Sprite>(CompType::Sprite);
		Comp_AI* aiComp = invader->getComponent<Comp_AI>(CompType::AI);
		Comp_Movable* moveComp = invader->getComponent<Comp_Movable>(CompType::Movable);
		spriteComp->setSize(m_invaderSize);
		colComp->setSize(m_invaderSize);
		posComp->setPosition(spawnPoint);
		aiComp->setTarget(posComp->getPosition());
		moveComp->setMaxSpeed(1e4f);
		actorManager->enableActor(invaderId);
	}
}

void State_Game::onPlayerMove(sf::Vector2f xy)
{
	Comp_Control* controlComp = m_stateManager->getContext()->m_actorManager->getActor(m_playerId)->getComponent<Comp_Control>(CompType::Control);
	controlComp->setMovementInput(xy);
}

void State_Game::onPlayerShoot()
{
	Message msg((MessageType)ActorMessageType::Shoot);
	msg.m_sender = m_playerId;
	msg.m_xy.x = 0;
	msg.m_xy.y = -1;
	m_stateManager->getContext()->m_systemManager->getMessageHandler()->dispatch(msg);
}
