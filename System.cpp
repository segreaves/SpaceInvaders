#include "System.h"
#include "SysManager.h"
#include "Message.h"

System::System(SysManager* systemManager) :
	m_systemManager(systemManager)
{
}

bool System::addActor(const ActorId& actor)
{
	if (hasActor(actor)) return false;
	m_actorIds.emplace_back(actor);
	return true;
}

bool System::removeActor(const ActorId& actor)
{
	auto it = std::find(m_actorIds.begin(), m_actorIds.end(), actor);
	if (it == m_actorIds.end()) return false;
	m_actorIds.erase(it);
	return true;
}

void System::removeAllActors()
{
	m_actorIds.clear();
}

bool System::hasActor(const ActorId& actor)
{
	if (std::find(m_actorIds.begin(), m_actorIds.end(), actor) != m_actorIds.end()) return true;
	return false;
}

bool System::fitsRequirements(const Bitmask& mask)
{
	return mask == m_requirements;
}
