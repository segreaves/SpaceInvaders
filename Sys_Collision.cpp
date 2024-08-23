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

void Sys_Collision::start()
{
}

void Sys_Collision::setupRequirements()
{
	m_requirements.set((unsigned int)CompType::Position);
	m_requirements.set((unsigned int)CompType::Collision);
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
	actorCollisions();
}

void Sys_Collision::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
		removeActor(actorId);
		break;
	}
}

void Sys_Collision::debugOverlay(WindowManager* windowManager)
{
	if (m_actorIds.empty()) return;
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
}

void Sys_Collision::actorCollisions()
{
	for (auto id1 = m_actorIds.begin(); id1 != m_actorIds.end(); id1++)
	{
		Actor* actor1 = m_systemManager->getActorManager()->getActor(*id1);
		Comp_Position* posComp1 = actor1->getComponent<Comp_Position>(CompType::Position);
		Comp_Collision* colComp1 = actor1->getComponent<Comp_Collision>(CompType::Collision);
		colComp1->setPosition(posComp1->getPosition());
		for (auto id2 = std::next(id1); id2 != m_actorIds.end(); id2++)
		{
			Actor* actor2 = m_systemManager->getActorManager()->getActor(*id2);
			Comp_Position* posComp2 = actor2->getComponent<Comp_Position>(CompType::Position);
			Comp_Collision* colComp2 = actor2->getComponent<Comp_Collision>(CompType::Collision);
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
