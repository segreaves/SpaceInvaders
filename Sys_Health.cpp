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
	req.set((unsigned int)ComponentType::Collision);
	req.set((unsigned int)ComponentType::SpriteSheet);
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
	for (auto& id : m_actorIds)
	{
		auto actor = m_systemManager->getActorManager()->getActor(id);
		auto spriteSheetComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		handleBlinking(spriteSheetComp, deltaTime);
	}
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
			auto spriteSheetComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
			if (!spriteSheetComp->m_isBlinking)
			{
				auto playerHealth = actor->getComponent<Comp_Health>(ComponentType::Health);
				m_systemManager->getLevelManager()->setPlayerLives(playerHealth->takeDamage());
				if (m_systemManager->getLevelManager()->getPlayerLives() > 0)
					spriteSheetComp->startDmgBlink();
				else
					m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
			}
		}
		else if (other->getTag() == "invader")
		{
			m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Invaded);
		}
		break;
	}
	}
}

void Sys_Health::handleBlinking(std::shared_ptr<Comp_SpriteSheet> spriteSheetComp, const float& deltaTime)
{
	if (!spriteSheetComp->m_isBlinking) return;
	if (spriteSheetComp->incrementBlinkTime(deltaTime) > m_maxDmgBlinkTime)
	{
		bool doneBlinking = ++spriteSheetComp->m_dmgBlinks > m_dmgBlinkFrames;
		if (doneBlinking)
			spriteSheetComp->stopDmgBlink();
		else
			spriteSheetComp->setEnabled(!spriteSheetComp->isEnabled());
		// reset blink time
		spriteSheetComp->m_dmgBlinkTime = spriteSheetComp->m_dmgBlinkTime - m_maxDmgBlinkTime;
	}
}
