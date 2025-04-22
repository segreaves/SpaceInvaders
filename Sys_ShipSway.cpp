#include "Sys_ShipSway.h"
#include "SysManager.h"
#include "WindowManager.h"

Sys_ShipSway::Sys_ShipSway(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_ShipSway::~Sys_ShipSway()
{
	onDestroy();
}

void Sys_ShipSway::start()
{
}

void Sys_ShipSway::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Movement);
	req.set((unsigned int)ComponentType::ShipSway);
	m_requirements.emplace_back(req);
}

void Sys_ShipSway::subscribeToChannels()
{
}

void Sys_ShipSway::unsubscribeFromChannels()
{
}

void Sys_ShipSway::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		const auto& actor = actorManager->getActor(id);
		const auto& moveComp = actor->getComponent<Comp_Movement>(ComponentType::Movement);
		const auto& swayComp = actor->getComponent<Comp_ShipSway>(ComponentType::ShipSway);
		const float speedChange = moveComp->getSpeedChange().x;
		const float torque = speedChange * swayComp->getSwayCoefficient();
		moveComp->addTorque(torque);
	}
}

void Sys_ShipSway::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_ShipSway::debugOverlay(WindowManager* windowManager)
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		auto actor = actorManager->getActor(id);
		auto moveComp = actor->getComponent<Comp_Movement>(ComponentType::Movement);
		// draw arrow representing speed change
		sf::Vector2f pos = actor->getComponent<Comp_Position>(ComponentType::Position)->getPosition();
		sf::Vector2f end = pos + sf::Vector2f(moveComp->getSpeedChange().x * 10.f, 0);
		sf::Vertex line[] =
		{
			sf::Vertex(pos, sf::Color::Blue),
			sf::Vertex(end, sf::Color::Blue)
		};
		windowManager->getRenderWindow()->draw(line, 2, sf::PrimitiveType::Lines);
	}
}

void Sys_ShipSway::notify(const Message& msg)
{
}
