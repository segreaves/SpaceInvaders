#pragma once
#include "Sys.h"
#include "Trigger.h"

class Sys_AIControl : public Sys
{
public:
	Sys_AIControl(SysManager* systemManager);
	~Sys_AIControl();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void setViewSpace(sf::FloatRect viewSpace);

	Trigger<void> m_invadersDefeated;
private:
	void selectTrackedInvaders();

	sf::FloatRect m_viewSpace;
	bool m_movingRight;
	float m_targetSpeed = 100.f;
	ActorId m_leftInvader;
	ActorId m_rightInvader;
};
