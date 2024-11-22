#include "Sys_BulletControl.h"
#include "SysManager.h"
#include "LevelManager.h"
#include "WindowManager.h"
#include "SoundType.h"

Sys_BulletControl::Sys_BulletControl(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_BulletControl::~Sys_BulletControl()
{
	onDestroy();
}

void Sys_BulletControl::start()
{
}

void Sys_BulletControl::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Movement);
	req.set((unsigned int)ComponentType::Collision);
	req.set((unsigned int)ComponentType::Bullet);
	req.set((unsigned int)ComponentType::SpriteSheet);
	m_requirements.emplace_back(req);
}

void Sys_BulletControl::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_BulletControl::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_BulletControl::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		const auto& bullet = actorManager->getActor(id);
		const auto& colComp = bullet->getComponent<Comp_Collision>(ComponentType::Collision);
		const auto& posComp = bullet->getComponent<Comp_Position>(ComponentType::Position);
		const auto& bulletComp = bullet->getComponent<Comp_Bullet>(ComponentType::Bullet);
		const auto& spriteComp = bullet->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		// update bullet frame
		bulletComp->incrementBulletFrameTime(deltaTime);
		if (bulletComp->getBulletFrameTime() >= bulletComp->getBulletFrameDuration())
		{
			bulletComp->resetBulletFrameTime();
			spriteComp->frameStep();
		}
		// update collider before checking if it has gone out of bounds
		colComp->setPosition(posComp->getPosition());
		sf::FloatRect bulletAABB = colComp->getAABB();
		// check if bullet is out of bounds
		if (bulletAABB.top + bulletAABB.height < 0 || bulletAABB.top > m_systemManager->getLevelManager()->getViewSpace().getSize().y)
			m_systemManager->addEvent(bullet->getId(), (EventId)ActorEventType::Despawned);
#ifdef DEBUG
		sf::RectangleShape rectTip(sf::Vector2f(4.f, 4.f));
		rectTip.setPosition(posComp->getPosition());
		rectTip.setOrigin(rectTip.getSize().x / 2, rectTip.getSize().y / 2);
		sf::Color color = sf::Color::White;
		color.a = 100;
		rectTip.setFillColor(color);
		m_bulletTips.emplace_back(rectTip);
#endif
	}
}

void Sys_BulletControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Spawned:
	{
		const auto& actor = m_systemManager->getActorManager()->getActor(actorId);
		const auto& bulletComp = actor->getComponent<Comp_Bullet>(ComponentType::Bullet);
		const auto& bulletMove = actor->getComponent<Comp_Movement>(ComponentType::Movement);
		bulletComp->resetBulletFrameTime();
		sf::Vector2f direction(0, bulletComp->getDirection());
		bulletMove->setVelocity(direction * bulletComp->getBulletSpeed());
		break;
	}
	case ActorEventType::Despawned:
		m_systemManager->getActorManager()->disableActor(actorId);
		break;
	}
}

void Sys_BulletControl::debugOverlay(WindowManager* windowManager)
{
	for (auto& rect : m_bulletTips)
		windowManager->getRenderWindow()->draw(rect);
	m_bulletTips.clear();
}

void Sys_BulletControl::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	switch ((ActorMessageType)msg.m_type)
	{
	case ActorMessageType::Collision:
		onBulletDestroyed(msg.m_receiver);
		m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
		break;
	}
}

void Sys_BulletControl::onBulletDestroyed(ActorId id)
{
	// play bullet hit sound
	Message msg((MessageType)ActorMessageType::Sound);
	msg.m_sender = id;
	msg.m_receiver = id;
	msg.m_int = (int)SoundType::BulletExplode;
	m_systemManager->getMessageHandler()->dispatch(msg);
}
