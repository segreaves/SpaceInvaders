#include "Sys_Control.h"
#include "SysManager.h"
#include "ActorManager.h"
#include <SFML/System/Vector2.hpp>

Sys_Control::Sys_Control(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_Control::~Sys_Control()
{
	unsubscribeFromChannels();
}

void Sys_Control::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Control);
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Movable);
}

void Sys_Control::subscribeToChannels()
{
}

void Sys_Control::unsubscribeFromChannels()
{
}

void Sys_Control::update(const float& deltaTime)
{
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
		Comp_Control* controlComp = actor->getComponent<Comp_Control>(CompType::Control);
		Comp_Movable* moveComp = actor->getComponent<Comp_Movable>(CompType::Movable);
		moveComp->accelerate(controlComp->getMovementInput());
	}
}

void Sys_Control::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Control::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Control::notify(const Message& msg)
{
}
