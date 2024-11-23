#include "Sys_Renderer.h"
#include "SysManager.h"
#include "ActorManager.h"
#include "WindowManager.h"

Sys_Renderer::Sys_Renderer(SysManager* systemManager) :
	Sys(systemManager),
	m_showDamageOverlay(false),
	m_damageOverlayTimer(0),
	m_damageOverlayDuration(0.5f)
{
	onCreate();
}

Sys_Renderer::~Sys_Renderer()
{
	onDestroy();
}

void Sys_Renderer::start()
{
	const auto& view = m_systemManager->getLevelManager()->getViewSpace();
	m_damageOverlay.setSize(sf::Vector2f(view.getSize()));
}

void Sys_Renderer::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
	{
		const auto& actor = m_systemManager->getActorManager()->getActor(id);
		const auto& posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		const auto& spriteSheetComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		if (spriteSheetComp)
		{
			spriteSheetComp->cropSprite();
			spriteSheetComp->updatePosition(posComp->getPosition());
			spriteSheetComp->updateRotation(posComp->getAngle());
		}
		const auto& particlesComp = actor->getComponent<Comp_Particles>(ComponentType::Particles);
		if (particlesComp)
			particlesComp->getParticleSystem()->update(deltaTime);
	}
	if (m_showDamageOverlay)
	{
		m_damageOverlayTimer -= deltaTime;
		if (m_damageOverlayTimer <= 0)
			m_showDamageOverlay = false;
	}
}

void Sys_Renderer::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Renderer::draw(WindowManager* windowManager)
{
	for (auto& id : m_actorIds)
	{
		auto actor = m_systemManager->getActorManager()->getActor(id);
		auto spriteSheetComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		if (spriteSheetComp && spriteSheetComp->isEnabled() && windowManager->getCurrentViewSpace().intersects(spriteSheetComp->getDrawableBounds()))
			draw(windowManager, std::static_pointer_cast<IDrawable>(spriteSheetComp));
		auto particlesComp = actor->getComponent<Comp_Particles>(ComponentType::Particles);
		if (particlesComp && particlesComp->getParticleSystem()->isEnabled())
			draw(windowManager, std::static_pointer_cast<IDrawable>(particlesComp));
	}
	if (m_showDamageOverlay)
	{
		m_damageCurrColor.a = static_cast<sf::Uint8>(255 * (m_damageOverlayTimer / m_damageOverlayDuration));
		m_damageOverlay.setFillColor(m_damageCurrColor);
		windowManager->getRenderWindow()->draw(m_damageOverlay);
	}
}

void Sys_Renderer::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Renderer::notify(const Message& msg)
{
	// only read messages for the player
	if (!hasActor(msg.m_receiver)) return;
	switch ((ActorMessageType)msg.m_type)
	{
	case ActorMessageType::Damage:
	{
		m_showDamageOverlay = true;
		m_damageOverlayTimer = m_damageOverlayDuration;
		m_damageCurrColor = APP_COLOR;
		break;
	}
	}
}

void Sys_Renderer::draw(WindowManager* windowManager, std::shared_ptr<IDrawable> drawable)
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
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Damage, this);
}

void Sys_Renderer::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Damage, this);
}
