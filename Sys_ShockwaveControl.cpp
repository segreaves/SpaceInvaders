#include "Sys_ShockwaveControl.h"
#include "SysManager.h"
#include "WindowManager.h"

Sys_ShockwaveControl::Sys_ShockwaveControl(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_ShockwaveControl::~Sys_ShockwaveControl()
{
	onDestroy();
}

void Sys_ShockwaveControl::start()
{
}

void Sys_ShockwaveControl::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Collision);
	req.set((unsigned int)ComponentType::SpriteSheet);
	req.set((unsigned int)ComponentType::Shockwave);
	m_requirements.emplace_back(req);
}

void Sys_ShockwaveControl::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_ShockwaveControl::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_ShockwaveControl::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
	{
		auto actor = m_systemManager->getActorManager()->getActor(id);
		auto shockwaveComp = actor->getComponent<Comp_Shockwave>(ComponentType::Shockwave);
		shockwaveComp->incrementTime(deltaTime);
		shockwaveComp->setRadius(shockwaveComp->getMaxRadius() * shockwaveComp->getTime() / shockwaveComp->getLifeTime());
		auto spriteComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		spriteComp->getSpriteSheet()->setSpriteScale(sf::Vector2f(2 * shockwaveComp->getRadius() / spriteComp->getSpriteSheet()->getSpriteSize().x, 2 * shockwaveComp->getRadius() / spriteComp->getSpriteSheet()->getSpriteSize().y));
		auto collisionComp = actor->getComponent<Comp_Collision>(ComponentType::Collision);
		collisionComp->setAABB(sf::Vector2f(2 * shockwaveComp->getRadius(), 2 * shockwaveComp->getRadius()));
		if (shockwaveComp->getTime() > shockwaveComp->getLifeTime())
			m_systemManager->addEvent(id, (EventId)ActorEventType::Despawned);
	}
}

void Sys_ShockwaveControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
		case ActorEventType::Despawned:
			m_systemManager->getActorManager()->disableActor(actorId);
			break;
	}
}

void Sys_ShockwaveControl::debugOverlay(WindowManager* windowManager)
{
	for (auto& id : m_actorIds)
	{
		auto actor = m_systemManager->getActorManager()->getActor(id);
		auto shockwaveComp = actor->getComponent<Comp_Shockwave>(ComponentType::Shockwave);
		auto posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		sf::CircleShape circle(shockwaveComp->getRadius());
		circle.setOrigin(shockwaveComp->getRadius(), shockwaveComp->getRadius());
		circle.setPosition(posComp->getPosition());
		circle.setFillColor(sf::Color(255, 255, 0, 50));
		windowManager->getRenderWindow()->draw(circle);
	}
}

void Sys_ShockwaveControl::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	ActorMessageType msgType = (ActorMessageType)msg.m_type;
	switch (msgType)
	{
		case ActorMessageType::Collision:
		{
			ActorManager* actorManager = m_systemManager->getActorManager();
			auto shockwave = actorManager->getActor(msg.m_receiver);
			auto other = actorManager->getActor(msg.m_sender);
			auto shockwavePosComp = shockwave->getComponent<Comp_Position>(ComponentType::Position);
			auto shockwaveColComp = shockwave->getComponent<Comp_Collision>(ComponentType::Collision);
			auto shockwaveComp = shockwave->getComponent<Comp_Shockwave>(ComponentType::Shockwave);
			auto otherPosComp = other->getComponent<Comp_Position>(ComponentType::Position);
			sf::Vector2f direction = otherPosComp->getPosition() - shockwavePosComp->getPosition();
			float distance = sqrt(direction.x * direction.x + direction.y * direction.y);
			float radius = shockwaveComp->getRadius();
			if (distance > radius) return;
			float force = shockwaveComp->getForce() * (1 - distance / radius);
			auto otherMoveComp = other->getComponent<Comp_Movement>(ComponentType::Movement);
			otherMoveComp->accelerate(force * direction / distance);
			break;
		}
	}
}