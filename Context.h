#pragma once
#include "WindowManager.h"
#include "Controller.h"
#include "ActorManager.h"
#include "SysManager.h"
#include "Level.h"

struct Context
{
	Context() :
		m_windowManager(nullptr),
		m_controller(nullptr),
		m_actorManager(nullptr),
		m_systemManager(nullptr),
		m_level(nullptr)
	{
	}

	WindowManager* m_windowManager;
	Controller* m_controller;
	ActorManager* m_actorManager;
	SysManager* m_systemManager;
	Level* m_level;
};
