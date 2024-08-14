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

void Sys_Control::setupRequirements()
{
	m_requirements = Bitmask();
	m_requirements.set((unsigned int)CompType::Controller);
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Movable);
}

void Sys_Control::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessage::Move, this);
}

void Sys_Control::update(const float& deltaTime)
{
}

void Sys_Control::handleEvent(const ActorId& actorId, const ActorEvent& msg)
{
}

void Sys_Control::notify(const Message& msg)
{
	switch ((ActorMessage)msg.m_type)
	{
	case ActorMessage::Move:
		move(msg.m_receiver, msg.m_xy.x, msg.m_xy.y);
		break;
	}
}

void Sys_Control::move(const ActorId& actorId, const float& x, const float& y)
{
	Comp_Movable* moveComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Movable>(CompType::Movable);
	moveComp->accelerate(sf::Vector2f(x, y));
}
