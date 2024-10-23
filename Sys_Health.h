#pragma once
#include "Sys.h"

class LevelManager;

class Sys_Health : public Sys
{
public:
	Sys_Health(SysManager* systemManager);
	~Sys_Health();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
};