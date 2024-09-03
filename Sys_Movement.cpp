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

void Sys_Movement::start()
{
}

void Sys_Movement::setupRequirements()
{
	m_requirements.set((unsigned int)ComponentType::Position);
	m_requirements.set((unsigned int)ComponentType::Movement);
}

void Sys_Movement::subscribeToChannels()
{
}

void Sys_Movement::unsubscribeFromChannels()
{
}

void Sys_Movement::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
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
		Comp_Movement* moveComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Movement>(ComponentType::Movement);
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
}

void Sys_Movement::move(const ActorId& actorId, const float& deltaTime)
{
	Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
	Comp_Position* posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
	Comp_Movement* moveComp = actor->getComponent<Comp_Movement>(ComponentType::Movement);
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
	posComp->move(moveComp->getVelocity() * deltaTime);
}
