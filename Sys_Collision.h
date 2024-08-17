#pragma once
#include "Sys.h"

class SysManager;
class Level;

class Sys_Collision : public Sys
{
public:
	Sys_Collision(SysManager* systemManager);
	~Sys_Collision();

	void setupRequirements() override;
	void subscribeToChannels() override;
	void unsubscribeFromChannels() override;

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void setLevel(Level* level);
private:
	void levelCollisions();
	void actorOnLevelCollisions(const ActorId& actorId, Comp_Position* posComp, Comp_Collision* colComp);
	void actorCollisions();

	Level* m_level;
};