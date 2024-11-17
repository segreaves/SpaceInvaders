#pragma once
#include "Sys.h"

class SysManager;

/// <summary>
/// Handles collision between actors. Splits actors into groups and checks for collisions between them.
/// On actor added to the system, it is added to a group based on its actor tag.
/// </summary>
class Sys_Collision : public Sys
{
public:
	Sys_Collision(SysManager* systemManager);
	~Sys_Collision();
protected:
	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void onActorAdded(const ActorId& actorId) override;
private:
	void detectCollisions();
	bool detectActorCollision(const ActorId& actorId, const ActorId& otherActor);

	std::unordered_map<std::string, std::vector<ActorId>> m_actorGroups;

#ifdef DEBUG
	std::vector<sf::RectangleShape> m_intersects;
#endif
};