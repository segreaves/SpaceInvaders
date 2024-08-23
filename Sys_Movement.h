#pragma once
#include "Sys.h"

class SysManager;

class Sys_Movement : public Sys
{
public:
	Sys_Movement(SysManager* systemManager);
	~Sys_Movement();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void move(const ActorId& actorId, const float& deltaTime);
};