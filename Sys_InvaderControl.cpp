#include "Sys_InvaderControl.h"
#include "SysManager.h"
#include "WindowManager.h"
#include "LevelManager.h"
#include "SoundType.h"

Sys_InvaderControl::Sys_InvaderControl(SysManager* systemManager) :
	Sys(systemManager),
	m_movingRight(false),
	m_gen(m_rd()),
	m_unifFloat(1.f, 30.f),
	m_unifInt(0, 1),
	m_aiSpeed(0),
	m_invaderBulletIndex(0),
	m_leftInvader(-1),
	m_rightInvader(-1),
	m_shockwaveIndex(0),
	m_loadTimer(1),
	m_beatDuration(0.5f),
	m_beatTimer(0),
	m_beatHigh(false)
{
	onCreate();
}

Sys_InvaderControl::~Sys_InvaderControl()
{
	onDestroy();
}

void Sys_InvaderControl::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Collision);
	req.set((unsigned int)ComponentType::Movement);
	req.set((unsigned int)ComponentType::Target);
	req.set((unsigned int)ComponentType::Invader);
	req.set((unsigned int)ComponentType::SpriteSheet);
	req.set((unsigned int)ComponentType::Spring);
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
	m_movingRight = m_unifInt(m_gen);

	auto levelManager = m_systemManager->getLevelManager();
	m_aiTarget = levelManager->getScreenCenter();
	setInvaderSpeed(levelManager->getLevelBaseSpeed());
	// set beat duration: 1 per sec at base speed, then decreases as speed increases
	m_beatDuration = m_systemManager->getLevelManager()->getLevelBaseSpeed() / m_systemManager->getLevelManager()->getInvaderBaseSpeed();
	m_beatTimer = 0;
	m_beatHigh = false;
	m_loadTimer = m_invaderLoadTime;
	// play start level sound
	Message msg((MessageType)ActorMessageType::Sound);
	msg.m_int = static_cast<int>(SoundType::InvaderSpawn);
	m_systemManager->getMessageHandler()->dispatch(msg);
}

void Sys_InvaderControl::loadInvader(const ActorId& id)
{
	const auto& invader = m_systemManager->getActorManager()->getActor(id);
	const auto& invComp = invader->getComponent<Comp_Invader>(ComponentType::Invader);
	const auto& posComp = invader->getComponent<Comp_Position>(ComponentType::Position);
	const auto& targetComp = invader->getComponent<Comp_Target>(ComponentType::Target);
	const auto& moveComp = invader->getComponent<Comp_Movement>(ComponentType::Movement);
	auto spriteSheetComp = invader->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
	spriteSheetComp->resetFrame();
	targetComp->setTarget(m_aiTarget + invComp->getSpawnOffset());
	posComp->setPosition(targetComp->getTarget());
	posComp->setAngle(0);
	moveComp->setVelocity(sf::Vector2f(0, 0));
	moveComp->setAngularVelocity(0);
	m_systemManager->getActorManager()->enableActor(id);
}

void Sys_InvaderControl::update(const float& deltaTime)
{
	// load invaders or update them
	if (m_invaderQueue.empty())
		handleInvaders(deltaTime);
	else 
		loadNextInvader(deltaTime);
}

bool Sys_InvaderControl::updateBeat(const float& deltaTime)
{
	// update beat timer
	m_beatTimer -= deltaTime;
	if (m_beatTimer > 0) return false;
	// reset timer
	m_beatTimer = m_beatDuration;
	// play sound
	Message msg((MessageType)ActorMessageType::Sound);
	msg.m_int = static_cast<int>(m_beatHigh ? SoundType::BeatHigh : SoundType::BeatLow);
	m_systemManager->getMessageHandler()->dispatch(msg);
	m_beatHigh = !m_beatHigh;
	return true;
}

void Sys_InvaderControl::loadNextInvader(const float& deltaTime)
{
	m_loadTimer -= deltaTime;
	if (m_loadTimer > 0) return;
	loadInvader(m_invaderQueue.front());
	m_invaderQueue.pop();
	m_loadTimer = m_invaderLoadTime;
	// if all invaders are loaded, select ones to track
	if (m_invaderQueue.empty())
		selectTrackedInvaders();
}

