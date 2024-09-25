#include "Sys_Collision.h"
#include "ActorManager.h"
#include "SysManager.h"
#include "WindowManager.h"

Sys_Collision::Sys_Collision(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_Collision::~Sys_Collision()
{
	unsubscribeFromChannels();
}

bool Sys_Collision::addActor(const ActorId& actorId)
{
	if (!Sys::addActor(actorId)) return false;
	std::string tag = m_systemManager->getActorManager()->getActor(actorId)->getTag();
	m_actorGroups[tag].push_back(actorId);
	return true;
}

bool Sys_Collision::removeActor(const ActorId& actorId)
{
	if (!Sys::removeActor(actorId)) return false;
	std::string tag = m_systemManager->getActorManager()->getActor(actorId)->getTag();
	if (m_actorGroups.find(tag) == m_actorGroups.end()) return false;
	auto it = std::find(m_actorGroups[tag].begin(), m_actorGroups[tag].end(), actorId);
	if (it != m_actorGroups[tag].end())
		m_actorGroups[tag].erase(it);
	return true;
}

void Sys_Collision::start()
{
}

void Sys_Collision::setupRequirements()
{
	m_requirements.set((unsigned int)ComponentType::Position);
	m_requirements.set((unsigned int)ComponentType::Collision);
}

void Sys_Collision::subscribeToChannels()
{
}

void Sys_Collision::unsubscribeFromChannels()
{
}

void Sys_Collision::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	detectCollisions();
}

void Sys_Collision::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Collision::debugOverlay(WindowManager* windowManager)
{
	if (m_actorIds.empty()) return;
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
		Comp_Collision* colComp = actor->getComponent<Comp_Collision>(ComponentType::Collision);
		sf::RectangleShape shape(sf::Vector2f(colComp->getAABB().width, colComp->getAABB().height));
		shape.setPosition(colComp->getAABB().left, colComp->getAABB().top);
		sf::Color color = sf::Color::Yellow;
		color.a = 50;
		shape.setFillColor(color);
		windowManager->getRenderWindow()->draw(shape);
	}
}

void Sys_Collision::notify(const Message& msg)
{
}

/// <summary>
/// Detects collisions between actors. This is a brute force method, as it checks for collisions
/// between all actors. This is fine for small numbers of actors, but for larger numbers, the
/// detectCollisions() method was implemented.
/// </summary>
void Sys_Collision::actorCollisions()
{
	for (auto id1 = m_actorIds.begin(); id1 != m_actorIds.end(); id1++)
	{
		Actor* actor1 = m_systemManager->getActorManager()->getActor(*id1);
		Comp_Position* posComp1 = actor1->getComponent<Comp_Position>(ComponentType::Position);
		Comp_Collision* colComp1 = actor1->getComponent<Comp_Collision>(ComponentType::Collision);
		colComp1->setPosition(posComp1->getPosition());
		for (auto id2 = std::next(id1); id2 != m_actorIds.end(); id2++)
		{
			Actor* actor2 = m_systemManager->getActorManager()->getActor(*id2);
			Comp_Position* posComp2 = actor2->getComponent<Comp_Position>(ComponentType::Position);
			Comp_Collision* colComp2 = actor2->getComponent<Comp_Collision>(ComponentType::Collision);
			colComp2->setPosition(posComp2->getPosition());
			if (colComp1->getAABB().intersects(colComp2->getAABB()))
			{
				Message msg1((MessageType)ActorMessageType::Collision);
				msg1.m_sender = *id1;
				msg1.m_receiver = *id2;
				m_systemManager->getMessageHandler()->dispatch(msg1);

				Message msg2((MessageType)ActorMessageType::Collision);
				msg2.m_sender = *id2;
				msg2.m_receiver = *id1;
				m_systemManager->getMessageHandler()->dispatch(msg2);
			}
		}
	}
}

