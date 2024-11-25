#pragma once
#include "Context.h"
#include "WindowManager.h"
#include "StateManager.h"
#include "SysManager.h"
#include "ActorManager.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "SoundManager.h"

class Engine
{
public:
	Engine();

	void update();
	void lateUpdate();
	void render();
	void afterEffects();
	void run();

private:
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
};
