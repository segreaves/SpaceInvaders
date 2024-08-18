#include "Sys_AIController.h"
#include "SysManager.h"

Sys_AIController::Sys_AIController(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_AIController::~Sys_AIController()
{
	unsubscribeFromChannels();
}

void Sys_AIController::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Collision);
	m_requirements.set((unsigned int)CompType::AIControl);
}

void Sys_AIController::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Resolve, this);
}

void Sys_AIController::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Resolve, this);
}

void Sys_AIController::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
		move(id, deltaTime);
}

void Sys_AIController::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	Comp_Movable* moveComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Movable>(CompType::Movable);
	switch (eventId)
	{
	case ActorEventType::CollidingOnX:
		// reverse direction
		// increase speed
		break;
	case ActorEventType::CollidingOnY:
		// if colliding on lower bound of the level, game over
		break;
	}
}

void Sys_AIController::debugOverlay(WindowManager* windowManager)
{
}

void Sys_AIController::notify(const Message& msg)
{
	ActorMessageType msgType = (ActorMessageType)msg.m_type;
	switch (msgType)
	{
	case ActorMessageType::Resolve:
		// resolve collision for all invaders save original colliding invader
		// original colliding invader has already been resolved
		for (auto& actorId : m_actorIds)
		{
			if (actorId == msg.m_receiver) continue;
			Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
			Comp_Position* posComp = actor->getComponent<Comp_Position>(CompType::Position);
			Comp_Collision* colComp = actor->getComponent<Comp_Collision>(CompType::Collision);
			posComp->move(msg.m_xy.x, msg.m_xy.y);
			colComp->setPosition(posComp->getPosition());
		}
		break;
	}
}

void Sys_AIController::move(const ActorId& actorId, const float& deltaTime)
{
}
