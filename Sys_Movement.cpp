#include "Sys_Movement.h"
#include "ActorManager.h"
#include "SysManager.h"
#include <iostream>

Sys_Movement::Sys_Movement(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

void Sys_Movement::setupRequirements()
{
	m_requirements = Bitmask();
	m_requirements.set((unsigned int)CompType::Position, true);
	m_requirements.set((unsigned int)CompType::Movable, true);
}

void Sys_Movement::subscribeToChannels()
{
}

void Sys_Movement::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
		move(id, deltaTime);
}

void Sys_Movement::handleEvent(const ActorId& actorId, const ActorEvent& msg)
{
}

void Sys_Movement::notify(const Message& msg)
{
}

void Sys_Movement::move(const ActorId& actorId, const float& deltaTime)
{
	Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
	Comp_Position* posComp = actor->getComponent<Comp_Position>(CompType::Position);
	Comp_Movable* moveComp = actor->getComponent<Comp_Movable>(CompType::Movable);
	moveComp->addVelocity(moveComp->getAcceleration() * deltaTime);
	moveComp->setAcceleration(sf::Vector2f(0, 0));
	if (moveComp->getFrictionCoefficient() > 0)
		moveComp->applyBaseFriction(moveComp->getVelocity() * deltaTime);
	float speed = sqrt(
		pow(moveComp->getVelocity().x, 2) + 
		pow(moveComp->getVelocity().y, 2));
	if (speed > moveComp->getMaxSpeed())
	{
		moveComp->setVelocity(
			sf::Vector2f(
				moveComp->getMaxSpeed() * moveComp->getVelocity().x / speed,
				moveComp->getMaxSpeed() * moveComp->getVelocity().y / speed));
	}
	posComp->move(moveComp->getVelocity() * deltaTime);
}
