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
		if (spriteSheetComp)
			spriteSheetComp->updatePosition(posComp->getPosition());
		Comp_Particles* particlesComp = actor->getComponent<Comp_Particles>(ComponentType::Particles);
		if (particlesComp)
			particlesComp->getParticleSystem()->update(deltaTime);
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
		Comp_SpriteSheet* spriteSheetComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		if (spriteSheetComp && spriteSheetComp->isEnabled() && windowManager->getCurrentViewSpace().intersects(spriteSheetComp->getDrawableBounds()))
			draw(windowManager, static_cast<IDrawable*>(spriteSheetComp));
		Comp_Particles* particlesComp = actor->getComponent<Comp_Particles>(ComponentType::Particles);
		if (particlesComp && particlesComp->getParticleSystem()->isEnabled())
			draw(windowManager, static_cast<IDrawable*>(particlesComp));
	}
}

void Sys_Renderer::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Renderer::notify(const Message& msg)
{
}

void Sys_Renderer::draw(WindowManager* windowManager, IDrawable* drawable)
{
	if (!drawable) return;
	drawable->draw(windowManager->getRenderWindow());
}

void Sys_Renderer::setupRequirements()
{
	Bitmask req_spriteSheet;
	req_spriteSheet.set((unsigned int)ComponentType::Position);
	req_spriteSheet.set((unsigned int)ComponentType::SpriteSheet);
	m_requirements.emplace_back(req_spriteSheet);
	Bitmask req_particles;
	req_particles.set((unsigned int)ComponentType::Position);
	req_particles.set((unsigned int)ComponentType::Particles);
	m_requirements.emplace_back(req_particles);
}

void Sys_Renderer::subscribeToChannels()
{
}

void Sys_Renderer::unsubscribeFromChannels()
{
}
