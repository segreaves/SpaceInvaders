#include "Sys.h"
#include "SysManager.h"
#include "Message.h"

Sys::Sys(SysManager* systemManager) :
	m_systemManager(systemManager)
{
}

bool Sys::addActor(const ActorId& actor)
{
	if (hasActor(actor)) return false;
	m_actorIds.emplace_back(actor);
	return true;
}

bool Sys::removeActor(const ActorId& actor)
{
	auto it = std::find(m_actorIds.begin(), m_actorIds.end(), actor);
	if (it == m_actorIds.end()) return false;
	m_actorIds.erase(it);
	return true;
}

void Sys::removeAllActors()
{
	m_actorIds.clear();
}

bool Sys::hasActor(const ActorId& actor)
{
	if (std::find(m_actorIds.begin(), m_actorIds.end(), actor) != m_actorIds.end()) return true;
	return false;
}

bool Sys::fitsRequirements(const Bitmask& mask)
{
	for (int i = 0; i < m_requirements.size(); i++)
		if (m_requirements.test(i) && !mask.test(i))
			return false;
	return true;
}
