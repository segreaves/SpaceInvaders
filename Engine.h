#pragma once
#include "Context.h"
#include "WindowManager.h"
#include "StateManager.h"
#include "SysManager.h"
#include "ActorManager.h"
#include "TextureManager.h"

class Engine
{
public:
	Engine();

	void update();
	void lateUpdate();
	void render();
	void run();

private:
	sf::Clock m_clock;
	sf::Time m_elapsed;
	Context m_context;
	WindowManager m_windowManager;
	StateManager m_stateManager;
	SysManager m_systemManager;
	ActorManager m_actorManager;
	TextureManager m_textureManager;
};
