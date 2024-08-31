#pragma once
#include "Sys.h"

class SysManager;

class Sys_BunkerDegradation : public Sys
{
	public:
	Sys_BunkerDegradation(SysManager* systemManager);
	~Sys_BunkerDegradation();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
};