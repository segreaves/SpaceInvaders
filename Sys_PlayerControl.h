#pragma once
#include "Sys.h"

class SysManager;

class Sys_PlayerControl : public Sys
{
public:
	Sys_PlayerControl(SysManager* systemManager);
	~Sys_PlayerControl();

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
	sf::FloatRect m_viewSpace;
};