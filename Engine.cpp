#include "Engine.h"

Engine::Engine() :
	m_windowManager("Space Invaders", sf::Vector2u(800, 600)),
	m_stateManager(&m_context),
	m_entityManager(&m_systemManager)
{
	m_clock.restart();
	srand(time(nullptr));

	m_context.m_windowManager = &m_windowManager;
	m_context.m_inputManager = m_windowManager.getInputManager();
	m_context.m_entityManager = &m_entityManager;

	m_stateManager.switchTo(StateType::Intro);
}

void Engine::update()
{
	float deltaTime = m_clock.getElapsedTime().asSeconds();
	m_windowManager.update();
	m_stateManager.update(deltaTime);
}

void Engine::lateUpdate()
{
	m_clock.restart();
}

void Engine::render()
{
	float deltaTime = m_clock.getElapsedTime().asSeconds();
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