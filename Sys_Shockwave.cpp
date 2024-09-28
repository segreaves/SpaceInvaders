#include "Sys_Shockwave.h"
#include "SysManager.h"

Sys_Shockwave::Sys_Shockwave(SysManager* systemManager) : Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_Shockwave::~Sys_Shockwave()
{
	unsubscribeFromChannels();
}

void Sys_Shockwave::start()
{
}

void Sys_Shockwave::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Collision);
	req.set((unsigned int)ComponentType::SpriteSheet);
	req.set((unsigned int)ComponentType::Shockwave);
	m_requirements.emplace_back(req);
}

void Sys_Shockwave::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_Shockwave::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_Shockwave::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(id);
		Comp_Shockwave* shockwaveComp = actor->getComponent<Comp_Shockwave>(ComponentType::Shockwave);
		shockwaveComp->incrementTime(deltaTime);
		shockwaveComp->setRadius(shockwaveComp->getMaxRadius() * shockwaveComp->getTime() / shockwaveComp->getLifeTime());
		Comp_SpriteSheet* spriteComp = actor->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		spriteComp->getSpriteSheet()->setSpriteScale(sf::Vector2f(2 * shockwaveComp->getRadius() / spriteComp->getSpriteSheet()->getSpriteSize().x, 2 * shockwaveComp->getRadius() / spriteComp->getSpriteSheet()->getSpriteSize().y));
		Comp_Collision* collisionComp = actor->getComponent<Comp_Collision>(ComponentType::Collision);
		collisionComp->setAABB(sf::Vector2f(2 * shockwaveComp->getRadius(), 2 * shockwaveComp->getRadius()));
		if (shockwaveComp->getTime() > shockwaveComp->getLifeTime())
			m_systemManager->addEvent(id, (EventId)ActorEventType::Despawned);
	}
}

void Sys_Shockwave::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
		case ActorEventType::Despawned:
			m_systemManager->getActorManager()->disableActor(actorId);
			break;
	}
}

void Sys_Shockwave::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Shockwave::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	ActorMessageType msgType = (ActorMessageType)msg.m_type;
	switch (msgType)
	{
		case ActorMessageType::Collision:
		{
			ActorManager* actorManager = m_systemManager->getActorManager();
			Actor* shockwave = actorManager->getActor(msg.m_receiver);
			Actor* other = actorManager->getActor(msg.m_sender);
			Comp_Position* shockwavePosComp = shockwave->getComponent<Comp_Position>(ComponentType::Position);
			Comp_Collision* shockwaveColComp = shockwave->getComponent<Comp_Collision>(ComponentType::Collision);
			Comp_Shockwave* shockwaveComp = shockwave->getComponent<Comp_Shockwave>(ComponentType::Shockwave);
			Comp_Position* otherPosComp = other->getComponent<Comp_Position>(ComponentType::Position);
			sf::Vector2f direction = otherPosComp->getPosition() - shockwavePosComp->getPosition();
			float distance = sqrt(pow(direction.x, 2) + pow(direction.y, 2));
			float radius = shockwaveComp->getRadius();
			if (distance > radius) return;
			float force = shockwaveComp->getForce() * (1 - distance / radius);
			Comp_Movement* otherMoveComp = other->getComponent<Comp_Movement>(ComponentType::Movement);
			otherMoveComp->accelerate(force * direction / distance);
			break;
		}
	}
}