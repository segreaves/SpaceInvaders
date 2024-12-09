#include "Sys_TorqueSpring.h"
#include "SysManager.h"

Sys_TorqueSpring::Sys_TorqueSpring(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_TorqueSpring::~Sys_TorqueSpring()
{
	onDestroy();
}

void Sys_TorqueSpring::start()
{
}

void Sys_TorqueSpring::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Movement);
	req.set((unsigned int)ComponentType::TorqueSpring);
	m_requirements.emplace_back(req);
}

void Sys_TorqueSpring::subscribeToChannels()
{
}

void Sys_TorqueSpring::unsubscribeFromChannels()
{
}

void Sys_TorqueSpring::update(const float& deltaTime)
{
	if (m_actorIds.empty()) return;
	ActorManager* actorManager = m_systemManager->getActorManager();
	for (auto& id : m_actorIds)
	{
		const auto& actor = actorManager->getActor(id);
		const auto& posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		const auto& moveComp = actor->getComponent<Comp_Movement>(ComponentType::Movement);
		const auto& torqueSpringComp = actor->getComponent<Comp_TorqueSpring>(ComponentType::TorqueSpring);
		const auto& stiffness = torqueSpringComp->getStiffness();
		const auto& damping = torqueSpringComp->getDamping();
		const auto& angle = posComp->getAngle();
		const auto& angularVelocity = moveComp->getAngularVelocity();
		float torque = -stiffness * angle;
		torque -= damping * angularVelocity;
		moveComp->addTorque(torque);
	}
}

void Sys_TorqueSpring::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_TorqueSpring::debugOverlay(WindowManager* windowManager)
{
}

void Sys_TorqueSpring::notify(const Message& msg)
{
}
