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

void Sys_Animator::start()
{
	for (auto& id : m_actorIds)
	{
		Comp_SpriteSheet* spriteComp = m_systemManager->getActorManager()->getActor(id)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		spriteComp->cropSprite();
	}
}

void Sys_Animator::setupRequirements()
{
	m_requirements.set((unsigned int)ComponentType::Position);
	m_requirements.set((unsigned int)ComponentType::Movement);
	m_requirements.set((unsigned int)ComponentType::SpriteSheet);
}

void Sys_Animator::subscribeToChannels()
{
}

void Sys_Animator::unsubscribeFromChannels()
{
}

void Sys_Animator::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(id);
		Comp_Position* posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		Comp_Movement* moveComp = actor->getComponent<Comp_Movement>(ComponentType::Movement);
		Comp_SpriteSheet* spriteSheetComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		spriteSheetComp->updatePosition(posComp->getPosition());
		if (spriteSheetComp->isAnimated())
		{
			if (spriteSheetComp->incrementFrameTime(deltaTime) >= spriteSheetComp->getFrameDuration())
			{
				spriteSheetComp->frameStep();
				spriteSheetComp->cropSprite();
				spriteSheetComp->setFrameTime(spriteSheetComp->getFrameTime() - spriteSheetComp->getFrameDuration());
			}
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
