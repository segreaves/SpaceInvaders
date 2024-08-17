#pragma once
#include "Sys.h"

class Sys_AIController : public Sys
{
public:
	Sys_AIController(SysManager* systemManager);
	~Sys_AIController();

	void setupRequirements() override;
	void subscribeToChannels() override;
	void unsubscribeFromChannels() override;

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void move(const ActorId& actorId, const float& deltaTime);
private:
};
