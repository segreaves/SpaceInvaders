#include "Sys_Rotation.h"
#include "SysManager.h"

Sys_Rotation::Sys_Rotation(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_Rotation::~Sys_Rotation()
{
	onDestroy();
}

void Sys_Rotation::start()
{
}

void Sys_Rotation::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Movement);
	m_requirements.emplace_back(req);
}

void Sys_Rotation::subscribeToChannels()
{
}

void Sys_Rotation::unsubscribeFromChannels()
{
}

void Sys_Rotation::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	for (auto& id : m_actorIds)
	{
		auto actor = m_systemManager->getActorManager()->getActor(id);
		auto posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		auto moveComp = actor->getComponent<Comp_Movement>(ComponentType::Movement);
		// handle rotation
		const float torque = moveComp->getTorque();
		moveComp->addAngularVelocity(torque * deltaTime);
		moveComp->applyAngularDampening(moveComp->getAngularVelocity() * deltaTime);
		posComp->rotate(moveComp->getAngularVelocity() * deltaTime);
		moveComp->setTorque(0.f);
	}
}

void Sys_Rotation::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Rotation::debugOverlay(WindowManager* windowManager)
{
}

void Sys_Rotation::notify(const Message& msg)
{
}
