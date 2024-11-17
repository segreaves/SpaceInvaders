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
		auto actor = m_systemManager->getActorManager()->getActor(id);
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
		auto actor = m_systemManager->getActorManager()->getActor(actorId);
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

void Sys_Collision::onActorAdded(const ActorId& actorId)
{
	auto actor = m_systemManager->getActorManager()->getActor(actorId);
	auto actorTag = actor->getTag();
	// look for tag in actor groups. If not there, add it
	if (m_actorGroups.find(actorTag) == m_actorGroups.end())
		m_actorGroups.emplace(actorTag, std::vector<ActorId>());
	// add actor to group
	m_actorGroups[actorTag].emplace_back(actorId);
}

/// <summary>
/// Detect collisions between actors. Not all actors can collide with one another, so we
/// only need to check for collisions between certain actor groups.
/// Once invaders have detected a collision, we can break the for loop, as this means that either
/// the invader has been destroyed or the game has been lost.
/// Collisions:
/// - Player bullets -> Invaders
/// - Invader bullets -> Player
/// - Invaders -> Player
/// - Invaders -> Bunkers
/// - Shockwave -> Invaders
/// - Bunkers -> Player bullets
/// - Bunkers -> Invader bullets
/// - Player bullets -> Invader bullets
/// - Mystery Ship -> Player bullets
/// </summary>
void Sys_Collision::detectCollisions()
{
	if (m_actorIds.empty()) return;
	auto actorManager = m_systemManager->getActorManager();
	auto actorGroups = actorManager->getActorGroups();
	// check player collisions
	if (actorGroups->find("player") != actorGroups->end())
	{
		for (auto& playerId : actorGroups->at("player"))
		{
			if (!actorManager->getActor(playerId)->isEnabled()) continue;
			// check collisions against bullets
			if (actorGroups->find("bullet_invader") != actorGroups->end())
				for (auto& bulletId : actorGroups->at("bullet_invader"))
				{
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
	}
	// check invader collisions
	if (actorGroups->find("invader") != actorGroups->end())
	{
		for (auto& invaderId : actorGroups->at("invader"))
		{
			if (!actorManager->getActor(invaderId)->isEnabled()) continue;
			// check collisions against player bullets
			if (actorGroups->find("bullet_player") != actorGroups->end())
				for (auto& bulletId : actorGroups->at("bullet_player"))
				{
					if (detectActorCollision(invaderId, bulletId))
					{
						// inform bullet of collision
						Message msg((MessageType)ActorMessageType::Collision);
						msg.m_sender = invaderId;
						msg.m_receiver = bulletId;
						m_systemManager->getMessageHandler()->dispatch(msg);
						break;
					}
				}
			// check collisions against player
			if (actorGroups->find("player") != actorGroups->end())
				for (auto& playerId : actorGroups->at("player"))
				{
					if (detectActorCollision(invaderId, playerId))
					{
						// create invaded event
						m_systemManager->addEvent(playerId, (EventId)ActorEventType::Invaded);
						break;
					}
				}
			// check collisions against bunkers
			if (actorGroups->find("bunker") != actorGroups->end())
				for (auto& bunkerId : actorGroups->at("bunker"))
				{
					if (detectActorCollision(invaderId, bunkerId))
					{
						// create invaded event - SEND TO PLAYER
						m_systemManager->addEvent(m_systemManager->getLevelManager()->getPlayerId(), (EventId)ActorEventType::Invaded);
						break;
					}
				}
		}
	}
	// check shockwave collisions
	if (actorGroups->find("shockwave") != actorGroups->end())
	{
		for (auto& shockwaveId : actorGroups->at("shockwave"))
		{
			if (!actorManager->getActor(shockwaveId)->isEnabled()) continue;
			// check collisions against invaders
			if (actorGroups->find("invader") != actorGroups->end())
				for (auto& invaderId : actorGroups->at("invader"))
				{
					detectActorCollision(shockwaveId, invaderId);
				}
		}
	}
	// check bunker collisions
	if (actorGroups->find("bunker") != actorGroups->end())
	{
		for (auto& bunkerId : actorGroups->at("bunker"))
		{
			if (!actorManager->getActor(bunkerId)->isEnabled()) continue;
			// check collisions against player bullets
			if (actorGroups->find("bullet_player") != actorGroups->end())
			{
				for (auto& playerBulletId : actorGroups->at("bullet_player"))
				{
					detectActorCollision(bunkerId, playerBulletId);
				}
			}
			// check collisions against invader bullets
			if (actorGroups->find("bullet_invader") != actorGroups->end())
			{
				for (auto& invaderBulletId : actorGroups->at("bullet_invader"))
				{
					detectActorCollision(bunkerId, invaderBulletId);
				}
			}
		}
	}
	// check player bullet collisions
	if (actorGroups->find("bullet_player") != actorGroups->end())
	{
		for (auto& playerBulletId : actorGroups->at("bullet_player"))
		{
			if (!actorManager->getActor(playerBulletId)->isEnabled()) continue;
			// check collisions against invader bullets
			if (actorGroups->find("bullet_invader") != actorGroups->end())
				for (auto& invaderBulletId : actorGroups->at("bullet_invader"))
				{
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
		}
	}
	// check mystery ship collisions
	if (actorGroups->find("mystery") != actorGroups->end())
	{
		for (auto& mysteryId : actorGroups->at("mystery"))
		{
			if (!actorManager->getActor(mysteryId)->isEnabled()) continue;
			// check collisions against player bullets
			if (actorGroups->find("bullet_player") != actorGroups->end())
				for (auto& bulletId : actorGroups->at("bullet_player"))
				{
					if (detectActorCollision(mysteryId, bulletId))
					{
						// inform bullet of collision
						Message msg((MessageType)ActorMessageType::Collision);
						msg.m_sender = mysteryId;
						msg.m_receiver = bulletId;
						m_systemManager->getMessageHandler()->dispatch(msg);
						break;
					}
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
	if (!m_systemManager->getActorManager()->getActor(otherId)->isEnabled()) return false;

	auto actorCollider = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Collision>(ComponentType::Collision);
	auto otherCollider = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Collision>(ComponentType::Collision);
	auto otherPosition = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Position>(ComponentType::Position);
	sf::FloatRect intersect;
	if (actorCollider->getAABB().intersects(otherCollider->getAABB(), intersect))
	{
#ifdef DEBUG
		sf::RectangleShape rectIntersect(sf::Vector2f(intersect.width, intersect.height));
		rectIntersect.setPosition(sf::Vector2f(intersect.left, intersect.top));
		sf::Color color = sf::Color::Red;
		color.a = 150;
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
