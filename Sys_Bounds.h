#pragma once
#include "Sys.h"

class SysManager;

class Sys_Bounds : public Sys
{
public:
	Sys_Bounds(SysManager* systemManager);
	~Sys_Bounds();

	void setupRequirements() override;
	void subscribeToChannels() override;
	void unsubscribeFromChannels() override;

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void setViewSpace(sf::FloatRect viewSpace);
private:
	sf::FloatRect m_viewSpace;
};