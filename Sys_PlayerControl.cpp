#include "Sys_PlayerControl.h"
#include "SysManager.h"
#include "ActorManager.h"
#include "LevelManager.h"
#include "WindowManager.h"
#include <SFML/System/Vector2.hpp>
#include "SoundType.h"

Sys_PlayerControl::Sys_PlayerControl(SysManager* systemManager) :
	Sys(systemManager),
	m_playerBulletIndex(0),
	m_font(),
	m_bulletCountText(m_font)
{
	onCreate();
	bool isLoaded = m_font.openFromFile(Utils::getAssetsDirectory() + "fonts/game_over.ttf");

	if (!isLoaded) {
		// Handle the error, e.g., log an error message or throw an exception
		std::cerr << "Failed to load font from file." << std::endl;
	}
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
		const auto& player = actorManager->getActor(id);
		const auto& posComp = player->getComponent<Comp_Position>(ComponentType::Position);
		const auto& targetComp = player->getComponent<Comp_Target>(ComponentType::Target);
		targetComp->setTarget(targetComp->getTarget().x, posComp->getPosition().y);
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
		const auto& player = m_systemManager->getActorManager()->getActor(id);

		// get the desired move direction
		const auto& targetComp = player->getComponent<Comp_Target>(ComponentType::Target);
		const auto& posComp = player->getComponent<Comp_Position>(ComponentType::Position);
		// set the player location to the mouse position (target)
		posComp->setPosition(targetComp->getTarget().x, posComp->getPosition().y);

		// check if player is out of bounds
		const auto& colComp = player->getComponent<Comp_Collision>(ComponentType::Collision);
		sf::FloatRect playerAABB = colComp->getAABB();

		float resolve = 0;
		if (playerAABB.position.x < 0)
			resolve = -playerAABB.position.x;
		else if (playerAABB.position.x + playerAABB.size.x > levelManager->getViewSpace().size.x)
			resolve = -(playerAABB.position.x + playerAABB.size.x - levelManager->getViewSpace().size.x);
		if (resolve != 0)
		{
			posComp->move(resolve, 0);
			const auto& moveComp = player->getComponent<Comp_Movement>(ComponentType::Movement);
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
	case ActorEventType::Spawned:
		// No action needed for Spawned event
		break;
	case ActorEventType::Invaded:
		// No action needed for Invaded event
		break;
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
		const auto& moveComp = actorManager->getActor(actorId)->getComponent<Comp_Movement>(ComponentType::Movement);
		moveComp->accelerate(sf::Vector2f(0, -m_knockback * -1));
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
		target.setOrigin({target.getRadius(), target.getRadius()});
		target.setFillColor(sf::Color::Red);
		target.setPosition(targetComp->getTarget());
		window->draw(target);
	}
	// show bullet count text
	m_bulletCountText.setFont(m_font);
	m_bulletCountText.setCharacterSize(70);
	m_bulletCountText.setFillColor(sf::Color::White);
	m_bulletCountText.setPosition({10, 10});
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
