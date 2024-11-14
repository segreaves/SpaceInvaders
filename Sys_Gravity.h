#pragma once
#include "Sys.h"

class SysManager;

class Sys_Gravity : public Sys
{
public:
	Sys_Gravity(SysManager* systemManager);
	~Sys_Gravity();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
};