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
	loadNextLevel();
	createPlayer();
	createBullets(100);
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
}

void State_Game::deactivate()
{
	m_stateManager->getContext()->m_controller->m_onMove.removeCallback("Game_onMove");
}

void State_Game::loadNextLevel()
{
	if (m_level)
		delete m_level;
	m_level = new Level(m_stateManager->getContext());
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_Collision>(SystemType::Collision)->setLevel(m_level);
	m_stateManager->getContext()->m_systemManager->getSystem<Sys_BulletSpawner>(SystemType::BulletSpawner)->setLevel(m_level);
}

void State_Game::createPlayer()
{
	Bitmask mask;
	mask.set((unsigned int)CompType::Position);
	mask.set((unsigned int)CompType::Sprite);
	mask.set((unsigned int)CompType::Movable);
	mask.set((unsigned int)CompType::PlayerControl);
	mask.set((unsigned int)CompType::Collision);

	m_playerId = m_stateManager->getContext()->m_actorManager->createActor(mask);
	// set player in center-bottom of the view space
	sf::FloatRect viewSpace = m_stateManager->getContext()->m_windowManager->getViewSpace();
	Comp_Position* posComp = m_stateManager->getContext()->m_actorManager->getActor(m_playerId)->getComponent<Comp_Position>(CompType::Position);
	posComp->setPosition(viewSpace.left + viewSpace.width / 2.f, viewSpace.top + viewSpace.height * 0.9f);
}

void State_Game::createBullets(unsigned int maxBullets)
{
	Bitmask mask;
	mask.set((unsigned int)CompType::Position);
	mask.set((unsigned int)CompType::Sprite);
	mask.set((unsigned int)CompType::Movable);
	mask.set((unsigned int)CompType::Collision);
	mask.set((unsigned int)CompType::BulletControl);

	ActorManager* actorManager = m_stateManager->getContext()->m_actorManager;
	for (unsigned int i = 0; i < maxBullets; i++)
	{
		int bulletId = actorManager->createActor(mask, false);
		Comp_Movable* moveComp = actorManager->getActor(bulletId)->getComponent<Comp_Movable>(CompType::Movable);
		moveComp->setFrictionCoefficient(0.0f);
	}
}

void State_Game::onPlayerMove(sf::Vector2f xy)
{
	Message msg((MessageType)ActorMessageType::Move);
	msg.m_receiver = m_playerId;
	msg.m_xy.x = xy.x;
	msg.m_xy.y = 0;
	m_stateManager->getContext()->m_systemManager->getMessageHandler()->dispatch(msg);
}

void State_Game::onPlayerShoot()
{
	
}
