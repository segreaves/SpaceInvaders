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
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::OutOfBounds, this);
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_InvaderControl::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::OutOfBounds, this);
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
  		removeActor(actorId);
		m_invaderDefeated.dispatch(m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Position>(ComponentType::Position)->getPosition());
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
		break;
	case ActorEventType::CollidingOnX:
		m_movingRight = !m_movingRight;
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
	if (!hasActor(msg.m_receiver)) return;
	ActorMessageType msgType = (ActorMessageType)msg.m_type;
	switch (msgType)
	{
	case ActorMessageType::OutOfBounds:
		// set new movement target for all invaders
		for (auto& actorId : m_actorIds)
		{
			Actor* invader = m_systemManager->getActorManager()->getActor(actorId);
			Comp_Invader* aiComp = invader->getComponent<Comp_Invader>(ComponentType::Invader);
			Comp_Collision* colComp = invader->getComponent<Comp_Collision>(ComponentType::Collision);
			aiComp->setTarget(aiComp->getTarget() + sf::Vector2f(msg.m_xy.x, msg.m_xy.y + colComp->getAABB().height));
		}
		m_movingRight = !m_movingRight;
		break;
	case ActorMessageType::Collision:
		ActorManager* actorManager = m_systemManager->getActorManager();
		Actor* actor = actorManager->getActor(msg.m_receiver);
		Actor* other = actorManager->getActor(msg.m_sender);
		if (other->getTag() == "player")
		{
			// player collided with invader
			std::cout << "Player invaded!" << std::endl;
			m_systemManager->addEvent(msg.m_sender, (EventId)ActorEventType::Despawned);
			return;
		}
		else if (other->getTag() == "bullet_player")
			m_systemManager->addEvent(msg.m_sender, (EventId)ActorEventType::Despawned);
		m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
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

	// check if invader is out of bounds
	sf::FloatRect invaderAABB = colComp->getAABB();

	float resolve = 0;
	if (invComp->getTarget().x - invaderAABB.width / 2.f < 0)
		resolve = -(invComp->getTarget().x - invaderAABB.width / 2.f);
	else if (invComp->getTarget().x + invaderAABB.width / 2.f > m_levelManager->getViewSpace().getSize().x)
		resolve = -(invComp->getTarget().x + invaderAABB.width / 2.f - m_levelManager->getViewSpace().getSize().x);
	if (resolve != 0)
	{
		Message msg((MessageType)ActorMessageType::OutOfBounds);
		msg.m_receiver = id;
		msg.m_sender = id;
		msg.m_xy.x = resolve;
		msg.m_xy.y = 0;
		m_systemManager->getMessageHandler()->dispatch(msg);
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
			m_invaderShot.dispatch(id);
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