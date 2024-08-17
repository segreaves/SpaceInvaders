#pragma once
#include "ISubscriber.h"
#include "ECS_Params.h"
#include "ActorManager.h"
#include <vector>

struct Message;
class SysManager;
class WindowManager;

using ActorId = unsigned int;
using ActorEvent = unsigned int;

class Sys : public ISubscriber
{
public:
	Sys(SysManager* systemManager);
	~Sys();

	virtual void setupRequirements() {}
	virtual void subscribeToChannels() {}
	virtual void unsubscribeFromChannels() {}

	virtual void update(const float& deltaTime) = 0;
	virtual void debugOverlay(WindowManager* windowManager) = 0;
	virtual void handleEvent(const ActorId& actorId, const ActorEventType& eventId) = 0;

	bool addActor(const ActorId& actor);
	bool removeActor(const ActorId& actor);
	void removeAllActors();
	bool hasActor(const ActorId& actor);
	bool fitsRequirements(const Bitmask& mask);
protected:
	Bitmask m_requirements;
	std::vector<ActorId> m_actorIds;
	SysManager* m_systemManager;
};
