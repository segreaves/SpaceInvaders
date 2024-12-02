#include "Engine.h"
#include "Params.h"

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
	m_context.m_textureManager = &m_textureManager;
	m_context.m_soundManager = &m_soundManager;

	// initialize state manager
	m_stateManager.switchTo(StateType::Intro);
	// set cursor invisible
	m_windowManager.getRenderWindow()->setMouseCursorVisible(false);
	// create texture to draw to
	m_windowTexture.create(m_windowManager.getRenderWindow()->getSize().x, m_windowManager.getRenderWindow()->getSize().y);
	// load shaders
	if (!m_brightnessShader.loadFromFile(Utils::getWorkingDirectory() + "assets/shaders/brightness.frag", sf::Shader::Fragment))
		std::cerr << "Error: Engine() Failed to load brightness shader" << std::endl;
	if (!m_blurShader.loadFromFile(Utils::getWorkingDirectory() + "assets/shaders/blur.frag", sf::Shader::Fragment))
		std::cerr << "Error: Engine() Failed to load blur shader" << std::endl;
	sf::Vector2u windowSize = m_windowManager.getRenderWindow()->getSize();
	m_brightnessTexture.create(windowSize.x, windowSize.y);
	m_blurTexture1.create(windowSize.x, windowSize.y);
	m_blurTexture2.create(windowSize.x, windowSize.y);
	m_blurShader.setUniform("textureSize", sf::Vector2f(windowSize));
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
	addBloom();
	addRenderLines();
}

void Engine::addBloom()
{
	sf::RenderWindow& window = *m_windowManager.getRenderWindow();
	sf::Vector2u windowSize = window.getSize();

	// Update the render texture with the current window content
	m_windowTexture.update(window);

	// Extract bright areas
	m_brightnessTexture.clear();
	sf::Sprite brightnessSprite(m_windowTexture);
	m_brightnessShader.setUniform("threshold", 0.5f);
	m_brightnessTexture.draw(brightnessSprite, &m_brightnessShader);
	m_brightnessTexture.display();

	// Apply horizontal blur
	m_blurTexture1.clear();
	sf::Sprite horizontalBlurSprite(m_brightnessTexture.getTexture());
	m_blurShader.setUniform("direction", sf::Vector2f(2.f, 0.0f));
	m_blurTexture1.draw(horizontalBlurSprite, &m_blurShader);
	m_blurTexture1.display();

	// Apply vertical blur
	m_blurTexture2.clear();
	sf::Sprite verticalBlurSprite(m_blurTexture1.getTexture());
	m_blurShader.setUniform("direction", sf::Vector2f(0.0f, 0.5f));
	m_blurTexture2.draw(verticalBlurSprite, &m_blurShader);
	m_blurTexture2.display();

	// Combine original scene with bloom
	window.clear();
	sf::Sprite finalSprite(m_windowTexture);
	window.draw(finalSprite);

	sf::Sprite bloomSprite(m_blurTexture2.getTexture());
	sf::RenderStates blendAdd(sf::BlendAdd);
	window.draw(bloomSprite, blendAdd);
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
	sf::Vector2f windowSize = sf::Vector2f(window->getSize());
	sf::Color color = sf::Color::Black;
	color.a = 150;
	float thickness = 2.f;
	// horizontal render lines
	for (unsigned int y = 0; y < window->getSize().y; y += thickness * 2)
	{
		sf::Vector2f start = sf::Vector2f(0, static_cast<float>(y));
		sf::Vector2f end = sf::Vector2f(static_cast<float>(window->getSize().x), static_cast<float>(y));
		sf::VertexArray render_line = createRenderLine(start, end, thickness, color);
		window->draw(render_line);
	}
}

sf::VertexArray Engine::createRenderLine(const sf::Vector2f& start, const sf::Vector2f& end, float thickness, sf::Color color)
{
	sf::VertexArray vertices(sf::Quads, 4);
	vertices[0].position = start;
	vertices[1].position = end;
	vertices[2].position = end + sf::Vector2f(0, thickness);
	vertices[3].position = start + sf::Vector2f(0, thickness);
	for (int i = 0; i < 4; i++)
		vertices[i].color = color;
	return vertices;
}
