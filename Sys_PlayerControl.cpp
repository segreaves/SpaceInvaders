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
	m_font.loadFromFile(Utils::getWorkingDirectory() + "assets/fonts/game_over.ttf");
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
		auto moveComp = player->getComponent<Comp_Movement>(ComponentType::Movement);
		sf::Vector2f direction = targetComp->getTarget() - posComp->getPosition();
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
		const int bulletId = m_systemManager->getLevelManager()->getPlayerBulletIds()[m_playerBulletIndex];
		m_playerBulletIndex = (m_playerBulletIndex + 1) % m_systemManager->getLevelManager()->getPlayerBulletIds().size();
		ActorManager* actorManager = m_systemManager->getActorManager();
		// enable bullet
		actorManager->enableActor(bulletId);
		// message bullet
		Message msg((MessageType)ActorMessageType::Shoot);
		msg.m_sender = actorId;
		msg.m_receiver = bulletId;
		m_systemManager->getMessageHandler()->dispatch(msg);
		// knock-back
		float knockback = 250000;
		const auto& moveComp = actorManager->getActor(actorId)->getComponent<Comp_Movement>(ComponentType::Movement);
		moveComp->accelerate(sf::Vector2f(0, -knockback * -1));
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
	// show bullet count text
	m_bulletCountText.setFont(m_font);
	m_bulletCountText.setCharacterSize(70);
	m_bulletCountText.setFillColor(sf::Color::White);
	m_bulletCountText.setPosition(10, 10);
	m_bulletCountText.setString("Bullets: " + std::to_string(m_playerBulletIndex));
	window->draw(m_bulletCountText);
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
	msg.m_xy = XY(100.f, 1.f);
	m_systemManager->getMessageHandler()->dispatch(msg);
	// enable player explosion particle system
	ActorId explosionId = m_systemManager->getLevelManager()->getPlayerExplosionId();
	const auto& explosionPos = m_systemManager->getActorManager()->getActor(explosionId)->getComponent<Comp_Position>(ComponentType::Position);
	const auto& playerPos = m_systemManager->getActorManager()->getActor(id)->getComponent<Comp_Position>(ComponentType::Position);
	explosionPos->setPosition(playerPos->getPosition());
	const auto& particlesComp = m_systemManager->getActorManager()->getActor(explosionId)->getComponent<Comp_Particles>(ComponentType::Particles);
	particlesComp->getParticleSystem()->initialize();
	particlesComp->getParticleSystem()->setEmitterPosition(explosionPos->getPosition());
	particlesComp->getParticleSystem()->emitParticles();
	m_systemManager->getActorManager()->enableActor(explosionId);
	m_systemManager->getLevelManager()->onPlayerDefeated();
}
