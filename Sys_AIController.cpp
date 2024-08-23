#include "Sys_AIController.h"
#include "SysManager.h"
#include "WindowManager.h"

Sys_AIController::Sys_AIController(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_AIController::~Sys_AIController()
{
	unsubscribeFromChannels();
}

void Sys_AIController::start()
{
	// set random direction
	srand(time(NULL));
	m_movingRight = rand() % 2;
}

void Sys_AIController::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Collision);
	m_requirements.set((unsigned int)CompType::Movable);
	m_requirements.set((unsigned int) CompType::Control);
	m_requirements.set((unsigned int)CompType::AI);
}

void Sys_AIController::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::OutOfBounds, this);
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_AIController::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::OutOfBounds, this);
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_AIController::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
		move(id, deltaTime);
}

void Sys_AIController::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
		removeActor(actorId);
		// increase speed
		break;
	case ActorEventType::CollidingOnX:
		m_movingRight = !m_movingRight;
		break;
	}
}

void Sys_AIController::debugOverlay(WindowManager* windowManager)
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

void Sys_AIController::notify(const Message& msg)
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

void Sys_AIController::move(const ActorId& actorId, const float& deltaTime)
{
	Comp_Position* posComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Position>(CompType::Position);
	Comp_AI* aiComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_AI>(CompType::AI);
	Comp_Movable* moveComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Movable>(CompType::Movable);
	Comp_Control* controlComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Control>(CompType::Control);
	setMoveTarget(aiComp, deltaTime);
	sf::Vector2f direction = aiComp->getTarget() - posComp->getPosition();
	controlComp->setMovementInput(direction);
	moveComp->accelerate(controlComp->getMovementInput());
}

void Sys_AIController::setMoveTarget(Comp_AI* aiComp, const float& deltaTime)
{
	if (m_movingRight)
		aiComp->setTarget(aiComp->getTarget() + sf::Vector2f(m_targetSpeed, 0) * deltaTime);
	else
		aiComp->setTarget(aiComp->getTarget() + sf::Vector2f(-m_targetSpeed, 0) * deltaTime);
}
