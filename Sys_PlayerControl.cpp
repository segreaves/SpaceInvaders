#include "Sys_PlayerControl.h"
#include "SysManager.h"
#include "ActorManager.h"
#include <SFML/System/Vector2.hpp>

Sys_PlayerControl::Sys_PlayerControl(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_PlayerControl::~Sys_PlayerControl()
{
	unsubscribeFromChannels();
}

void Sys_PlayerControl::start()
{
}

void Sys_PlayerControl::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Control);
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Movable);
}

void Sys_PlayerControl::subscribeToChannels()
{
}

void Sys_PlayerControl::unsubscribeFromChannels()
{
}

void Sys_PlayerControl::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		Actor* player = actorManager->getActor(id);
		Comp_Control* controlComp = player->getComponent<Comp_Control>(CompType::Control);
		Comp_Movable* moveComp = player->getComponent<Comp_Movable>(CompType::Movable);

		// player movement
		moveComp->accelerate(controlComp->getMovementInput());

		// check if player is out of bounds
		Comp_Collision* colComp = player->getComponent<Comp_Collision>(CompType::Collision);
		sf::FloatRect playerAABB = colComp->getAABB();

		float resolve = 0;
		if (playerAABB.left < 0)
			resolve = -playerAABB.left;
		else if (playerAABB.left + playerAABB.width > m_viewSpace.getSize().x)
			resolve = -(playerAABB.left + playerAABB.width - m_viewSpace.getSize().x);
		if (resolve != 0)
		{
			Comp_Position* posComp = player->getComponent<Comp_Position>(CompType::Position);
			Comp_Movable* moveComp = player->getComponent<Comp_Movable>(CompType::Movable);
			posComp->move(resolve, 0);
			moveComp->setAcceleration(0, moveComp->getAcceleration().y);
			moveComp->setVelocity(0, moveComp->getVelocity().y);
			moveComp->setCollidingOnX(true);
		}
	}
}

void Sys_PlayerControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
		removeActor(actorId);
		break;
	}
}

void Sys_PlayerControl::debugOverlay(WindowManager* windowManager)
{
}

void Sys_PlayerControl::notify(const Message& msg)
{
}

void Sys_PlayerControl::setViewSpace(sf::FloatRect viewSpace)
{
	m_viewSpace = viewSpace;
}
