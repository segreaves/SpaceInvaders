#pragma once
#include "Sys.h"

class SysManager;

class Sys_Movement : public Sys
{
public:
	Sys_Movement(SysManager* systemManager);
	~Sys_Movement();

	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void start();
	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void handleMovement(std::shared_ptr<Comp_Position> posComp, std::shared_ptr<Comp_Movement> moveComp, const float& deltaTime);
	void handleRotation(std::shared_ptr<Comp_Position> posComp, std::shared_ptr<Comp_Movement> moveComp, const float& deltaTime);
};