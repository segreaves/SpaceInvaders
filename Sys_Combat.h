#pragma once
#include "Sys.h"
#include <SFML/System/Vector2.hpp>

class SysManager;

class Sys_Combat : public Sys
{
public:
	Sys_Combat(SysManager* systemManager);
	~Sys_Combat();

	void setupRequirements() override;
	void subscribeToChannels() override;
	void unsubscribeFromChannels() override;

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	bool addBullet(Actor* bullet);
private:
	void shoot(const ActorId& actorId, sf::Vector2f direction);
	
	unsigned int m_currentBullet;
	std::vector<Actor*> m_bullets;
};