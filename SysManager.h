#pragma once
#include "SystemType.h"
#include "Sys.h"
#include "Sys_Renderer.h"
#include "Sys_Movement.h"
#include "Sys_Rotation.h"
#include "Sys_PlayerControl.h"
#include "Sys_Collision.h"
#include "Sys_InvaderControl.h"
#include "Sys_BulletControl.h"
#include "Sys_BunkerControl.h"
#include "Sys_ShockwaveControl.h"
#include "Sys_Health.h"
#include "Sys_LevelState.h"
#include "Sys_Sound.h"
#include "Sys_Spring.h"
#include "Sys_Gravity.h"
#include "Sys_ShipSway.h"
#include "Sys_TorqueSpring.h"
#include "MessageHandler.h"
#include <map>
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

	void start();
	void update(const float& deltaTime);
	void draw(WindowManager* windowManager);
	void handleEvents();
	void addEvent(ActorId actorId, ActorEvent eventId);
	
	void actorModified(const ActorId& actorId, const Bitmask& mask);
	void removeActor(const ActorId& actorId);

	void setActorManager(ActorManager* actorManager) { m_actorManager = actorManager; }
	ActorManager* getActorManager() { return m_actorManager; }
	MessageHandler* getMessageHandler();
	template<class T>
	T* getSystem(const SystemType& type)
	{
		auto sys = m_systems.find(type);
		return sys != m_systems.end() ? dynamic_cast<T*>(sys->second) : nullptr;
	}

	void purgeSystems();
	void purgeActors();

	void setLevelManager(LevelManager* levelManager) { m_levelManager = levelManager; }
	LevelManager* getLevelManager() { return m_levelManager; }
private:
	ActorManager* m_actorManager;
	std::map<SystemType, Sys*> m_systems;
	std::unordered_map<ActorId, std::queue<EventId>> m_actorEvents;
	MessageHandler m_messageHandler;
	LevelManager* m_levelManager;
	std::vector<SystemType> m_systemOrder;
};