#include "Sys_Bounds.h"
#include "SysManager.h"
#include "WindowManager.h"

Sys_Bounds::Sys_Bounds(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_Bounds::~Sys_Bounds()
{
	unsubscribeFromChannels();
}

void Sys_Bounds::start()
{
	selectTrackedInvaders();
}

void Sys_Bounds::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Movable);
	m_requirements.set((unsigned int)CompType::Collision);
}

void Sys_Bounds::subscribeToChannels()
{
}

void Sys_Bounds::unsubscribeFromChannels()
{
}

void Sys_Bounds::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
		if (actor->getTag() == "player")
			trackPlayerPosition(actor);
		else if (actor->getTag() == "invader" && (actor->getId() == m_leftInvader || actor->getId() == m_rightInvader))
			trackInvadersPosition(actor);
		else if (actor->getTag() == "bullet")
			trackBulletsPosition(actor);
	}
}

void Sys_Bounds::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
		removeActor(actorId);
		selectTrackedInvaders();
		break;
	}
}

void Sys_Bounds::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Bounds::notify(const Message& msg)
{
}

void Sys_Bounds::setViewSpace(sf::FloatRect viewSpace)
{
	m_viewSpace = viewSpace;
}

void Sys_Bounds::trackPlayerPosition(Actor* player)
{
	if (!player->getComponent<Comp_Player>(CompType::Player)) return;
	Comp_Collision* colComp = player->getComponent<Comp_Collision>(CompType::Collision);
	sf::FloatRect playerAABB = colComp->getAABB();
	
	float resolve = 0;
	if (playerAABB.left < 0)
		resolve = -playerAABB.left;
	else if (playerAABB.left + playerAABB.width > m_viewSpace.getSize().x)
		resolve = -(playerAABB.left + playerAABB.width - m_viewSpace.getSize().x);
	if (resolve != 0)
	{
		Comp_Position* posComp = player->getComponent<Comp_Position>(CompType::Position);
		Comp_Movable* moveComp = player->getComponent<Comp_Movable>(CompType::Movable);
		posComp->move(resolve, 0);
		moveComp->setAcceleration(0, moveComp->getAcceleration().y);
		moveComp->setVelocity(0, moveComp->getVelocity().y);
		moveComp->setCollidingOnX(true);
	}
}

void Sys_Bounds::trackInvadersPosition(Actor* invader)
{
	if (!invader->getComponent<Comp_AI>(CompType::AI)) return;
	Comp_Collision* colComp = invader->getComponent<Comp_Collision>(CompType::Collision);
	sf::FloatRect invaderAABB = colComp->getAABB();
	Comp_AI* aiComp = invader->getComponent<Comp_AI>(CompType::AI);

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

void Sys_Bounds::trackBulletsPosition(Actor* bullet)
{
	if (!bullet->getComponent<Comp_Bullet>(CompType::Bullet)) return;
	Comp_Collision* colComp = bullet->getComponent<Comp_Collision>(CompType::Collision);
	sf::FloatRect bulletAABB = colComp->getAABB();
	// check if bullet is out of bounds (vertically)
	if (bulletAABB.top + bulletAABB.height < 0 || bulletAABB.top > m_viewSpace.getSize().y)
		m_systemManager->addEvent(bullet->getId(), (EventId)ActorEventType::Despawned);
}

void Sys_Bounds::selectTrackedInvaders()
{
	float minX = m_viewSpace.getSize().x;
	float maxX = 0.f;
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
		if (actor->getTag() == "invader")
		{
			Comp_Position* posComp = actor->getComponent<Comp_Position>(CompType::Position);
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
	}
	Comp_Sprite* leftSprite = m_systemManager->getActorManager()->getActor(m_leftInvader)->getComponent<Comp_Sprite>(CompType::Sprite);
	if (leftSprite) leftSprite->setColor(sf::Color::Red);
	Comp_Sprite* rightSprite = m_systemManager->getActorManager()->getActor(m_rightInvader)->getComponent<Comp_Sprite>(CompType::Sprite);
	if (rightSprite) rightSprite->setColor(sf::Color::Red);
}
