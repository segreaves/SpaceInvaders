#include "Sys_InvaderControl.h"
#include "SysManager.h"
#include "WindowManager.h"
#include "LevelManager.h"

Sys_InvaderControl::Sys_InvaderControl(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_InvaderControl::~Sys_InvaderControl()
{
	unsubscribeFromChannels();
}

void Sys_InvaderControl::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Collision);
	req.set((unsigned int)ComponentType::Movement);
	req.set((unsigned int)ComponentType::Target);
	req.set((unsigned int)ComponentType::Control);
	req.set((unsigned int)ComponentType::Invader);
	req.set((unsigned int)ComponentType::SpriteSheet);
	m_requirements.emplace_back(req);
}

void Sys_InvaderControl::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_InvaderControl::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_InvaderControl::start()
{
	srand(time(nullptr));
	m_movingRight = rand() % 2;

	m_currentInvaderSpeed = m_levelManager->getInvaderBaseSpeed() + (m_levelManager->getLevel() - 1) * m_levelManager->getLevelSpeedIncrease();

	for (auto& id : m_actorIds)
	{
		Actor* invader = m_systemManager->getActorManager()->getActor(id);
		auto posComp = invader->getComponent<Comp_Position>(ComponentType::Position);
		auto invComp = invader->getComponent<Comp_Invader>(ComponentType::Invader);
		posComp->setPosition(invComp->getSpawnPosition());
		auto targetComp = invader->getComponent<Comp_Target>(ComponentType::Target);
		targetComp->setTarget(invComp->getSpawnPosition());
		auto moveComp = invader->getComponent<Comp_Movement>(ComponentType::Movement);
		moveComp->setVelocity(sf::Vector2f(0, 0));
		auto controlComp = invader->getComponent<Comp_Control>(ComponentType::Control);
		controlComp->setMaxSpeed(m_currentInvaderSpeed);
		auto spriteComp = invader->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		spriteComp->setFPS(spriteComp->getDefaultFPS() * m_currentInvaderSpeed / m_levelManager->getInvaderBaseSpeed());
	}
	selectTrackedInvaders();
}

void Sys_InvaderControl::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		Actor* invader = actorManager->getActor(id);
		auto posComp = invader->getComponent<Comp_Position>(ComponentType::Position);
		auto invComp = invader->getComponent<Comp_Invader>(ComponentType::Invader);
		auto moveComp = invader->getComponent<Comp_Movement>(ComponentType::Movement);
		auto targetComp = invader->getComponent<Comp_Target>(ComponentType::Target);
		auto controlComp = invader->getComponent<Comp_Control>(ComponentType::Control);
		auto colComp = invader->getComponent<Comp_Collision>(ComponentType::Collision);

		handleMovement(deltaTime, id, posComp, moveComp, controlComp, targetComp, colComp);
		handleShooting(deltaTime, id, invComp);
	}
}

void Sys_InvaderControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
		case ActorEventType::Despawned:
		{
			m_systemManager->getActorManager()->disableActor(actorId);
			auto posComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Position>(ComponentType::Position);
			instantiateShockwave(posComp->getPosition());
			if (!m_actorIds.empty())
			{
				selectTrackedInvaders();
				increaseInvaderSpeed();
			}
			break;
		}
		case ActorEventType::Shoot:
		{
			const int bulletId = m_levelManager->getInvaderBulletIds()[m_invaderBulletIndex++ % m_levelManager->getInvaderBulletIds().size()];
			ActorManager* actorManager = m_systemManager->getActorManager();
			actorManager->enableActor(bulletId);
			sf::Vector2f shootDirection(0, 1);
			Actor* bullet = actorManager->getActor(bulletId);
			Actor* shooter = actorManager->getActor(actorId);
			auto shooterPos = shooter->getComponent<Comp_Position>(ComponentType::Position);
			auto shooterCol = shooter->getComponent<Comp_Collision>(ComponentType::Collision);
			// set bullet just below invader
			auto bulletPos = bullet->getComponent<Comp_Position>(ComponentType::Position);
			auto bulletCol = bullet->getComponent<Comp_Collision>(ComponentType::Collision);
			bulletPos->setPosition(shooterPos->getPosition() +
				shootDirection * (bulletCol->getAABB().getSize().y / 2 + shooterCol->getAABB().getSize().y / 2.f));
			auto bulletMove = bullet->getComponent<Comp_Movement>(ComponentType::Movement);
			auto bulletComp = bullet->getComponent<Comp_Bullet>(ComponentType::Bullet);
			bulletMove->setVelocity(shootDirection * bulletComp->getBulletSpeed());
			// knock-back
			float knockback = 50000;
			auto moveComp = actorManager->getActor(actorId)->getComponent<Comp_Movement>(ComponentType::Movement);
			moveComp->accelerate(sf::Vector2f(0, -knockback * shootDirection.y));
			break;
		}
	}
}

void Sys_InvaderControl::debugOverlay(WindowManager* windowManager)
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

void Sys_InvaderControl::notify(const Message& msg)
{
	ActorMessageType msgType = (ActorMessageType)msg.m_type;
	if (!hasActor(msg.m_receiver)) return;
	switch (msgType)
	{
		case ActorMessageType::Collision:
			Actor* actor = m_systemManager->getActorManager()->getActor(msg.m_receiver);
			Actor* other = m_systemManager->getActorManager()->getActor(msg.m_sender);
			if (other->getTag() == "player")// invader collided with player, player loses
			{
				std::cout << "Player invaded!" << std::endl;
				m_systemManager->addEvent(msg.m_sender, (EventId)ActorEventType::Despawned);
				return;
			}
			else if (other->getTag() == "bullet_player")// invader collided with player bullet, invader dies
			{
				m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
			}
			break;
	}
}

