#pragma once
#include "Context.h"
#include "WindowManager.h"
#include "InputManager.h"
#include "StateManager.h"
#include "SystemManager.h"
#include "EntityManager.h"

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
	Context m_context;
	WindowManager m_windowManager;
	StateManager m_stateManager;
	SystemManager m_systemManager;
	EntityManager m_entityManager;
};
