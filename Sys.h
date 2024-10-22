#pragma once
#include "ISubscriber.h"
#include "Params.h"
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
	virtual ~Sys();

	virtual void setupRequirements() = 0;
	virtual void subscribeToChannels() = 0;
	virtual void unsubscribeFromChannels() = 0;

	virtual void start() = 0;
	virtual void update(const float& deltaTime) = 0;
	virtual void debugOverlay(WindowManager* windowManager) = 0;
	virtual void handleEvent(const ActorId& actorId, const ActorEventType& eventId) = 0;

	void onCreate();
	void onDestroy();

	virtual bool addActor(const ActorId& actorId);
	virtual bool removeActor(const ActorId& actorId);
	int getActorCount() const;
	bool hasActor(const ActorId& actorId);
	bool fitsRequirements(const Bitmask& mask);

	void purge();
protected:
	std::vector<Bitmask> m_requirements;
	std::vector<ActorId> m_actorIds;
	SysManager* m_systemManager;
};
