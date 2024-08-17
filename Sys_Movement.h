#pragma once
#include "Sys.h"

class SysManager;

class Sys_Movement : public Sys
{
public:
	Sys_Movement(SysManager* systemManager);
	~Sys_Movement();

	void setupRequirements() override;
	void subscribeToChannels() override;
	void unsubscribeFromChannels() override;

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void move(const ActorId& actorId, const float& deltaTime);
};