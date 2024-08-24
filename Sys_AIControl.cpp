#include "Sys_AIControl.h"
#include "SysManager.h"
#include "WindowManager.h"

Sys_AIControl::Sys_AIControl(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_AIControl::~Sys_AIControl()
{
	unsubscribeFromChannels();
}

void Sys_AIControl::start()
{
	// set random direction
	srand(time(NULL));
	m_movingRight = rand() % 2;

	selectTrackedInvaders();
}

void Sys_AIControl::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Collision);
	m_requirements.set((unsigned int)CompType::Movable);
	m_requirements.set((unsigned int)CompType::Control);
	m_requirements.set((unsigned int)CompType::AI);
}

void Sys_AIControl::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::OutOfBounds, this);
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_AIControl::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::OutOfBounds, this);
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_AIControl::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		Actor* invader = actorManager->getActor(id);
		Comp_Position* posComp = invader->getComponent<Comp_Position>(CompType::Position);
		Comp_AI* aiComp = invader->getComponent<Comp_AI>(CompType::AI);
		Comp_Movable* moveComp = invader->getComponent<Comp_Movable>(CompType::Movable);
		Comp_Control* controlComp = invader->getComponent<Comp_Control>(CompType::Control);

		// invader movement
		aiComp->setTarget(aiComp->getTarget() + sf::Vector2f(m_movingRight ? m_targetSpeed : -m_targetSpeed, 0) * deltaTime);
		sf::Vector2f direction = aiComp->getTarget() - posComp->getPosition();
		controlComp->setMovementInput(direction);
		moveComp->accelerate(controlComp->getMovementInput());

		// check if invader is out of bounds
		Comp_Collision* colComp = invader->getComponent<Comp_Collision>(CompType::Collision);
		sf::FloatRect invaderAABB = colComp->getAABB();

		float resolve = 0;
		if (aiComp->getTarget().x - invaderAABB.width / 2.f < 0)
			resolve = -(aiComp->getTarget().x - invaderAABB.width / 2.f);
		else if (aiComp->getTarget().x + invaderAABB.width / 2.f > m_viewSpace.getSize().x)
			resolve = -(aiComp->getTarget().x + invaderAABB.width / 2.f - m_viewSpace.getSize().x);
		if (resolve != 0)
		{
			Message msg((MessageType)ActorMessageType::OutOfBounds);
			msg.m_receiver = invader->getId();
			msg.m_sender = invader->getId();
			msg.m_xy.x = resolve;
			msg.m_xy.y = 0;
			m_systemManager->getMessageHandler()->dispatch(msg);
		}
	}
}

void Sys_AIControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
		removeActor(actorId);
		if (m_actorIds.empty())
			m_invadersDefeated.dispatch();
		else
			if (actorId == m_leftInvader || actorId == m_rightInvader)
				selectTrackedInvaders();
		// increase speed
		break;
	case ActorEventType::CollidingOnX:
		m_movingRight = !m_movingRight;
		break;
	}
}

void Sys_AIControl::debugOverlay(WindowManager* windowManager)
{
	ActorManager* actorManager = m_systemManager->getActorManager();
	sf::RenderWindow* window = windowManager->getRenderWindow();
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = actorManager->getActor(actorId);
		Comp_AI* aiComp = actor->getComponent<Comp_AI>(CompType::AI);
		sf::CircleShape target(2.5f);
		target.setOrigin(target.getRadius(), target.getRadius());
		target.setFillColor(sf::Color::Red);
		target.setPosition(aiComp->getTarget());
		window->draw(target);
	}
}

void Sys_AIControl::notify(const Message& msg)
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
			Comp_AI* aiComp = invader->getComponent<Comp_AI>(CompType::AI);
			Comp_Collision* colComp = invader->getComponent<Comp_Collision>(CompType::Collision);
			aiComp->setTarget(aiComp->getTarget() + sf::Vector2f(msg.m_xy.x, msg.m_xy.y + colComp->getAABB().height));
		}
		m_movingRight = !m_movingRight;
		break;
		case ActorMessageType::Collision:
		{
			Actor* actor = m_systemManager->getActorManager()->getActor(msg.m_receiver);
			Actor* other = m_systemManager->getActorManager()->getActor(msg.m_sender);
			if (other->getTag() == "player")
			{
				// player collided with invader
				std::cout << "Player invaded!" << std::endl;
				m_systemManager->addEvent(msg.m_sender, (EventId)ActorEventType::Despawned);
				return;
			}
			m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
			break;
		}
	}
}

void Sys_AIControl::setViewSpace(sf::FloatRect viewSpace)
{
	m_viewSpace = viewSpace;
}

void Sys_AIControl::selectTrackedInvaders()
{
	if (getActorCount() == 0) return;
	float minX = m_viewSpace.getSize().x;
	float maxX = 0.f;
	m_leftInvader = -1;
	m_rightInvader = -1;
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
		Comp_Position* posComp = actor->getComponent<Comp_Position>(CompType::Position);
		Comp_Sprite* sprite = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Sprite>(CompType::Sprite);
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
	Comp_Sprite* leftSprite = m_systemManager->getActorManager()->getActor(m_leftInvader)->getComponent<Comp_Sprite>(CompType::Sprite);
	if (leftSprite) leftSprite->setColor(sf::Color::Red);
	Comp_Sprite* rightSprite = m_systemManager->getActorManager()->getActor(m_rightInvader)->getComponent<Comp_Sprite>(CompType::Sprite);
	if (rightSprite) rightSprite->setColor(sf::Color::Red);
}
