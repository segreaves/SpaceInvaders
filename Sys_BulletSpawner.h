#pragma once
#include "Sys.h"

class SysManager;

class Sys_BulletSpawner : public Sys
{
public:
	Sys_BulletSpawner(SysManager* systemManager);
	~Sys_BulletSpawner();

	void setupRequirements() override;
	void subscribeToChannels() override;
	void unsubscribeFromChannels() override;

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
private:
	unsigned int m_currentBullet;
};
