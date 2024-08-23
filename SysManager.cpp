#include "SysManager.h"
#include "ActorManager.h"
#include "WindowManager.h"

SysManager::SysManager() :
	m_actorManager(nullptr)
{
	m_systems[SystemType::Renderer] = new Sys_Renderer(this);
	m_systems[SystemType::Control] = new Sys_Control(this);
	m_systems[SystemType::Movement] = new Sys_Movement(this);
	m_systems[SystemType::Collision] = new Sys_Collision(this);
	m_systems[SystemType::Bounds] = new Sys_Bounds(this);
	m_systems[SystemType::AIController] = new Sys_AIController(this);
	m_systems[SystemType::BulletSpawner] = new Sys_BulletSpawner(this);
}

SysManager::~SysManager()
{
	purgeSystems();
}

void SysManager::start()
{
	for (auto& sys : m_systems)
		sys.second->start();
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
	for (auto& sys : m_systems)
		sys.second->debugOverlay(windowManager);
}

void SysManager::handleEvents()
{
	for (auto& eventQueue : m_actorEvents)
	{
		EventId eventId = 0;
		while (eventQueue.second.size() > 0)
		{
			eventId = eventQueue.second.front();
			eventQueue.second.pop();
			for (auto& sys : m_systems)
				sys.second->handleEvent(eventQueue.first, (ActorEventType)eventId);
		}
	}
}

void SysManager::addEvent(ActorId actorId, ActorEvent eventId)
{
	m_actorEvents[actorId].push(eventId);
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
