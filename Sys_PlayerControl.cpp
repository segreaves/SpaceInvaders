#include "Sys_PlayerControl.h"
#include "SysManager.h"
#include "ActorManager.h"
#include "LevelManager.h"
#include "WindowManager.h"
#include <SFML/System/Vector2.hpp>
#include "SoundType.h"

Sys_PlayerControl::Sys_PlayerControl(SysManager* systemManager) :
	Sys(systemManager),
	m_playerBulletIndex(0)
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
		auto player = actorManager->getActor(id);
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
	m_requirements.emplace_back(req);
}

void Sys_PlayerControl::subscribeToChannels()
{
}

void Sys_PlayerControl::unsubscribeFromChannels()
{
}

void Sys_PlayerControl::update(const float& deltaTime)
{
	LevelManager* levelManager = m_systemManager->getLevelManager();
	for (auto& id : m_actorIds)
	{
		auto player = m_systemManager->getActorManager()->getActor(id);
		// clamp the player target to the view space
		auto targetComp = player->getComponent<Comp_Target>(ComponentType::Target);
		float lowerBound = levelManager->getViewSpace().getPosition().x;
		float upperBound = levelManager->getViewSpace().getPosition().x + levelManager->getViewSpace().getSize().x;
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
		else if (playerAABB.left + playerAABB.width > levelManager->getViewSpace().getSize().x)
			resolve = -(playerAABB.left + playerAABB.width - levelManager->getViewSpace().getSize().x);
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
	case ActorEventType::Shoot:
	{
		const int bulletId = m_systemManager->getLevelManager()->getPlayerBulletIds()[m_playerBulletIndex++ % m_systemManager->getLevelManager()->getPlayerBulletIds().size()];
		ActorManager* actorManager = m_systemManager->getActorManager();
		actorManager->enableActor(bulletId);
		sf::Vector2f shootDirection(0, -1);
		auto bullet = actorManager->getActor(bulletId);
		auto shooter = actorManager->getActor(actorId);
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
		// play sound
		Message msg((MessageType)ActorMessageType::Sound);
		msg.m_sender = actorId;
		msg.m_receiver = actorId;
		msg.m_int = (int)SoundType::PlayerShoot;
		m_systemManager->getMessageHandler()->dispatch(msg);
		break;
	}
	case ActorEventType::Despawned:
		onPlayerDestroyed(actorId);
		break;
	}
}

void Sys_PlayerControl::debugOverlay(WindowManager* windowManager)
{
	ActorManager* actorManager = m_systemManager->getActorManager();
	sf::RenderWindow* window = windowManager->getRenderWindow();
	for (auto& actorId : m_actorIds)
	{
		auto actor = actorManager->getActor(actorId);
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
}

void Sys_PlayerControl::onPlayerDestroyed(ActorId id)
{
	// play explosion sound
	Message msg((MessageType)ActorMessageType::Sound);
	msg.m_sender = id;
	msg.m_receiver = id;
	msg.m_int = (int)SoundType::PlayerExplode;
	m_systemManager->getMessageHandler()->dispatch(msg);
	// enable player explosion particle system
	ActorId explosionId = m_systemManager->getLevelManager()->getPlayerExplosionId();
	auto explosionPos = m_systemManager->getActorManager()->getActor(explosionId)->getComponent<Comp_Position>(ComponentType::Position);
	auto playerPos = m_systemManager->getActorManager()->getActor(id)->getComponent<Comp_Position>(ComponentType::Position);
	explosionPos->setPosition(playerPos->getPosition());
	auto particlesComp = m_systemManager->getActorManager()->getActor(explosionId)->getComponent<Comp_Particles>(ComponentType::Particles);
	particlesComp->getParticleSystem()->initialize();
	particlesComp->getParticleSystem()->setEmitterPosition(explosionPos->getPosition());
	particlesComp->getParticleSystem()->emitParticles();
	m_systemManager->getActorManager()->enableActor(explosionId);
}
