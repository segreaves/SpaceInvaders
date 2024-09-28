#include "Sys.h"
#include "SysManager.h"
#include "Message.h"

Sys::Sys(SysManager* systemManager) :
	m_systemManager(systemManager)
{
	m_requirements = std::vector<Bitmask>();
}

Sys::~Sys()
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

bool Sys::hasActor(const ActorId& actor)
{
	if (std::find(m_actorIds.begin(), m_actorIds.end(), actor) != m_actorIds.end()) return true;
	return false;
}

bool Sys::fitsRequirements(const Bitmask& mask)
{
	return std::find_if(m_requirements.begin(), m_requirements.end(), [&mask](const Bitmask& req) { return (req & mask) == req; }) != m_requirements.end();
}

int Sys::getActorCount() const
{
	return m_actorIds.size();
}
