#include "Sys_PlayerControl.h"
#include "SysManager.h"
#include "ActorManager.h"
#include "LevelManager.h"
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
}

void Sys_PlayerControl::setupRequirements()
{
	m_requirements.set((unsigned int)ComponentType::Control);
	m_requirements.set((unsigned int)ComponentType::Position);
	m_requirements.set((unsigned int)ComponentType::Movement);
	m_requirements.set((unsigned int)ComponentType::Player);
}

void Sys_PlayerControl::subscribeToChannels()
{
}

void Sys_PlayerControl::unsubscribeFromChannels()
{
}

void Sys_PlayerControl::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		Actor* player = actorManager->getActor(id);
		Comp_Control* controlComp = player->getComponent<Comp_Control>(ComponentType::Control);
		Comp_Movement* moveComp = player->getComponent<Comp_Movement>(ComponentType::Movement);

		moveComp->accelerate(controlComp->getMovementInput());

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
