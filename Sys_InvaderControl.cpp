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

void Sys_InvaderControl::start()
{
	srand(time(nullptr));
	m_movingRight = rand() % 2;

	selectTrackedInvaders();
}

void Sys_InvaderControl::setupRequirements()
{
	m_requirements.set((unsigned int)ComponentType::Position);
	m_requirements.set((unsigned int)ComponentType::Collision);
	m_requirements.set((unsigned int)ComponentType::Movement);
	m_requirements.set((unsigned int)ComponentType::Control);
	m_requirements.set((unsigned int)ComponentType::Invader);
}

void Sys_InvaderControl::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_InvaderControl::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_InvaderControl::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		Actor* invader = actorManager->getActor(id);
		Comp_Position* posComp = invader->getComponent<Comp_Position>(ComponentType::Position);
		Comp_Invader* invComp = invader->getComponent<Comp_Invader>(ComponentType::Invader);
		Comp_Movement* moveComp = invader->getComponent<Comp_Movement>(ComponentType::Movement);
		Comp_Control* controlComp = invader->getComponent<Comp_Control>(ComponentType::Control);
		Comp_Collision* colComp = invader->getComponent<Comp_Collision>(ComponentType::Collision);

		handleMovement(deltaTime, id, posComp, moveComp, controlComp, invComp, colComp);
		handleShooting(deltaTime, id, invComp);
	}
}

void Sys_InvaderControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
		m_systemManager->getActorManager()->disableActor(actorId);
		if (!m_actorIds.empty())
		{
			selectTrackedInvaders();
			ActorManager* actorManager = m_systemManager->getActorManager();
			for (auto& id : m_actorIds)
			{
				Actor* invader = actorManager->getActor(id);
				Comp_Control* controlComp = invader->getComponent<Comp_Control>(ComponentType::Control);
				controlComp->setMaxSpeed(controlComp->getMaxSpeed() + m_levelManager->getDefeatSpeedIncrease());
			}
		}
		m_invaderDefeated.dispatch(actorId);
		break;
	case ActorEventType::Shoot:
		m_invaderShoot.dispatch(actorId);
		break;
	}
}

void Sys_InvaderControl::debugOverlay(WindowManager* windowManager)
{
	ActorManager* actorManager = m_systemManager->getActorManager();
	sf::RenderWindow* window = windowManager->getRenderWindow();
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = actorManager->getActor(actorId);
		Comp_Invader* aiComp = actor->getComponent<Comp_Invader>(ComponentType::Invader);
		sf::CircleShape target(2.5f);
		target.setOrigin(target.getRadius(), target.getRadius());
		target.setFillColor(sf::Color::Red);
		target.setPosition(aiComp->getTarget());
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
			// inform bullet of collision
			Message msgOther((MessageType)ActorMessageType::Collision);
			msgOther.m_sender = msg.m_receiver;
			msgOther.m_receiver = other->getId();
			m_systemManager->getMessageHandler()->dispatch(msgOther);
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
		Comp_Position* posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		Comp_Sprite* sprite = actor->getComponent<Comp_Sprite>(ComponentType::Sprite);
		if (sprite) sprite->setColor(sprite->getDefaultColor());
		if (posComp->getPosition().x < minX)
		{
			m_leftInvader = actorId;
			minX = posComp->getPosition().x;
		}
		if (posComp->getPosition().x > maxX)
		{
			m_rightInvader = actorId;
			maxX = posComp->getPosition().x;
		}
	}
	Comp_Sprite* leftSprite = actorManager->getActor(m_leftInvader)->getComponent<Comp_Sprite>(ComponentType::Sprite);
	if (leftSprite) leftSprite->setColor(sf::Color::Red);
	Comp_Sprite* rightSprite = actorManager->getActor(m_rightInvader)->getComponent<Comp_Sprite>(ComponentType::Sprite);
	if (rightSprite) rightSprite->setColor(sf::Color::Red);
}

void Sys_InvaderControl::handleMovement(const float& deltaTime, const ActorId& id, Comp_Position* posComp, Comp_Movement* moveComp, Comp_Control* controlComp, Comp_Invader* invComp, Comp_Collision* colComp)
{
	// invader movement
	invComp->setTarget(invComp->getTarget() + sf::Vector2f(m_movingRight ? controlComp->getMaxSpeed() : -controlComp->getMaxSpeed(), 0) * deltaTime);
	sf::Vector2f direction = invComp->getTarget() - posComp->getPosition();
	controlComp->setMovementInput(direction / m_maxTargetDistance);
	moveComp->accelerate(controlComp->getMovementDirection() * controlComp->getMaxAcceleration());

	// check if invader is out of bounds. This is done only for the tracked invaders
	if (id != m_leftInvader && id != m_rightInvader) return;
	sf::FloatRect invaderAABB = colComp->getAABB();
	bool boundsLeft = invComp->getTarget().x - invaderAABB.width / 2.f < m_bounds;
	bool boundsRight = invComp->getTarget().x + invaderAABB.width / 2.f > m_levelManager->getViewSpace().getSize().x - m_bounds;
	if (boundsLeft || boundsRight)
	{
		float resolveX = boundsLeft ? (m_bounds + invaderAABB.width / 2.f - invComp->getTarget().x) : (m_levelManager->getViewSpace().getSize().x - m_bounds - invaderAABB.width / 2.f - invComp->getTarget().x);
		float resolveY = m_dropDistance;
		sf::Vector2f resolve = sf::Vector2f(resolveX, resolveY);
		// set new movement target for all invaders
		for (auto& actorId : m_actorIds)
		{
			Actor* invader = m_systemManager->getActorManager()->getActor(actorId);
			Comp_Invader* aiComp = invader->getComponent<Comp_Invader>(ComponentType::Invader);
			aiComp->setTarget(aiComp->getTarget() + resolve);
		}
		m_movingRight = boundsLeft ? true : false;
	}
}

void Sys_InvaderControl::handleShooting(const float& deltaTime, const ActorId& id, Comp_Invader* invComp)
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