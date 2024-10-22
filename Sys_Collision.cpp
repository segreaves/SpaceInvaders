#include "Sys_Collision.h"
#include "ActorManager.h"
#include "SysManager.h"
#include "WindowManager.h"

Sys_Collision::Sys_Collision(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_Collision::~Sys_Collision()
{
	onDestroy();
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
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Collision);
	m_requirements.emplace_back(req);
}

void Sys_Collision::subscribeToChannels()
{
}

void Sys_Collision::unsubscribeFromChannels()
{
}

void Sys_Collision::update(const float& deltaTime)
{
	// update all collision components with the current position
	for (auto& id : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(id);
		auto posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		auto colComp = actor->getComponent<Comp_Collision>(ComponentType::Collision);
		colComp->setPosition(posComp->getPosition());
	}
	// check collisions between specific actor groups
	detectCollisions();
}

void Sys_Collision::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Collision::debugOverlay(WindowManager* windowManager)
{
#ifdef DEBUG
	if (m_actorIds.empty()) return;
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
		auto colComp = actor->getComponent<Comp_Collision>(ComponentType::Collision);
		sf::RectangleShape shape(sf::Vector2f(colComp->getAABB().width, colComp->getAABB().height));
		shape.setPosition(colComp->getAABB().left, colComp->getAABB().top);
		sf::Color color = sf::Color::Yellow;
		color.a = 50;
		shape.setFillColor(color);
		windowManager->getRenderWindow()->draw(shape);
	}
	for (auto& rect : m_intersects)
		windowManager->getRenderWindow()->draw(rect);
	m_intersects.clear();
#endif
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
		auto posComp1 = actor1->getComponent<Comp_Position>(ComponentType::Position);
		auto colComp1 = actor1->getComponent<Comp_Collision>(ComponentType::Collision);
		colComp1->setPosition(posComp1->getPosition());
		for (auto id2 = std::next(id1); id2 != m_actorIds.end(); id2++)
		{
			Actor* actor2 = m_systemManager->getActorManager()->getActor(*id2);
			auto posComp2 = actor2->getComponent<Comp_Position>(ComponentType::Position);
			auto colComp2 = actor2->getComponent<Comp_Collision>(ComponentType::Collision);
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
	if (m_actorIds.empty()) return;
	// check player collisions
	if (m_actorGroups.find("player") != m_actorGroups.end())
	{
		for (auto& playerId : m_actorGroups["player"])
		{
			// check collisions against bullets
			if (m_actorGroups.find("bullet_invader") != m_actorGroups.end())
				for (auto& bulletId : m_actorGroups["bullet_invader"])
					if (detectActorCollision(playerId, bulletId))
					{
						// inform bullet of collision
						Message msg((MessageType)ActorMessageType::Collision);
						msg.m_sender = playerId;
						msg.m_receiver = bulletId;
						m_systemManager->getMessageHandler()->dispatch(msg);
						break;
					}
		}
	}
	// check invader collisions
	if (m_actorGroups.find("invader") != m_actorGroups.end())
	{
		for (auto& invaderId : m_actorGroups["invader"])
		{
			// check collisions against player bullets
			if (m_actorGroups.find("bullet_player") != m_actorGroups.end())
				for (auto& bulletId : m_actorGroups["bullet_player"])
					if (detectActorCollision(invaderId, bulletId))
					{
						// inform bullet of collision
						Message msg((MessageType)ActorMessageType::Collision);
						msg.m_sender = invaderId;
						msg.m_receiver = bulletId;
						m_systemManager->getMessageHandler()->dispatch(msg);
						break;
					}
			// check collisions against player
			if (m_actorGroups.find("player") != m_actorGroups.end())
				for (auto& playerId : m_actorGroups["player"])
					if (detectActorCollision(invaderId, playerId))
						break;
		}
	}
	// check shockwave collisions
	if (m_actorGroups.find("shockwave") != m_actorGroups.end())
	{
		for (auto& shockwaveId : m_actorGroups["shockwave"])
		{
			// check collisions against invaders
			if (m_actorGroups.find("invader") != m_actorGroups.end())
				for (auto& invaderId : m_actorGroups["invader"])
					detectActorCollision(shockwaveId, invaderId);
		}
	}
	// check bunker collisions
	if (m_actorGroups.find("bunker") != m_actorGroups.end())
	{
		for (auto& bunkerId : m_actorGroups["bunker"])
		{
			// check collisions against player bullets
			if (m_actorGroups.find("bullet_player") != m_actorGroups.end())
				for (auto& playerBulletId : m_actorGroups["bullet_player"])
					detectActorCollision(bunkerId, playerBulletId);
			// check collisions against invader bullets
			if (m_actorGroups.find("bullet_invader") != m_actorGroups.end())
				for (auto& invaderBulletId : m_actorGroups["bullet_invader"])
					detectActorCollision(bunkerId, invaderBulletId);
		}
	}
	// check player bullet collisions
	if (m_actorGroups.find("bullet_player") != m_actorGroups.end())
		for (auto& playerBulletId : m_actorGroups["bullet_player"])
		{
			// check collisions against invader bullets
			if (m_actorGroups.find("bullet_invader") != m_actorGroups.end())
				for (auto& invaderBulletId : m_actorGroups["bullet_invader"])
					if (detectActorCollision(playerBulletId, invaderBulletId))
					{
						// inform other bullet of collision
						Message msg((MessageType)ActorMessageType::Collision);
						msg.m_sender = playerBulletId;
						msg.m_receiver = invaderBulletId;
						m_systemManager->getMessageHandler()->dispatch(msg);
						break;
					}
		}
	// check mystery ship collisions
	if (m_actorGroups.find("mystery") != m_actorGroups.end())
	{
		for (auto& mysteryId : m_actorGroups["mystery"])
		{
			// check collisions against bullets
			if (m_actorGroups.find("bullet_player") != m_actorGroups.end())
				for (auto& bulletId : m_actorGroups["bullet_player"])
					if (detectActorCollision(mysteryId, bulletId))
					{
						// inform other bullet of collision
						Message msg((MessageType)ActorMessageType::Collision);
						msg.m_sender = mysteryId;
						msg.m_receiver = bulletId;
						m_systemManager->getMessageHandler()->dispatch(msg);
						break;
					}
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
bool Sys_Collision::detectActorCollision(const ActorId& actorId, const ActorId& otherId)
{
	auto actorCollider = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Collision>(ComponentType::Collision);
	auto otherCollider = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Collision>(ComponentType::Collision);
	auto otherPosition = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Position>(ComponentType::Position);
	otherCollider->setPosition(otherPosition->getPosition());
	sf::FloatRect intersect;
	if (actorCollider->getAABB().intersects(otherCollider->getAABB(), intersect))
	{
#ifdef DEBUG
		sf::RectangleShape rectIntersect(sf::Vector2f(intersect.width, intersect.height));
		rectIntersect.setPosition(sf::Vector2f(intersect.left, intersect.top));
		sf::Color color = sf::Color::Red;
		color.a = 100;
		rectIntersect.setFillColor(color);
		m_intersects.emplace_back(rectIntersect);
#endif
		Message msg((MessageType)ActorMessageType::Collision);
		msg.m_sender = otherId;
		msg.m_receiver = actorId;
		m_systemManager->getMessageHandler()->dispatch(msg);
		return true;
	}
	else
		return false;
}
