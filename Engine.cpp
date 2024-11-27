#include "Engine.h"

Engine::Engine() :
	m_windowManager("Space Invaders", sf::Vector2u(1920, 1080)),
	m_stateManager(&m_context),
	m_soundManager(&m_audioManager),
	m_actorManager(&m_systemManager, &m_textureManager, &m_soundManager)
{
	m_clock.restart();

	// set up context
	m_context.m_windowManager = &m_windowManager;
	m_context.m_controller = m_windowManager.getController();
	m_context.m_actorManager = &m_actorManager;
	m_systemManager.setActorManager(&m_actorManager);
	m_context.m_systemManager = &m_systemManager;
	m_context.m_soundManager = &m_soundManager;

	// initialize state manager
	m_stateManager.switchTo(StateType::Intro);
	// set cursor invisible
	m_windowManager.getRenderWindow()->setMouseCursorVisible(false);
	// create texture to draw to
	m_windowTexture.create(m_windowManager.getRenderWindow()->getSize().x, m_windowManager.getRenderWindow()->getSize().y);
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
	m_windowManager.clear();
	m_stateManager.draw();
	afterEffects();
	m_windowManager.display();
}

void Engine::afterEffects()
{
	sf::RenderWindow& window = *m_windowManager.getRenderWindow();
	m_windowTexture.update(window);
	window.clear();
	
	sf::Sprite sprite(m_windowTexture);
	window.draw(sprite);
	addRenderLines();
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

void Engine::addRenderLines()
{
	// after everything is drawn, draw the render lines
	sf::RenderWindow* window = m_windowManager.getRenderWindow();
	// vertical lines
	for (unsigned int x = 0; x < window->getSize().x; x += 3)
	{
		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(static_cast<float>(x), 0.f), sf::Color(0, 0, 0, 50)),
			sf::Vertex(sf::Vector2f(static_cast<float>(x), static_cast<float>(window->getSize().y)), sf::Color(0, 0, 0, 50))
		};
		window->draw(line, 2, sf::Lines);
	}
	// horizontal lines
	for (unsigned int y = 0; y < window->getSize().y; y += 3)
	{
		sf::Vertex line[] =
		{
			sf::Vertex(sf::Vector2f(0, static_cast<float>(y)), sf::Color(0, 0, 0, 100)),
			sf::Vertex(sf::Vector2f(static_cast<float>(window->getSize().x), static_cast<float>(y)), sf::Color(0, 0, 0, 100))
		};
		window->draw(line, 2, sf::Lines);
	}
}
