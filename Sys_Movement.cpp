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
	{
		const auto& actor = m_systemManager->getActorManager()->getActor(id);
		const auto& posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		const auto& moveComp = actor->getComponent<Comp_Movement>(ComponentType::Movement);
		
		sf::Vector2f acceleration = moveComp->getAcceleration();
		// get friction as opposite to acceleration
		const auto friction = moveComp->getFrictionCoefficient() * moveComp->getVelocity();
		acceleration.x = std::abs(acceleration.x) > std::abs(friction.x) ? acceleration.x - friction.x : 0;
		acceleration.y = std::abs(acceleration.y) > std::abs(friction.y) ? acceleration.y - friction.y : 0;

		acceleration.x = moveComp->getAcceleration().x * !moveComp->getCollidingOnX();
		acceleration.y = moveComp->getAcceleration().y * !moveComp->getCollidingOnY();

		// handle movement
		moveComp->addVelocity(acceleration * deltaTime);
		moveComp->resetCollisionFlags();
		//moveComp->applyFriction(deltaTime);
		posComp->move(moveComp->getVelocity() * deltaTime);
		moveComp->setAcceleration(sf::Vector2f(0, 0));
		moveComp->updateSpeedChange();
	}
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
