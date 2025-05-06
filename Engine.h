#pragma once
#include "Context.h"
#include "WindowManager.h"
#include "StateManager.h"
#include "SysManager.h"
#include "ActorManager.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "SoundManager.h"

/// <summary>
/// Runs the game loop.
/// </summary>
class Engine
{
public:
	Engine();

	void update();
	void lateUpdate();
	void render();
	void run();
private:
	void afterEffects();
	void addBloom();
	void addRenderLines();
	sf::VertexArray createRenderLine(const sf::Vector2f& start, const sf::Vector2f& end, int thickness, sf::Color color);

	sf::Clock m_clock;
	sf::Time m_elapsed;
	// shared context used for communication between components
	Context m_context;
	// managers
	WindowManager m_windowManager;
	StateManager m_stateManager;
	SysManager m_systemManager;
	TextureManager m_textureManager;
	AudioManager m_audioManager;
	SoundManager m_soundManager;
	ActorManager m_actorManager;
	// graphics
	sf::Texture m_windowTexture;
	// shaders
	sf::Shader m_brightnessShader;
	sf::Shader m_blurShader;
	sf::RenderTexture m_brightnessTexture;
	sf::RenderTexture m_blurTexture1;
	sf::RenderTexture m_blurTexture2;
};
