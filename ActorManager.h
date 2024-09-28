#pragma once
#include "Actor.h"
#include "Params.h"
#include "Comp_Position.h"
#include "Comp_Sprite.h"
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
#include "ActorEventType.h"
#include "TextureManager.h"
#include <functional>

class SysManager;

using ActorId = unsigned int;

class ActorManager
{
public:
	ActorManager(SysManager* systemManager, TextureManager* textureManager);
	~ActorManager();

	int createActor(Bitmask components, std::string tag);
	int initializeActor(std::string tag);
	void addComponent(const ActorId& id, ComponentType compType);
	bool destroyActor(ActorId id);
	void destroyAllActors();
	void enableActor(const ActorId& id);
	void enableAllActors();
	void disableActor(const ActorId& id);
	void disableAllActors();

	Actor* getActor(const ActorId& id);
	unsigned int loadActorProfile(const std::string actorName, const std::string tag);

	template<class T>
	void addComponentType(ComponentType compType)
	{
		if (m_componentFactory.size() > MAX_COMPONENTS)
			std::cout << "! Fatal Error: ComponentFactory has reached maximum capacity." << std::endl;
		m_componentFactory[compType] = []()->Comp* { return new T(); };
	}
private:
	unsigned int m_idCounter;
	std::unordered_map<ActorId, Actor*> m_actors;
	std::unordered_map<ComponentType, std::function<Comp* (void)>> m_componentFactory;
	SysManager* m_systemManager;
	TextureManager* m_textureManager;
};
