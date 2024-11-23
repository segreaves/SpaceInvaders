#pragma once
#include "Sys.h"

class SysManager;

/// <summary>
/// Handles collision between actors. Only tests specific groups of actors.
/// </summary>
class Sys_Collision : public Sys
{
public:
	Sys_Collision(SysManager* systemManager);
	~Sys_Collision();

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
private:
	void detectCollisions();
	bool detectActorCollision(const ActorId& actorId, const ActorId& otherActor);

#ifdef DEBUG
	std::vector<sf::RectangleShape> m_intersects;
#endif
};