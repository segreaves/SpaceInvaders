#include "Sys_BulletControl.h"
#include "SysManager.h"
#include "LevelManager.h"

Sys_BulletControl::Sys_BulletControl(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_BulletControl::~Sys_BulletControl()
{
	unsubscribeFromChannels();
}

void Sys_BulletControl::start()
{
}

void Sys_BulletControl::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Movement);
	req.set((unsigned int)ComponentType::Collision);
	req.set((unsigned int)ComponentType::Bullet);
	m_requirements.emplace_back(req);
}

void Sys_BulletControl::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_BulletControl::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_BulletControl::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		Actor* bullet = actorManager->getActor(id);
		// no need to update bullet movement as their velocity is constant and set up on creation
		Comp_Collision* colComp = bullet->getComponent<Comp_Collision>(ComponentType::Collision);
		sf::FloatRect bulletAABB = colComp->getAABB();
		// check if bullet is out of bounds
		if (bulletAABB.top + bulletAABB.height < 0 || bulletAABB.top > m_levelManager->getViewSpace().getSize().y)
			m_systemManager->addEvent(bullet->getId(), (EventId)ActorEventType::Despawned);
	}
}

void Sys_BulletControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
		m_systemManager->getActorManager()->disableActor(actorId);
		break;
	}
}

void Sys_BulletControl::debugOverlay(WindowManager* windowManager)
{
}

void Sys_BulletControl::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	switch ((ActorMessageType)msg.m_type)
	{
	case ActorMessageType::Collision:
		m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
		break;
	}
}

void Sys_BulletControl::setLevelManager(LevelManager* levelManager)
{
	m_levelManager = levelManager;
}
