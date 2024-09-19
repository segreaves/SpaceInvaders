#include "Sys_Renderer.h"
#include "SysManager.h"
#include "ActorManager.h"
#include "WindowManager.h"

Sys_Renderer::Sys_Renderer(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_Renderer::~Sys_Renderer()
{
	unsubscribeFromChannels();
}

void Sys_Renderer::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	for (auto& id : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(id);
		Comp_Position* posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		Comp_Sprite* spriteComp = actor->getComponent<Comp_Sprite>(ComponentType::Sprite);
		spriteComp->setPosition(posComp->getPosition());
		Comp_SpriteSheet* spriteSheetComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		if (spriteSheetComp)
		{
			spriteSheetComp->updatePosition(posComp->getPosition());
		}
	}
}

void Sys_Renderer::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
		removeActor(actorId);
		break;
	}
}

void Sys_Renderer::draw(WindowManager* windowManager)
{
	for (auto& id : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(id);
		Comp_Position* posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		Comp_Sprite* spriteComp = actor->getComponent<Comp_Sprite>(ComponentType::Sprite);
		Comp_SpriteSheet* spriteSheetComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		// culling
		sf::FloatRect view = windowManager->getCurrentViewSpace();
		/*if (view.intersects(spriteComp->getDrawableBounds()))
			spriteComp->draw(windowManager->getRenderWindow());*/
		if (spriteSheetComp)
		{
			if (view.intersects(spriteSheetComp->getDrawableBounds()))
			spriteSheetComp->draw(windowManager->getRenderWindow());
		}
	}
}

void Sys_Renderer::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Renderer::notify(const Message& msg)
{
}

void Sys_Renderer::start()
{
}

void Sys_Renderer::setupRequirements()
{
	m_requirements.set((unsigned int)ComponentType::Position);
	m_requirements.set((unsigned int)ComponentType::Sprite);
}

void Sys_Renderer::subscribeToChannels()
{
}

void Sys_Renderer::unsubscribeFromChannels()
{
}
