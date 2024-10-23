#include "Sys_Movement.h"
#include "ActorManager.h"
#include "SysManager.h"

Sys_Movement::Sys_Movement(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_Movement::~Sys_Movement()
{
	onDestroy();
}

void Sys_Movement::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Movement);
	m_requirements.emplace_back(req);
}

void Sys_Movement::subscribeToChannels()
{
}

void Sys_Movement::unsubscribeFromChannels()
{
}

void Sys_Movement::start()
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
}

void Sys_Movement::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Movement::notify(const Message& msg)
{
}

void Sys_Movement::move(const ActorId& actorId, const float& deltaTime)
{
	auto actor = m_systemManager->getActorManager()->getActor(actorId);
	auto posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
	auto moveComp = actor->getComponent<Comp_Movement>(ComponentType::Movement);
	// movement
	sf::Vector2f acceleration = sf::Vector2f(
		moveComp->getAcceleration().x * !moveComp->getCollidingOnX(),
		moveComp->getAcceleration().y * !moveComp->getCollidingOnY());
	moveComp->updatePrevVelocity();
	moveComp->addVelocity(acceleration * deltaTime);
	moveComp->setAcceleration(sf::Vector2f(0, 0));
	moveComp->resetCollisionFlags();
	if (moveComp->getFrictionCoefficient() > 0)
		moveComp->applyBaseFriction(moveComp->getVelocity() * deltaTime);
	posComp->move(moveComp->getVelocity() * deltaTime);
}
