#pragma once
#include "Sys.h"

class SysManager;

class Sys_Movement : public Sys
{
public:
	Sys_Movement(SysManager* systemManager);

	void setupRequirements();
	void subscribeToChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEvent& msg);

	void notify(const Message& msg);

	void move(const ActorId& actorId, const float& deltaTime);
};