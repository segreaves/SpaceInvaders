#include "Sys_Movement.h"
#include "ActorManager.h"
#include "SysManager.h"

Sys_Movement::Sys_Movement(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_Movement::~Sys_Movement()
{
	unsubscribeFromChannels();
}

void Sys_Movement::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Movable);
}

void Sys_Movement::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Resolve, this);
}

void Sys_Movement::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Resolve, this);
}

void Sys_Movement::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
		move(id, deltaTime);
}

void Sys_Movement::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
		removeActor(actorId);
		break;
	case ActorEventType::CollidingOnX:
		Comp_Movable* moveComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Movable>(CompType::Movable);
		moveComp->setVelocity(0, moveComp->getVelocity().y);
		moveComp->setAcceleration(0, moveComp->getAcceleration().y);
		break;
	}
}

void Sys_Movement::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Movement::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	ActorMessageType msgType = (ActorMessageType)msg.m_type;
	switch (msgType)
	{
	case ActorMessageType::Resolve:
		Actor* actor = m_systemManager->getActorManager()->getActor(msg.m_receiver);
		Comp_Position* posComp = actor->getComponent<Comp_Position>(CompType::Position);
		posComp->move(msg.m_xy.x, msg.m_xy.y);
		break;
	}
}

void Sys_Movement::move(const ActorId& actorId, const float& deltaTime)
{
	Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
	Comp_Position* posComp = actor->getComponent<Comp_Position>(CompType::Position);
	Comp_Movable* moveComp = actor->getComponent<Comp_Movable>(CompType::Movable);
	sf::Vector2f acceleration = sf::Vector2f(
		moveComp->getAcceleration().x * !moveComp->getCollidingOnX(),
		moveComp->getAcceleration().y * !moveComp->getCollidingOnY());
	moveComp->addVelocity(acceleration * deltaTime);
	moveComp->setAcceleration(sf::Vector2f(0, 0));
	moveComp->resetCollisionFlags();
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
