#pragma once
#include "WindowManager.h"
#include "Controller.h"
#include "ActorManager.h"
#include "SysManager.h"
#include "LevelManager.h"

/// <summary>
/// Allows systems in the game to communicate in a controlled manner.
/// </summary>
struct Context
{
	Context() :
		m_windowManager(nullptr),
		m_controller(nullptr),
		m_actorManager(nullptr),
		m_systemManager(nullptr),
		m_textureManager(nullptr),
		m_soundManager(nullptr)
	{
	}

	WindowManager* m_windowManager;
	Controller* m_controller;
	ActorManager* m_actorManager;
	SysManager* m_systemManager;
	TextureManager* m_textureManager;
	SoundManager* m_soundManager;
};
