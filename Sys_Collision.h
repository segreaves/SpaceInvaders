#pragma once
#include "Sys.h"

class SysManager;

/// <summary>
/// Handles collision between actors.
/// </summary>
class Sys_Collision : public Sys
{
public:
	Sys_Collision(SysManager* systemManager);
	~Sys_Collision();

	void setupRequirements() override;
	void subscribeToChannels() override;
	void unsubscribeFromChannels() override;

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
private:
	void actorCollisions();
};