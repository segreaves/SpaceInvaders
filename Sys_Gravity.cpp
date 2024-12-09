#include "Sys_Gravity.h"
#include "SysManager.h"
#include "WindowManager.h"

Sys_Gravity::Sys_Gravity(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_Gravity::~Sys_Gravity()
{
	onDestroy();
}

void Sys_Gravity::start()
{
}

void Sys_Gravity::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Movement);
	req.set((unsigned int)ComponentType::Mass);
	m_requirements.emplace_back(req);
}

void Sys_Gravity::subscribeToChannels()
{
}

void Sys_Gravity::unsubscribeFromChannels()
{
}

void Sys_Gravity::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
	{
		auto actor = m_systemManager->getActorManager()->getActor(id);
		auto massComp = actor->getComponent<Comp_Mass>(ComponentType::Mass);
		auto moveComp = actor->getComponent<Comp_Movement>(ComponentType::Movement);

		sf::Vector2f gravityForce = massComp->getMass() * M_GRAVITY * M_DOWN;
		moveComp->accelerate(gravityForce);
	}
}

void Sys_Gravity::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Gravity::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Gravity::notify(const Message& msg)
{
}
