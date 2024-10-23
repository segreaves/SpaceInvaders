#include "Sys.h"
#include "SysManager.h"
#include "Message.h"

Sys::Sys(SysManager* systemManager) :
	m_systemManager(systemManager)
{
}

Sys::~Sys()
{
}

void Sys::onCreate()
{
	m_requirements = std::vector<Bitmask>();
	setupRequirements();
	subscribeToChannels();
}

void Sys::onDestroy()
{
	unsubscribeFromChannels();
	purge();
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

void Sys::purge()
{
	m_actorIds.clear();
}