void Sys_InvaderControl::setLevelManager(LevelManager* levelManager)
{
	m_levelManager = levelManager;
}

void Sys_InvaderControl::selectTrackedInvaders()
{
	if (getActorCount() == 0) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	float minX = m_levelManager->getViewSpace().getSize().x;
	float maxX = 0.f;
	m_leftInvader = -1;
	m_rightInvader = -1;
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = actorManager->getActor(actorId);
		auto posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		auto colComp = actor->getComponent<Comp_Collision>(ComponentType::Collision);
		if (posComp->getPosition().x - colComp->getAABB().width / 2 < minX)
		{
			m_leftInvader = actorId;
			minX = posComp->getPosition().x - colComp->getAABB().width;
		}
		if (posComp->getPosition().x + colComp->getAABB().width / 2 > maxX)
		{
			m_rightInvader = actorId;
			maxX = posComp->getPosition().x + colComp->getAABB().width;
		}
	}
#ifdef DEBUG
	auto leftSprite = actorManager->getActor(m_leftInvader)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
	if (leftSprite) leftSprite->getSpriteSheet()->setSpriteColor(sf::Color::Red);
	auto rightSprite = actorManager->getActor(m_rightInvader)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
	if (rightSprite) rightSprite->getSpriteSheet()->setSpriteColor(sf::Color::Red);
#endif
}

void Sys_InvaderControl::instantiateShockwave(sf::Vector2f position)
{
	const int shockwaveId = m_levelManager->getShockwaveIds()[m_shockwaveIndex++ % m_levelManager->getShockwaveIds().size()];
	auto actorManager = m_systemManager->getActorManager();
	auto shockwavePos = actorManager->getActor(shockwaveId)->getComponent<Comp_Position>(ComponentType::Position);
	auto shockwaveCol = actorManager->getActor(shockwaveId)->getComponent<Comp_Collision>(ComponentType::Collision);
	shockwavePos->setPosition(position);
	auto shockwaveComp = actorManager->getActor(shockwaveId)->getComponent<Comp_Shockwave>(ComponentType::Shockwave);
	shockwaveComp->setRadius(0);
	shockwaveComp->resetTime();
	actorManager->enableActor(shockwaveId);
}

void Sys_InvaderControl::increaseInvaderSpeed()
{
	m_currentInvaderSpeed += m_levelManager->getDefeatSpeedIncrease();
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& invaderId : m_actorIds)
	{
		Actor* invader = actorManager->getActor(invaderId);
		auto moveComp = invader->getComponent<Comp_Movement>(ComponentType::Movement);
		auto controlComp = invader->getComponent<Comp_Control>(ComponentType::Control);
		auto spriteComp = invader->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
		controlComp->setMaxSpeed(m_currentInvaderSpeed);
		spriteComp->setFPS(spriteComp->getDefaultFPS() * m_currentInvaderSpeed / m_levelManager->getInvaderBaseSpeed());
	}
}

void Sys_InvaderControl::handleMovement(const float& deltaTime, const ActorId& id, std::shared_ptr<Comp_Position> posComp, std::shared_ptr<Comp_Movement> moveComp, std::shared_ptr<Comp_Control> controlComp, std::shared_ptr<Comp_Target> targetComp, std::shared_ptr<Comp_Collision> colComp)
{
	// invader movement
	targetComp->setTarget(targetComp->getTarget() + sf::Vector2f(m_movingRight ? controlComp->getMaxSpeed() : -controlComp->getMaxSpeed(), 0) * deltaTime);
	sf::Vector2f direction = targetComp->getTarget() - posComp->getPosition();
	controlComp->setMovementInput(direction / m_maxTargetDistance);
	moveComp->accelerate(controlComp->getMovementDirection() * controlComp->getMaxAcceleration());

	// check if invader is out of bounds. This is done only for the tracked invaders
	if (id != m_leftInvader && id != m_rightInvader) return;
	sf::FloatRect invaderAABB = colComp->getAABB();
	bool boundsLeft = targetComp->getTarget().x - invaderAABB.width / 2.f < m_bounds;
	bool boundsRight = targetComp->getTarget().x + invaderAABB.width / 2.f > m_levelManager->getViewSpace().getSize().x - m_bounds;
	if (boundsLeft || boundsRight)
	{
		// drop invaders
		sf::Vector2f resolve(
			boundsLeft ? (m_bounds + invaderAABB.width / 2.f - targetComp->getTarget().x) : (m_levelManager->getViewSpace().getSize().x - m_bounds - invaderAABB.width / 2.f - targetComp->getTarget().x),
			m_dropDistance
		);
		// changer movement direction
		m_movingRight = boundsLeft ? true : false;
		// set new movement target for all invaders
		for (auto& actorId : m_actorIds)
		{
			Actor* invader = m_systemManager->getActorManager()->getActor(actorId);
			auto targetComp = invader->getComponent<Comp_Target>(ComponentType::Target);
			targetComp->setTarget(targetComp->getTarget() + resolve);
		}
	}
}

void Sys_InvaderControl::handleShooting(const float& deltaTime, const ActorId& id, std::shared_ptr<Comp_Invader> invComp)
{
	if (invComp->canShoot())
	{
		if (invComp->getTimeToShoot() > 0)
			invComp->decreaseTimeToShoot(deltaTime);
		else
		{
			m_systemManager->addEvent(id, (EventId)ActorEventType::Shoot);
			invComp->setCanShoot(false);
		}
	}
	else
	{
		// get random time to shoot
		srand(time(nullptr) + id);
		float random = rand() % 100;
		float waitTime = 1.f + 30.f * random / 100.f;
		invComp->setTimeToShoot(waitTime);
		invComp->setCanShoot(true);
	}
}