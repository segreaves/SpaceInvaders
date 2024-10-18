#pragma once
#include "Sys.h"

class SysManager;

/// <summary>
/// Handles collision between actors.
/// </summary>
class Sys_Collision : public Sys
{
public:
	Sys_Collision(SysManager* systemManager);
	~Sys_Collision();

	bool addActor(const ActorId& actorId) override;
	bool removeActor(const ActorId& actorId) override;

	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);
private:
	void actorCollisions();
	void detectCollisions();
	bool detectActorCollision(const ActorId& actorId, const ActorId& otherActor);

	std::unordered_map<std::string, std::vector<ActorId>> m_actorGroups;

#ifdef DEBUG
	std::vector<sf::RectangleShape> m_intersects;
#endif
};