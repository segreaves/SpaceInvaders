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
	m_requirements.set((unsigned int)ComponentType::Position);
	m_requirements.set((unsigned int)ComponentType::Collision);
	m_requirements.set((unsigned int)ComponentType::Shockwave);
}

void Sys_Shockwave::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Explode, this);
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_Shockwave::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Explode, this);
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_Shockwave::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	for (auto& id : m_actorIds)
		m_systemManager->getActorManager()->disableActor(id);
}

void Sys_Shockwave::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
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
	case ActorMessageType::Explode:
	{
		Comp_Position* posComp = m_systemManager->getActorManager()->getActor(msg.m_receiver)->getComponent<Comp_Position>(ComponentType::Position);
		Comp_Collision* colComp = m_systemManager->getActorManager()->getActor(msg.m_receiver)->getComponent<Comp_Collision>(ComponentType::Collision);
		Comp_Shockwave* shockwaveComp = m_systemManager->getActorManager()->getActor(msg.m_receiver)->getComponent<Comp_Shockwave>(ComponentType::Shockwave);
		posComp->setPosition(sf::Vector2f(msg.m_xy.x, msg.m_xy.y));
		colComp->setPosition(posComp->getPosition());
		break;
	}
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
		//float radius = shockwaveComp->getRadius();
		float radius = shockwaveColComp->getAABB().width / 2.f;
		if (distance > radius) return;
		// force = baseForce * (1 - distance / radius)
		float force = shockwaveComp->getForce() * (1 - distance / radius);
		Comp_Movement* otherMoveComp = other->getComponent<Comp_Movement>(ComponentType::Movement);
		otherMoveComp->accelerate(force * direction / distance);
		break;
	}
	}
}