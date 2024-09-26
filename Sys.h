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

	virtual void setupRequirements() = 0;
	virtual void subscribeToChannels() = 0;
	virtual void unsubscribeFromChannels() = 0;

	virtual void start() = 0;
	virtual void update(const float& deltaTime) = 0;
	virtual void debugOverlay(WindowManager* windowManager) = 0;
	virtual void handleEvent(const ActorId& actorId, const ActorEventType& eventId) = 0;

	virtual bool addActor(const ActorId& actorId);
	virtual bool removeActor(const ActorId& actorId);
	bool hasActor(const ActorId& actorId);
	bool fitsRequirements(const Bitmask& mask);

	int getActorCount() const;
protected:
	Bitmask m_requirements;
	std::vector<ActorId> m_actorIds;
	SysManager* m_systemManager;
};
