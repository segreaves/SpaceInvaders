#include "Sys_Health.h"
#include "SysManager.h"
#include "LevelManager.h"

Sys_Health::Sys_Health(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_Health::~Sys_Health()
{
	onDestroy();
}

void Sys_Health::start()
{
}

void Sys_Health::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Health);
	m_requirements.emplace_back(req);
}

void Sys_Health::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_Health::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_Health::update(const float& deltaTime)
{
}

void Sys_Health::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Health::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Health::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	ActorMessageType msgType = (ActorMessageType)msg.m_type;
	switch (msgType)
	{
	case ActorMessageType::Collision:
	{
		auto actor = m_systemManager->getActorManager()->getActor(msg.m_receiver);
		auto other = m_systemManager->getActorManager()->getActor(msg.m_sender);
		if (other->getTag() == "bullet_invader")
		{
			auto playerHealth = actor->getComponent<Comp_Health>(ComponentType::Health);
			m_systemManager->getLevelManager()->setPlayerLives(playerHealth->takeDamage());
			if (m_systemManager->getLevelManager()->getPlayerLives() <= 0)
				m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
			else
			{
				Message msg((MessageType)ActorMessageType::Damage);
				msg.m_receiver = actor->getId();
				m_systemManager->getMessageHandler()->dispatch(msg);
			}
		}
		break;
	}
	}
}
