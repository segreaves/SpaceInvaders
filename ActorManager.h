#pragma once
#include "Actor.h"
#include "Params.h"
#include "Comp_Position.h"
#include "Comp_SpriteSheet.h"
#include "Comp_Control.h"
#include "Comp_Player.h"
#include "Comp_Invader.h"
#include "Comp_Bullet.h"
#include "Comp_Movement.h"
#include "Comp_Collision.h"
#include "Comp_Bunker.h"
#include "Comp_Shockwave.h"
#include "Comp_Target.h"
#include "Comp_Health.h"
#include "Comp_Particles.h"
#include "Comp_Grid.h"
#include "ActorEventType.h"
#include "TextureManager.h"
#include "Trigger.h"
#include <functional>

class SysManager;

using ActorId = unsigned int;

class ActorManager
{
public:
	ActorManager(SysManager* systemManager, TextureManager* textureManager);
	~ActorManager();

	int initializeActor(std::string tag);
	void addComponent(const ActorId& id, ComponentType compType);
	bool destroyActor(ActorId id);
	void purge();
	void enableActor(const ActorId& id);
	void enableAllActors();
	void disableActor(const ActorId& id);
	void disableAllActors();

	std::shared_ptr<Actor> getActor(const ActorId& id);
	std::unordered_map<std::string, std::vector<ActorId>>* getActorGroups() { return &m_actorGroups; }
	unsigned int loadActorProfile(const std::string actorName, const std::string tag);

	template<class T>
	void addComponentType(ComponentType compType)
	{
		if (m_componentFactory.size() > MAX_COMPONENTS)
			std::cout << "! Fatal Error: ComponentFactory has reached maximum capacity." << std::endl;
		m_componentFactory[compType] = []()->std::shared_ptr<Comp> { return std::make_shared<T>(); };
	}

	Trigger<unsigned int> m_actorDisabled;
private:
	unsigned int m_idCounter;
	std::unordered_map<ActorId, std::shared_ptr<Actor>> m_actors;
	std::unordered_map<std::string, std::vector<ActorId>> m_actorGroups;
	std::unordered_map<ComponentType, std::function<std::shared_ptr<Comp>(void)>> m_componentFactory;
	SysManager* m_systemManager;
	TextureManager* m_textureManager;
};
