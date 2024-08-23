#include "Sys_BulletSpawner.h"
#include "SysManager.h"

Sys_BulletSpawner::Sys_BulletSpawner(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_BulletSpawner::~Sys_BulletSpawner()
{
	unsubscribeFromChannels();
	m_bullets.clear();
}

void Sys_BulletSpawner::start()
{
}

void Sys_BulletSpawner::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Movable);
	m_requirements.set((unsigned int)CompType::Collision);
	m_requirements.set((unsigned int)CompType::Sprite);
	m_requirements.set((unsigned int)CompType::Bullet);
}

void Sys_BulletSpawner::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Shoot, this);
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_BulletSpawner::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Shoot, this);
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_BulletSpawner::update(const float& deltaTime)
{
}

void Sys_BulletSpawner::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_BulletSpawner::debugOverlay(WindowManager* windowManager)
{
}

void Sys_BulletSpawner::notify(const Message& msg)
{
	switch ((ActorMessageType)msg.m_type)
	{
		case ActorMessageType::Shoot:
			shoot(msg.m_sender, sf::Vector2f(msg.m_xy.x, msg.m_xy.y));
			break;
		case ActorMessageType::Collision:
			m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
			break;
	}
}

bool Sys_BulletSpawner::addBullet(Actor* bullet)
{
	if (!bullet->getComponent<Comp_Bullet>(CompType::Bullet)) return false;
	m_bullets.push_back(bullet);
	return true;
}

void Sys_BulletSpawner::shoot(const ActorId& actorId, sf::Vector2f direction)
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

void Sys_BulletSpawner::incrementBullet()
{
	m_currentBullet = ++m_currentBullet % m_bullets.size();
}
