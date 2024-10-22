#include "Sys_PlayerControl.h"
#include "SysManager.h"
#include "ActorManager.h"
#include "LevelManager.h"
#include "WindowManager.h"
#include <SFML/System/Vector2.hpp>

Sys_PlayerControl::Sys_PlayerControl(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_PlayerControl::~Sys_PlayerControl()
{
	onDestroy();
}

void Sys_PlayerControl::start()
{
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		Actor* player = actorManager->getActor(id);
		auto posComp = player->getComponent<Comp_Position>(ComponentType::Position);
		auto targetComp = player->getComponent<Comp_Target>(ComponentType::Target);
		targetComp->setTarget(posComp->getPosition());
	}
}

void Sys_PlayerControl::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Control);
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Movement);
	req.set((unsigned int)ComponentType::Target);
	req.set((unsigned int)ComponentType::Player);
	req.set((unsigned int)ComponentType::Health);
	m_requirements.emplace_back(req);
}

void Sys_PlayerControl::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_PlayerControl::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_PlayerControl::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
	{
		Actor* player = m_systemManager->getActorManager()->getActor(id);
		// clamp the player target to the view space
		auto targetComp = player->getComponent<Comp_Target>(ComponentType::Target);
		float lowerBound = m_levelManager->getViewSpace().getPosition().x;
		float upperBound = m_levelManager->getViewSpace().getPosition().x + m_levelManager->getViewSpace().getSize().x;
		targetComp->setTarget(sf::Vector2f(targetComp->getTarget().x < lowerBound ? lowerBound : (targetComp->getTarget().x > upperBound ? upperBound : targetComp->getTarget().x), targetComp->getTarget().y));
		
		auto controlComp = player->getComponent<Comp_Control>(ComponentType::Control);
		auto posComp = player->getComponent<Comp_Position>(ComponentType::Position);
		sf::Vector2f direction = targetComp->getTarget() - posComp->getPosition();

		auto moveComp = player->getComponent<Comp_Movement>(ComponentType::Movement);
		controlComp->setMovementInput(direction);
		moveComp->accelerate(controlComp->getMovementInput() * controlComp->getMaxAcceleration());

		// check if player is out of bounds
		auto colComp = player->getComponent<Comp_Collision>(ComponentType::Collision);
		sf::FloatRect playerAABB = colComp->getAABB();

		float resolve = 0;
		if (playerAABB.left < 0)
			resolve = -playerAABB.left;
		else if (playerAABB.left + playerAABB.width > m_levelManager->getViewSpace().getSize().x)
			resolve = -(playerAABB.left + playerAABB.width - m_levelManager->getViewSpace().getSize().x);
		if (resolve != 0)
		{
			auto posComp = player->getComponent<Comp_Position>(ComponentType::Position);
			auto moveComp = player->getComponent<Comp_Movement>(ComponentType::Movement);
			posComp->move(resolve, 0);
			moveComp->setAcceleration(0, moveComp->getAcceleration().y);
			moveComp->setVelocity(0, moveComp->getVelocity().y);
			moveComp->setCollidingOnX(true);
		}
	}
}

void Sys_PlayerControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
		m_systemManager->getActorManager()->disableActor(actorId);
		break;
	case ActorEventType::Shoot:
	{
		const int bulletId = m_levelManager->getPlayerBulletIds()[m_playerBulletIndex++ % m_levelManager->getPlayerBulletIds().size()];
		ActorManager* actorManager = m_systemManager->getActorManager();
		actorManager->enableActor(bulletId);
		sf::Vector2f shootDirection(0, -1);
		Actor* bullet = actorManager->getActor(bulletId);
		Actor* shooter = actorManager->getActor(actorId);
		auto shooterPos = shooter->getComponent<Comp_Position>(ComponentType::Position);
		auto shooterCol = shooter->getComponent<Comp_Collision>(ComponentType::Collision);
		// set bullet just above player
		auto bulletPos = bullet->getComponent<Comp_Position>(ComponentType::Position);
		auto bulletCol = bullet->getComponent<Comp_Collision>(ComponentType::Collision);
		bulletPos->setPosition(shooterPos->getPosition() +
			shootDirection * (bulletCol->getAABB().getSize().y / 2 + shooterCol->getAABB().getSize().y / 2.f));
		auto bulletMove = bullet->getComponent<Comp_Movement>(ComponentType::Movement);
		auto bulletComp = bullet->getComponent<Comp_Bullet>(ComponentType::Bullet);
		bulletMove->setVelocity(shootDirection * bulletComp->getBulletSpeed());
		// knock-back
		float knockback = 1000000;
		auto moveComp = actorManager->getActor(actorId)->getComponent<Comp_Movement>(ComponentType::Movement);
		moveComp->accelerate(sf::Vector2f(0, -knockback * shootDirection.y));
		break;
	}
	}
}

void Sys_PlayerControl::debugOverlay(WindowManager* windowManager)
{
	ActorManager* actorManager = m_systemManager->getActorManager();
	sf::RenderWindow* window = windowManager->getRenderWindow();
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = actorManager->getActor(actorId);
		auto targetComp = actor->getComponent<Comp_Target>(ComponentType::Target);
		sf::CircleShape target(2.5f);
		target.setOrigin(target.getRadius(), target.getRadius());
		target.setFillColor(sf::Color::Red);
		target.setPosition(targetComp->getTarget());
		window->draw(target);
	}
}

void Sys_PlayerControl::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	ActorMessageType msgType = (ActorMessageType)msg.m_type;
	switch (msgType)
	{
		case ActorMessageType::Collision:
		{
			ActorManager* actorManager = m_systemManager->getActorManager();
			Actor* actor = actorManager->getActor(msg.m_receiver);
			Actor* other = actorManager->getActor(msg.m_sender);
			if (other->getTag() == "bullet_invader")
			{
				auto playerHealth = actor->getComponent<Comp_Health>(ComponentType::Health);
				m_levelManager->setPlayerLives(playerHealth->takeDamage());
				if (m_levelManager->getPlayerLives() <= 0)
					m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
				else
				{
					Message msg((MessageType)ActorMessageType::Damage);
					msg.m_receiver = actor->getId();
					m_systemManager->getMessageHandler()->dispatch(msg);
				}
			}
			break;
		}
	}
}

void Sys_PlayerControl::setLevelManager(LevelManager* levelManager)
{
	m_levelManager = levelManager;
}
