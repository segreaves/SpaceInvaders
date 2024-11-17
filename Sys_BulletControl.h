#pragma once
#include "Sys.h"
#include <SFML/System/Vector2.hpp>

class SysManager;
class LevelManager;

class Sys_BulletControl : public Sys
{
public:
	Sys_BulletControl(SysManager* systemManager);
	~Sys_BulletControl();
protected:
	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
private:
	void onBulletDestroyed(ActorId id);

	std::vector<sf::RectangleShape> m_bulletTips;
};