void Sys_InvaderControl::handleInvaders(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	handleAITargetMovement(deltaTime);
	bool beat = updateBeat(deltaTime);
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		const auto& invader = actorManager->getActor(id);
		const auto& invComp = invader->getComponent<Comp_Invader>(ComponentType::Invader);
		const auto& targetComp = invader->getComponent<Comp_Target>(ComponentType::Target);
		const auto& colComp = invader->getComponent<Comp_Collision>(ComponentType::Collision);
		const auto& spriteSheetComp = invader->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);

		// update the target position
		updateMoveTarget(targetComp, invComp);
		tryShooting(deltaTime, id, invComp);
		if (id == m_leftInvader || id == m_rightInvader)
			checkBounds(deltaTime, id, targetComp, colComp);
		if (beat)
			spriteSheetComp->frameStep();
	}
}

void Sys_InvaderControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
	{
		onInvaderDeath(actorId);
		if (!m_actorIds.empty())
		{
			selectTrackedInvaders();
			setInvaderSpeed(m_aiSpeed + m_systemManager->getLevelManager()->getKillSpeedIncrease());
		}
		break;
	}
	case ActorEventType::Shoot:
	{
		const int bulletId = m_systemManager->getLevelManager()->getInvaderBulletIds()[m_invaderBulletIndex];
		m_invaderBulletIndex = (m_invaderBulletIndex + 1) % m_systemManager->getLevelManager()->getPlayerBulletIds().size();
		ActorManager* actorManager = m_systemManager->getActorManager();
		// shooter components
		const auto& moveComp = actorManager->getActor(actorId)->getComponent<Comp_Movement>(ComponentType::Movement);
		// enable bullet
		actorManager->enableActor(bulletId);
		// message bullet
		Message msg((MessageType)ActorMessageType::Shoot);
		msg.m_sender = actorId;
		msg.m_receiver = bulletId;
		m_systemManager->getMessageHandler()->dispatch(msg);
		// knock-back
		float knockback = 50000;
		moveComp->accelerate(sf::Vector2f(0, -knockback * 1));
		break;
	}
	}
}

void Sys_InvaderControl::debugOverlay(WindowManager* windowManager)
{
	ActorManager* actorManager = m_systemManager->getActorManager();
	sf::RenderWindow* window = windowManager->getRenderWindow();
	for (const auto& actorId : m_actorIds)
	{
		const auto& actor = actorManager->getActor(actorId);
		const auto& targetComp = actor->getComponent<Comp_Target>(ComponentType::Target);
		sf::CircleShape target(2.5f);
		target.setOrigin(target.getRadius(), target.getRadius());
		target.setFillColor(sf::Color::Red);
		target.setPosition(targetComp->getTarget());
		window->draw(target);
	}
	sf::CircleShape aiTarget(5.f);
	aiTarget.setOrigin(aiTarget.getRadius(), aiTarget.getRadius());
	aiTarget.setFillColor(sf::Color::Yellow);
	aiTarget.setPosition(m_aiTarget);
	window->draw(aiTarget);
	// show bullet count text
	m_bulletCountText.setFont(m_font);
	m_bulletCountText.setCharacterSize(70);
	m_bulletCountText.setFillColor(sf::Color::White);
	m_bulletCountText.setPosition(10, 90);
	m_bulletCountText.setString("Bullets: " + std::to_string(m_invaderBulletIndex));
	window->draw(m_bulletCountText);
}

