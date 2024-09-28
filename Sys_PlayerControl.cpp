#include "Sys_PlayerControl.h"
#include "SysManager.h"
#include "ActorManager.h"
#include "LevelManager.h"
#include "WindowManager.h"
#include <SFML/System/Vector2.hpp>

Sys_PlayerControl::Sys_PlayerControl(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_PlayerControl::~Sys_PlayerControl()
{
	unsubscribeFromChannels();
}

void Sys_PlayerControl::start()
{
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		Actor* player = actorManager->getActor(id);
		Comp_Position* posComp = player->getComponent<Comp_Position>(ComponentType::Position);
		Comp_Target* targetComp = player->getComponent<Comp_Target>(ComponentType::Target);
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
	for (auto& id : m_actorIds)
	{
		Actor* player = m_systemManager->getActorManager()->getActor(id);
		// clamp the player target to the view space
		Comp_Target* targetComp = player->getComponent<Comp_Target>(ComponentType::Target);
		float lowerBound = m_levelManager->getViewSpace().getPosition().x;
		float upperBound = m_levelManager->getViewSpace().getPosition().x + m_levelManager->getViewSpace().getSize().x;
		targetComp->setTarget(sf::Vector2f(targetComp->getTarget().x < lowerBound ? lowerBound : (targetComp->getTarget().x > upperBound ? upperBound : targetComp->getTarget().x), targetComp->getTarget().y));
		
		Comp_Control* controlComp = player->getComponent<Comp_Control>(ComponentType::Control);
		Comp_Position* posComp = player->getComponent<Comp_Position>(ComponentType::Position);
		sf::Vector2f direction = targetComp->getTarget() - posComp->getPosition();

		Comp_Movement* moveComp = player->getComponent<Comp_Movement>(ComponentType::Movement);
		controlComp->setMovementInput(direction);
		moveComp->accelerate(controlComp->getMovementInput() * controlComp->getMaxAcceleration());

		// check if player is out of bounds
		Comp_Collision* colComp = player->getComponent<Comp_Collision>(ComponentType::Collision);
		sf::FloatRect playerAABB = colComp->getAABB();

		float resolve = 0;
		if (playerAABB.left < 0)
			resolve = -playerAABB.left;
		else if (playerAABB.left + playerAABB.width > m_levelManager->getViewSpace().getSize().x)
			resolve = -(playerAABB.left + playerAABB.width - m_levelManager->getViewSpace().getSize().x);
		if (resolve != 0)
		{
			Comp_Position* posComp = player->getComponent<Comp_Position>(ComponentType::Position);
			Comp_Movement* moveComp = player->getComponent<Comp_Movement>(ComponentType::Movement);
			posComp->move(resolve, 0);
			moveComp->setAcceleration(0, moveComp->getAcceleration().y);
			moveComp->setVelocity(0, moveComp->getVelocity().y);
			moveComp->setCollidingOnX(true);
		}
	}
}

void Sys_PlayerControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_PlayerControl::debugOverlay(WindowManager* windowManager)
{
	ActorManager* actorManager = m_systemManager->getActorManager();
	sf::RenderWindow* window = windowManager->getRenderWindow();
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = actorManager->getActor(actorId);
		Comp_Target* targetComp = actor->getComponent<Comp_Target>(ComponentType::Target);
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
		ActorManager* actorManager = m_systemManager->getActorManager();
		Actor* actor = actorManager->getActor(msg.m_receiver);
		Actor* other = actorManager->getActor(msg.m_sender);
		if (other->getTag() == "bullet")
			m_systemManager->addEvent(msg.m_sender, (EventId)ActorEventType::Despawned);
		m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
		break;
	}
}

void Sys_PlayerControl::setLevelManager(LevelManager* levelManager)
{
	m_levelManager = levelManager;
}
