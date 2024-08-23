#pragma once
#include "Sys.h"

class SysManager;

class Sys_Bounds : public Sys
{
public:
	Sys_Bounds(SysManager* systemManager);
	~Sys_Bounds();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void setViewSpace(sf::FloatRect viewSpace);
private:
	void trackPlayerPosition(Actor* player);
	void trackInvadersPosition(Actor* invader);
	void trackBulletsPosition(Actor* bullet);
	void selectTrackedInvaders();

	sf::FloatRect m_viewSpace;
	ActorId m_leftInvader;
	ActorId m_rightInvader;
};