#include "Sys_BulletControl.h"
#include "SysManager.h"

Sys_BulletControl::Sys_BulletControl(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_BulletControl::~Sys_BulletControl()
{
	unsubscribeFromChannels();
	m_bullets.clear();
}

void Sys_BulletControl::start()
{
}

void Sys_BulletControl::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Movable);
	m_requirements.set((unsigned int)CompType::Collision);
	m_requirements.set((unsigned int)CompType::Sprite);
	m_requirements.set((unsigned int)CompType::Bullet);
}

void Sys_BulletControl::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Shoot, this);
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_BulletControl::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Shoot, this);
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_BulletControl::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		Actor* bullet = actorManager->getActor(id);
		// no need to update bullet movement as their velocity is constant and set up on creation
		Comp_Collision* colComp = bullet->getComponent<Comp_Collision>(CompType::Collision);
		sf::FloatRect bulletAABB = colComp->getAABB();
		// check if bullet is out of bounds
		if (bulletAABB.top + bulletAABB.height < 0 || bulletAABB.top > m_viewSpace.getSize().y)
			m_systemManager->addEvent(bullet->getId(), (EventId)ActorEventType::Despawned);
	}
}

void Sys_BulletControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_BulletControl::debugOverlay(WindowManager* windowManager)
{
}

void Sys_BulletControl::notify(const Message& msg)
{
	switch ((ActorMessageType)msg.m_type)
	{
		case ActorMessageType::Shoot:
			shoot(msg.m_sender, sf::Vector2f(msg.m_xy.x, msg.m_xy.y));
			break;
		case ActorMessageType::Collision:
			if (hasActor(msg.m_receiver))
				m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
			break;
	}
}

void Sys_BulletControl::setViewSpace(sf::FloatRect viewSpace)
{
	m_viewSpace = viewSpace;
}

bool Sys_BulletControl::addBullet(Actor* bullet)
{
	if (!bullet->getComponent<Comp_Bullet>(CompType::Bullet)) return false;
	m_bullets.push_back(bullet);
	return true;
}

void Sys_BulletControl::shoot(const ActorId& actorId, sf::Vector2f direction)
{
	if (m_bullets.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	Actor* shooter = actorManager->getActor(actorId);
	Comp_Position* shooterPos = shooter->getComponent<Comp_Position>(CompType::Position);
	Comp_Collision* shooterCol = shooter->getComponent<Comp_Collision>(CompType::Collision);
	Comp_Player* playerComp = shooter->getComponent<Comp_Player>(CompType::Player);

	Actor* bullet = m_bullets[m_currentBullet];
	Comp_Position* bulletPos = bullet->getComponent<Comp_Position>(CompType::Position);
	Comp_Collision* bulletCol = bullet->getComponent<Comp_Collision>(CompType::Collision);
	bulletPos->setPosition(shooterPos->getPosition() +
		direction * (bulletCol->getAABB().getSize().y + shooterCol->getAABB().getSize().y) / 2.f);
	Comp_Movable* bulletMove = bullet->getComponent<Comp_Movable>(CompType::Movable);
	if (playerComp)
		bulletMove->setVelocity(direction * 1000.f);
	else
		bulletMove->setVelocity(direction * 750.f);
	Comp_Sprite* bulletSprite = bullet->getComponent<Comp_Sprite>(CompType::Sprite);
	bulletSprite->setPosition(bulletPos->getPosition());
	actorManager->enableActor(bullet->getId());
	incrementBullet();
}

void Sys_BulletControl::incrementBullet()
{
	m_currentBullet = ++m_currentBullet % m_bullets.size();
}
