#pragma once
#include "Sys.h"

class Sys_AIController : public Sys
{
public:
	Sys_AIController(SysManager* systemManager);
	~Sys_AIController();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
private:
	void move(const ActorId& actorId, const float& deltaTime);
	void setMoveTarget(Comp_AI* aiComp, const float& deltaTime);

	bool m_movingRight;
	float m_targetSpeed = 100.f;
};
