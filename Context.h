#pragma once
#include "WindowManager.h"
#include "Controller.h"
//#include "EntityManager.h"
//#include "SystemManager.h"
#include "ActorManager.h"
#include "SysManager.h"

struct Context
{
	Context() :
		m_windowManager(nullptr),
		m_controller(nullptr),
		m_entityManager(nullptr),
		m_systemManager(nullptr)
	{
	}

	WindowManager* m_windowManager;
	Controller* m_controller;
	//EntityManager* m_entityManager;
	//SystemManager* m_systemManager;
	ActorManager* m_entityManager;
	SysManager* m_systemManager;
};
