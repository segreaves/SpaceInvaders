#pragma once
#include "SystemType.h"
#include "Sys.h"
#include "Sys_Renderer.h"
#include "Sys_Movement.h"
#include "Sys_Control.h"
#include "MessageHandler.h"
#include "CompType.h"
#include <unordered_map>
#include <queue>

class ActorManager;
class WindowManager;

using ActorId = unsigned int;
using EventId = unsigned int;

/// <summary>
/// Responsible for managing all systems in the game.
/// Updates all systems, draws all systems, and handles events.
/// </summary>
class SysManager
{
public:
	SysManager();
	~SysManager();

	void update(const float& deltaTime);
	void draw(WindowManager* windowManager);
	void handleEvents();
	
	void actorModified(const ActorId& actorId, const Bitmask& mask);
	void removeActor(const ActorId& actorId);
	void removeAllActors();

	void setActorManager(ActorManager* actorManager);
	ActorManager* getActorManager();
	MessageHandler* getMessageHandler();
	template<class T>
	T* getSystem(CompType type)
	{
		auto& sys = m_systems.find(type);
		return sys != m_systems.end() ? dynamic_cast<T*>(sys->second) : nullptr;
	}

	void purgeSystems();
private:
	ActorManager* m_actorManager;
	std::unordered_map<SystemType, Sys*> m_systems;
	std::unordered_map<ActorId, std::queue<EventId>> m_actorEvents;
	MessageHandler m_messageHandler;
};