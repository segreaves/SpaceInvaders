#pragma once
#include "Sys.h"
#include "Trigger.h"

class SysManager;
class LevelManager;

/// <summary>
/// This system handles most of the game logic related to the player in terms of
/// movement direction, shooting, death.
/// </summary>
class Sys_PlayerControl : public Sys
{
public:
	Sys_PlayerControl(SysManager* systemManager);
	~Sys_PlayerControl();
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
	void onPlayerDestroyed(ActorId id);

	unsigned int m_playerBulletIndex;
	sf::Font m_font;
	sf::Text m_bulletCountText;
	const float m_knockback = 250000;
};