/// <summary>
/// Detect collisions between actors. Not all actors can collide with one another, so we
/// only need to check for collisions between certain actor groups.
/// Once invaders have detected a collision, we can break the for loop, as this means that either
/// the invader has been destroyed or the game has been lost.
/// </summary>
void Sys_Collision::detectCollisions()
{
	ActorManager* actorManager = m_systemManager->getActorManager();
	// check invader collisions
	if (m_actorGroups.find("invader") != m_actorGroups.end())
	{
		for (auto& invaderId : m_actorGroups["invader"])
		{
			Actor* invader = actorManager->getActor(invaderId);
			Comp_Collision* invaderCollider = invader->getComponent<Comp_Collision>(ComponentType::Collision);
			Comp_Position* invaderPosition = invader->getComponent<Comp_Position>(ComponentType::Position);
			invaderCollider->setPosition(invaderPosition->getPosition());
			// check collisions against bullets
			if (m_actorGroups.find("bullet_player") != m_actorGroups.end())
				for (auto& bulletId : m_actorGroups["bullet_player"])
					if (detectActorCollision(invaderId, invaderCollider, actorManager->getActor(bulletId)))
						break;
			// check collisions against bunkers
			if (m_actorGroups.find("bunker") != m_actorGroups.end())
				for (auto& bunkerId : m_actorGroups["bunker"])
					if (detectActorCollision(invaderId, invaderCollider, actorManager->getActor(bunkerId)))
						break;
			// check collisions against player
			if (m_actorGroups.find("player") != m_actorGroups.end())
				for (auto& playerId : m_actorGroups["player"])
					if (detectActorCollision(invaderId, invaderCollider, actorManager->getActor(playerId)))
						break;
		}
	}
	// check shockwave collisions
	if (m_actorGroups.find("shockwave") != m_actorGroups.end())
	{
		for (auto& shockwaveId : m_actorGroups["shockwave"])
		{
			Comp_Collision* shockwaveCollider = actorManager->getActor(shockwaveId)->getComponent<Comp_Collision>(ComponentType::Collision);
			// check collisions against invaders
			if (m_actorGroups.find("invader") != m_actorGroups.end())
				for (auto& invaderId : m_actorGroups["invader"])
					detectActorCollision(shockwaveId, shockwaveCollider, actorManager->getActor(invaderId));
		}
	}
	// check bunker collisions
	if (m_actorGroups.find("bunker") != m_actorGroups.end())
	{
		for (auto& bunkerId : m_actorGroups["bunker"])
		{
			Actor* bunker = actorManager->getActor(bunkerId);
			Comp_Collision* bunkerCollider = bunker->getComponent<Comp_Collision>(ComponentType::Collision);
			Comp_Position* bunkerPosition = bunker->getComponent<Comp_Position>(ComponentType::Position);
			bunkerCollider->setPosition(bunkerPosition->getPosition());
			// check collisions against bullets
			if (m_actorGroups.find("bullet_player") != m_actorGroups.end())
				for (auto& bulletId : m_actorGroups["bullet_player"])
					if (detectActorCollision(bunkerId, bunkerCollider, actorManager->getActor(bulletId)))
						break;
			if (m_actorGroups.find("bullet_invader") != m_actorGroups.end())
				for (auto& bulletId : m_actorGroups["bullet_invader"])
					if (detectActorCollision(bunkerId, bunkerCollider, actorManager->getActor(bulletId)))
						break;
		}
	}
	// check mystery ship collisions
	if (m_actorGroups.find("mystery") != m_actorGroups.end())
	{
		for (auto& mysteryId : m_actorGroups["mystery"])
		{
			Actor* mystery = actorManager->getActor(mysteryId);
			Comp_Collision* mysteryCollider = mystery->getComponent<Comp_Collision>(ComponentType::Collision);
			Comp_Position* mysteryPosition = mystery->getComponent<Comp_Position>(ComponentType::Position);
			mysteryCollider->setPosition(mysteryPosition->getPosition());
			// check collisions against bullets
			if (m_actorGroups.find("bullet") != m_actorGroups.end())
				for (auto& bulletId : m_actorGroups["bullet"])
					if (detectActorCollision(mysteryId, mysteryCollider, actorManager->getActor(bulletId)))
						break;
		}
	}
}

/// <summary>
/// Check collision for actorId against otherActor.
/// If colliding, then sends a collision message to the actorId.
/// </summary>
/// <param name="actorId"></param>
/// <param name="colComp"></param>
/// <param name="otherActor"></param>
/// <returns></returns>
bool Sys_Collision::detectActorCollision(const ActorId& actorId, Comp_Collision* colComp, Actor* otherActor)
{
	Comp_Collision* otherCollider = otherActor->getComponent<Comp_Collision>(ComponentType::Collision);
	Comp_Position* otherPosition = otherActor->getComponent<Comp_Position>(ComponentType::Position);
	otherCollider->setPosition(otherPosition->getPosition());
	if (colComp->getAABB().intersects(otherCollider->getAABB()))
	{
		Message msg((MessageType)ActorMessageType::Collision);
		msg.m_sender = otherActor->getId();
		msg.m_receiver = actorId;
		m_systemManager->getMessageHandler()->dispatch(msg);
		return true;
	}
	else
		return false;
}
