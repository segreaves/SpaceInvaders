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
}

void Sys_Animator::unsubscribeFromChannels()
{
}

void Sys_Animator::start()
{
	for (auto& id : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(id);
		Comp_SpriteSheet* spriteComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		spriteComp->resetFrameStep();
		spriteComp->cropSprite();
		spriteComp->setFrameTime(0);
	}
}

void Sys_Animator::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
	{
		Comp_SpriteSheet* spriteSheetComp = m_systemManager->getActorManager()->getActor(id)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		if (spriteSheetComp->isAnimated())
			if (spriteSheetComp->incrementFrameTime(deltaTime) >= spriteSheetComp->getFrameDuration())
			{
				spriteSheetComp->frameStep();
				spriteSheetComp->cropSprite();
				spriteSheetComp->setFrameTime(spriteSheetComp->getFrameTime() - spriteSheetComp->getFrameDuration());
			}
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
}
