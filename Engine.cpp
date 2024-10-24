#include "Engine.h"

Engine::Engine() :
	m_windowManager("Space Invaders", sf::Vector2u(1920, 1080)),
	m_stateManager(&m_context),
	m_actorManager(&m_systemManager, &m_textureManager)
{
	m_clock.restart();

	m_context.m_windowManager = &m_windowManager;
	m_context.m_controller = m_windowManager.getController();
	m_context.m_actorManager = &m_actorManager;
	m_systemManager.setActorManager(&m_actorManager);
	m_context.m_systemManager = &m_systemManager;

	m_stateManager.switchTo(StateType::Intro);

	m_windowManager.getRenderWindow()->setMouseCursorVisible(false);
}

void Engine::update()
{
	float dt = m_elapsed.asSeconds();
	m_windowManager.update(dt);
	m_stateManager.update(dt);
}

void Engine::lateUpdate()
{
	m_stateManager.lateUpdate();
	m_elapsed = m_clock.restart();
}

void Engine::render()
{
	m_windowManager.drawStart();
	m_stateManager.draw();
	m_windowManager.drawEnd();
}

void Engine::run()
{
	while (m_windowManager.isOpen())
	{
		update();
		render();
		lateUpdate();
	}
}