void Sys_InvaderControl::notify(const Message& msg)
{
	ActorMessageType msgType = (ActorMessageType)msg.m_type;
	if (!hasActor(msg.m_receiver)) return;
	switch (msgType)
	{
		case ActorMessageType::Collision:
		{
			auto actor = m_systemManager->getActorManager()->getActor(msg.m_receiver);
			auto other = m_systemManager->getActorManager()->getActor(msg.m_sender);
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
}

void Sys_InvaderControl::queueInvaders(std::vector<ActorId>& invaders)
{
	m_invaderQueue = std::queue<ActorId>(std::deque<ActorId>(invaders.begin(), invaders.end()));
}

/// <summary>
/// Select the leftmost and rightmost invaders to track their movement.
/// </summary>
void Sys_InvaderControl::selectTrackedInvaders()
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	float minX = m_systemManager->getLevelManager()->getViewSpace().getSize().x;
	float maxX = 0.f;
	m_leftInvader = -1;
	m_rightInvader = -1;
	for (auto& actorId : m_actorIds)
	{
		auto actor = actorManager->getActor(actorId);
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
#ifdef DEBUG
		actorManager->getActor(actorId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet)->getSpriteSheet()->setSpriteColor(APP_COLOR);
#endif
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
	auto numShockwaves = m_systemManager->getLevelManager()->getShockwaveIds().size();
	if (numShockwaves == 0) return;
	const int shockwaveId = m_systemManager->getLevelManager()->getShockwaveIds()[m_shockwaveIndex];
	m_shockwaveIndex = (m_shockwaveIndex + 1) % numShockwaves;
	auto actorManager = m_systemManager->getActorManager();
	auto shockwavePos = actorManager->getActor(shockwaveId)->getComponent<Comp_Position>(ComponentType::Position);
	shockwavePos->setPosition(position);
	auto shockwaveComp = actorManager->getActor(shockwaveId)->getComponent<Comp_Shockwave>(ComponentType::Shockwave);
	shockwaveComp->setRadius(0);
	shockwaveComp->resetTime();
	actorManager->enableActor(shockwaveId);
}

void Sys_InvaderControl::handleAITargetMovement(const float& deltaTime)
{
	m_aiTarget += sf::Vector2f(m_movingRight ? m_aiSpeed : -m_aiSpeed, 0) * deltaTime;
}

void Sys_InvaderControl::updateMoveTarget(std::shared_ptr<Comp_Target> targetComp, std::shared_ptr<Comp_Invader> invComp)
{
	targetComp->setTarget(m_aiTarget + invComp->getSpawnOffset());
}

void Sys_InvaderControl::checkBounds(const float& deltaTime, const ActorId& id, std::shared_ptr<Comp_Target> targetComp, std::shared_ptr<Comp_Collision> colComp)
{
	sf::FloatRect invaderAABB = colComp->getAABB();
	bool boundsLeft = targetComp->getTarget().x < m_bounds;
	bool boundsRight = targetComp->getTarget().x > m_systemManager->getLevelManager()->getViewSpace().getSize().x - m_bounds;
	if (boundsLeft || boundsRight)
	{
		// drop invaders
		sf::Vector2f resolve(
			boundsLeft ? (m_bounds - targetComp->getTarget().x) : (m_systemManager->getLevelManager()->getViewSpace().getSize().x - m_bounds - targetComp->getTarget().x),
			m_dropDistance
		);
		// change movement direction
		m_movingRight = boundsLeft ? true : false;
		m_aiTarget += resolve;
	}
}

void Sys_InvaderControl::tryShooting(const float& deltaTime, const ActorId& id, std::shared_ptr<Comp_Invader> invComp)
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
		invComp->setTimeToShoot(m_unifFloat(m_gen));
		invComp->setCanShoot(true);
	}
}

void Sys_InvaderControl::onInvaderDeath(const ActorId& id)
{
	// play explosion sound
	Message msg((MessageType)ActorMessageType::Sound);
	msg.m_sender = id;
	msg.m_receiver = id;
	msg.m_int = (int)SoundType::InvaderExplode;
	m_systemManager->getMessageHandler()->dispatch(msg);
	// create shockwave
	const auto& posComp = m_systemManager->getActorManager()->getActor(id)->getComponent<Comp_Position>(ComponentType::Position);
	instantiateShockwave(posComp->getPosition());
	// disable invader
	m_systemManager->getActorManager()->disableActor(id);
	// update level manager
	m_systemManager->getLevelManager()->onInvaderDefeated();
}

void Sys_InvaderControl::setInvaderSpeed(const float& speed)
{
	// set invader AI speed
	m_aiSpeed = speed;
	// decrease beat duration as speed increases
	m_beatDuration = m_systemManager->getLevelManager()->getLevelBaseSpeed() / m_aiSpeed;
}
