#pragma once
#include "Sys.h"
#include <SFML/System/Vector2.hpp>

class SysManager;

class Sys_BulletControl : public Sys
{
public:
	Sys_BulletControl(SysManager* systemManager);
	~Sys_BulletControl();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void setViewSpace(sf::FloatRect viewSpace);

	bool addBullet(Actor* bullet);
private:
	void shoot(const ActorId& actorId, sf::Vector2f direction);
	void incrementBullet();
	
	sf::FloatRect m_viewSpace;
	unsigned int m_currentBullet;
	std::vector<Actor*> m_bullets;
};