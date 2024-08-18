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
	for (auto& id : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(id);
		Comp_Position* posComp = actor->getComponent<Comp_Position>(CompType::Position);
		Comp_Sprite* spriteComp = actor->getComponent<Comp_Sprite>(CompType::Sprite);
		if (!posComp || !spriteComp) continue;
		spriteComp->setPosition(posComp->getPosition());
	}
}

void Sys_Renderer::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Renderer::draw(WindowManager* windowManager)
{
	for (auto& id : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(id);
		Comp_Position* posComp = actor->getComponent<Comp_Position>(CompType::Position);
		Comp_Sprite* spriteComp = actor->getComponent<Comp_Sprite>(CompType::Sprite);
		if (!posComp || !spriteComp) continue;
		// culling
		sf::FloatRect view = windowManager->getViewSpace();
		if (view.intersects(spriteComp->getDrawableBounds()))
			spriteComp->draw(windowManager->getRenderWindow());
	}
}

void Sys_Renderer::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Renderer::notify(const Message& msg)
{
}

void Sys_Renderer::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Sprite);
}

void Sys_Renderer::subscribeToChannels()
{
}

void Sys_Renderer::unsubscribeFromChannels()
{
}
