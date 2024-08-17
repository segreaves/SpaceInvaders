#include "Sys_Collision.h"
#include "ActorManager.h"
#include "SysManager.h"
#include "WindowManager.h"
#include "Level.h"

Sys_Collision::Sys_Collision(SysManager* systemManager) :
	Sys(systemManager),
	m_level(nullptr)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_Collision::~Sys_Collision()
{
	unsubscribeFromChannels();
}

void Sys_Collision::setupRequirements()
{
	m_requirements.set(
		(unsigned int)CompType::Position |
		(unsigned int)CompType::Collision);
}

void Sys_Collision::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Resolve, this);
}

void Sys_Collision::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Resolve, this);
}

void Sys_Collision::update(const float& deltaTime)
{
	levelCollisions();
	actorCollisions();
}

void Sys_Collision::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Collision::debugOverlay(WindowManager* windowManager)
{
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
		Comp_Collision* colComp = actor->getComponent<Comp_Collision>(CompType::Collision);
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
	if (!hasActor(msg.m_receiver)) return;
	ActorMessageType messageType = (ActorMessageType)msg.m_type;
	switch (messageType)
	{
	case ActorMessageType::Resolve:
		Actor* actor = m_systemManager->getActorManager()->getActor(msg.m_receiver);
		Comp_Position* posComp = actor->getComponent<Comp_Position>(CompType::Position);
		Comp_Collision* colComp = actor->getComponent<Comp_Collision>(CompType::Collision);
		colComp->setPosition(posComp->getPosition());
		break;
	}
}

void Sys_Collision::setLevel(Level* level)
{
	m_level = level;
}

void Sys_Collision::levelCollisions()
{
	for (auto& actorId : m_actorIds)
	{
		Actor* actor = m_systemManager->getActorManager()->getActor(actorId);
		Comp_Position* posComp = actor->getComponent<Comp_Position>(CompType::Position);
		Comp_Collision* colComp = actor->getComponent<Comp_Collision>(CompType::Collision);
		colComp->setPosition(posComp->getPosition());
		colComp->resetCollision();
		actorOnLevelCollisions(actorId, posComp, colComp);
	}
}

void Sys_Collision::actorOnLevelCollisions(const ActorId& actorId, Comp_Position* posComp, Comp_Collision* colComp)
{
	auto levelColliders = m_level->getColliders();
	if (levelColliders.empty()) return;
	sf::FloatRect actorAABB = colComp->getAABB();
	for (auto& obj : levelColliders)
	{
		sf::FloatRect intersection;
		if (actorAABB.intersects(obj.second, intersection))
		{
			float resolve = 0;
			if (obj.first == Direction::Left || obj.first == Direction::Right)
			{
				float xDiff = (actorAABB.left + actorAABB.width / 2.f) -
					(obj.second.left + obj.second.width);
				if (xDiff > 0)
					resolve = obj.second.left + obj.second.width - actorAABB.left;
				else
					resolve = -(actorAABB.left + actorAABB.width - obj.second.left);
				m_systemManager->addEvent(actorId, (ActorEvent)ActorEventType::CollidingOnX);
				colComp->setCollidingX(true);
				Message msg((MessageType)ActorMessageType::Resolve);
				msg.m_receiver = actorId;
				msg.m_sender = actorId;
				msg.m_xy.x = resolve;
				msg.m_xy.y = 0;
				m_systemManager->getMessageHandler()->dispatch(msg);
			}
			else
			{
				float yDiff = (actorAABB.top + actorAABB.height / 2.f) -
					(obj.second.top + obj.second.top);
				if (yDiff > 0)
					resolve = obj.second.top + obj.second.height - actorAABB.top;
				else
					resolve = obj.second.top + actorAABB.top - actorAABB.height;
				m_systemManager->addEvent(actorId, (ActorEvent)ActorEventType::CollidingOnY);
				colComp->setCollidingY(true);
				Message msg((MessageType)ActorMessageType::Resolve);
				msg.m_receiver = actorId;
				msg.m_sender = actorId;
				msg.m_xy.x = 0;
				msg.m_xy.y = resolve;
				m_systemManager->getMessageHandler()->dispatch(msg);
			}
		}
	}
}

void Sys_Collision::actorCollisions()
{
	for (auto id1 = m_actorIds.begin(); id1 != m_actorIds.end(); id1++)
	{
		for (auto id2 = std::next(id1); id2 != m_actorIds.end(); id2++)
		{
			Comp_Collision* colComp1 = m_systemManager->getActorManager()->getActor(*id1)->getComponent<Comp_Collision>(CompType::Collision);
			Comp_Collision* colComp2 = m_systemManager->getActorManager()->getActor(*id2)->getComponent<Comp_Collision>(CompType::Collision);
			if (colComp1->getAABB().intersects(colComp2->getAABB()))
			{
				Message msg1((MessageType)ActorMessageType::Collision);
				msg1.m_receiver = *id2;
				msg1.m_sender = *id1;
				m_systemManager->getMessageHandler()->dispatch(msg1);

				Message msg2((MessageType)ActorMessageType::Collision);
				msg2.m_receiver = *id1;
				msg2.m_sender = *id2;
				m_systemManager->getMessageHandler()->dispatch(msg2);
			}
		}
	}
}
