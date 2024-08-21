#pragma once
#include "Sys.h"

class SysManager;

class Sys_Control : public Sys
{
public:
	Sys_Control(SysManager* systemManager);
	~Sys_Control();

	void setupRequirements() override;
	void subscribeToChannels() override;
	void unsubscribeFromChannels() override;

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
};