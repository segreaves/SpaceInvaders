#include "SysManager.h"
#include "ActorManager.h"
#include "WindowManager.h"

SysManager::SysManager() :
	m_actorManager(nullptr)
{
	m_systems[SystemType::Renderer] = new Sys_Renderer(this);
}

SysManager::~SysManager()
{
	purgeSystems();
}

void SysManager::update(const float& deltaTime)
{
	for (auto& sys : m_systems)
		sys.second->update(deltaTime);
	handleEvents();
}

void SysManager::draw(WindowManager* windowManager)
{
	auto it = m_systems.find(SystemType::Renderer);
	if (it == m_systems.end()) return;
	Sys_Renderer* rend = (Sys_Renderer*)it->second;
	rend->draw(windowManager);
}

void SysManager::addEvent(const ActorId& actorId, const EventId& eventId)
{
	m_actorEvents[actorId].push(eventId);
}

void SysManager::handleEvents()
{
	for (auto& eventQueue : m_actorEvents)
	{
		EventId eventId = 0;
		while (processActorEvent(eventQueue.second, eventId))
			for (auto& sys : m_systems)
				if (sys.second->hasActor(eventQueue.first))
					sys.second->handleEvent(eventQueue.first, (ActorEvent)eventId);
	}
}

void SysManager::actorModified(const ActorId& actorId, const Bitmask& mask)
{
	for (auto& sys : m_systems)
	{
		if (sys.second->fitsRequirements(mask))
			sys.second->addActor(actorId);
		else
			sys.second->removeActor(actorId);
	}
}

void SysManager::removeActor(const ActorId& actorId)
{
	for (auto& sys : m_systems)
		sys.second->removeActor(actorId);
}

void SysManager::removeAllActors()
{
	for (auto& sys : m_systems)
		sys.second->removeAllActors();
}

bool SysManager::processActorEvent(std::queue<EventId> eventQueue, EventId& eventId)
{
	if (eventQueue.empty()) return false;
	eventId = eventQueue.front();
	eventQueue.pop();
	return true;
}

void SysManager::setActorManager(ActorManager* actorManager)
{
	m_actorManager = actorManager;
}

ActorManager* SysManager::getActorManager()
{
	return m_actorManager;
}

MessageHandler* SysManager::getMessageHandler()
{
	return &m_messageHandler;
}

void SysManager::purgeSystems()
{
	for (auto& sys : m_systems)
		delete sys.second;
	m_systems.clear();
}
