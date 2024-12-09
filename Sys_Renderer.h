#pragma once
#include "Sys.h"

class SysManager;
class WindowManager;

/// <summary>
/// Renders all drawable actors in the game.
/// </summary>
class Sys_Renderer : public Sys
{
	friend class SysManager;
public:
	Sys_Renderer(SysManager* systemManager);
	~Sys_Renderer();
protected:
	void start();
	void setupRequirements();
	void subscribeToChannels();
	void unsubscribeFromChannels();

	void update(const float& deltaTime);
	void handleEvent(const ActorId& actorId, const ActorEventType& eventId);
	void draw(WindowManager* windowManager);
	void debugOverlay(WindowManager* windowManager);

	void notify(const Message& msg);

	void draw(WindowManager* windowManager, std::shared_ptr<IDrawable> drawable);
private:
	bool m_showDamageOverlay;
	float m_damageOverlayTimer;
	float m_damageOverlayDuration;
	sf::RectangleShape m_damageOverlay;
	sf::Color m_damageCurrColor;
};
