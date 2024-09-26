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

void Sys_Renderer::start()
{
}

void Sys_Renderer::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(id);
		Comp_Position* posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		Comp_SpriteSheet* spriteSheetComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		spriteSheetComp->updatePosition(posComp->getPosition());
	}
}

void Sys_Renderer::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Renderer::draw(WindowManager* windowManager)
{
	for (auto& id : m_actorIds)
	{
		Comp_SpriteSheet* spriteSheetComp = m_systemManager->getActorManager()->getActor(id)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		// culling
		sf::FloatRect view = windowManager->getCurrentViewSpace();
		if (view.intersects(spriteSheetComp->getDrawableBounds()))
			spriteSheetComp->draw(windowManager->getRenderWindow());
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
	m_requirements.set((unsigned int)ComponentType::Position);
	m_requirements.set((unsigned int)ComponentType::SpriteSheet);
}

void Sys_Renderer::subscribeToChannels()
{
}

void Sys_Renderer::unsubscribeFromChannels()
{
}
