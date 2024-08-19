#include "Sys_Bounds.h"
#include "SysManager.h"

Sys_Bounds::Sys_Bounds(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_Bounds::~Sys_Bounds()
{
	unsubscribeFromChannels();
}

void Sys_Bounds::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Collision);
}

void Sys_Bounds::subscribeToChannels()
{
}

void Sys_Bounds::unsubscribeFromChannels()
{
}

void Sys_Bounds::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
		Comp_Position* posComp = actor->getComponent<Comp_Position>(CompType::Position);
		Comp_Collision* colComp = actor->getComponent<Comp_Collision>(CompType::Collision);
		sf::FloatRect actorAABB = colComp->getAABB();
		Comp_BulletControl* bulletControlComp = actor->getComponent<Comp_BulletControl>(CompType::BulletControl);
		if (bulletControlComp)
		{
			// check if bullet is out of bounds (vertically)
			if (actorAABB.top + actorAABB.height < 0 || actorAABB.top > m_viewSpace.getSize().y)
				m_systemManager->getActorManager()->disableActor(actorId);
			continue;
		}
		float resolve = 0;
		// check if player/invader is out of bounds (horizontally)
		if (actorAABB.left < 0)
			resolve = -actorAABB.left;
		else if (actorAABB.left + actorAABB.width > m_viewSpace.getSize().x)
			resolve = -(actorAABB.left + actorAABB.width - m_viewSpace.getSize().x);
		if (resolve != 0)
		{
			m_systemManager->addEvent(actorId, (ActorEvent)ActorEventType::CollidingOnX);
			Message msg((MessageType)ActorMessageType::Resolve);
			msg.m_receiver = actorId;
			msg.m_xy.x = resolve;
			msg.m_xy.y = 0;
			m_systemManager->getMessageHandler()->dispatch(msg);
		}
	}
}

void Sys_Bounds::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Bounds::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Bounds::notify(const Message& msg)
{
}

void Sys_Bounds::setViewSpace(sf::FloatRect viewSpace)
{
	m_viewSpace = viewSpace;
}
