#include "Sys_Combat.h"
#include "SysManager.h"

Sys_Combat::Sys_Combat(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_Combat::~Sys_Combat()
{
	unsubscribeFromChannels();
	m_bullets.clear();
}

void Sys_Combat::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Movable);
	m_requirements.set((unsigned int)CompType::Collision);
	m_requirements.set((unsigned int)CompType::Sprite);
}

void Sys_Combat::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Shoot, this);
}

void Sys_Combat::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Shoot, this);
}

void Sys_Combat::update(const float& deltaTime)
{
}

void Sys_Combat::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Combat::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Combat::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	switch ((ActorMessageType)msg.m_type)
	{
		case ActorMessageType::Shoot:
		shoot(msg.m_receiver, sf::Vector2f(msg.m_xy.x, msg.m_xy.y));
		break;
	}
}

bool Sys_Combat::addBullet(Actor* bullet)
{
	if (!bullet->getComponent<Comp_Bullet>(CompType::Bullet)) return false;
	m_bullets.push_back(bullet);
	return true;
}

void Sys_Combat::shoot(const ActorId& actorId, sf::Vector2f direction)
{
	if (m_bullets.empty()) return;
	Actor* shooter = m_systemManager->getActorManager()->getActor(actorId);
	Comp_Position* shooterPos = shooter->getComponent<Comp_Position>(CompType::Position);
	Comp_Collision* shooterCol = shooter->getComponent<Comp_Collision>(CompType::Collision);
	Comp_Player* playerComp = shooter->getComponent<Comp_Player>(CompType::Player);

	Actor* bullet = m_bullets[m_currentBullet++];
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
	m_systemManager->getActorManager()->enableActor(bullet->getId());
}
