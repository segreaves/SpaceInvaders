#include "Sys_Animator.h"
#include "SysManager.h"
#include "WindowManager.h"

Sys_Animator::Sys_Animator(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_Animator::~Sys_Animator()
{
	unsubscribeFromChannels();
}

void Sys_Animator::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Movement);
	req.set((unsigned int)ComponentType::SpriteSheet);
	m_requirements.emplace_back(req);
}

void Sys_Animator::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Damage, this);
}

void Sys_Animator::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Damage, this);
}

void Sys_Animator::start()
{
	for (auto& id : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(id);
		auto spriteComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		spriteComp->resetFrameStep();
		spriteComp->cropSprite();
		spriteComp->setFrameTime(0);
	}
}

void Sys_Animator::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
	{
		auto spriteSheetComp = m_systemManager->getActorManager()->getActor(id)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		if (spriteSheetComp->isAnimated())
			handleAnimation(spriteSheetComp, deltaTime);
		if (spriteSheetComp->m_isBlinking)
			handleBlinking(spriteSheetComp, deltaTime);
	}
}

void Sys_Animator::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Animator::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Animator::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	switch ((ActorMessageType)msg.m_type)
	{
		case ActorMessageType::Damage:
		{
			auto spriteComp = m_systemManager->getActorManager()->getActor(msg.m_receiver)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
			spriteComp->startDmgBlink();
			break;
		}
	}
}

void Sys_Animator::handleAnimation(std::shared_ptr<Comp_SpriteSheet> spriteSheetComp, const float& deltaTime)
{
	if (spriteSheetComp->incrementFrameTime(deltaTime) >= spriteSheetComp->getFrameDuration())
	{
		spriteSheetComp->frameStep();
		spriteSheetComp->cropSprite();
		spriteSheetComp->setFrameTime(spriteSheetComp->getFrameTime() - spriteSheetComp->getFrameDuration());
	}
}

void Sys_Animator::handleBlinking(std::shared_ptr<Comp_SpriteSheet> spriteSheetComp, const float& deltaTime)
{
	if (spriteSheetComp->incrementBlinkTime(deltaTime) > m_maxDmgBlinkTime)
	{
		bool doneBlinking = ++spriteSheetComp->m_dmgBlinks > m_dmgBlinkFrames;
		spriteSheetComp->setEnabled(doneBlinking ? true : !spriteSheetComp->isEnabled());
		// reset blink time
		spriteSheetComp->m_dmgBlinkTime = spriteSheetComp->m_dmgBlinkTime - m_maxDmgBlinkTime;
	}
}
