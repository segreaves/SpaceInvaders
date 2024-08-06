#pragma once
#include "WindowManager.h"
#include "InputManager.h"
#include "EntityManager.h"

struct Context
{
	Context() :
		m_windowManager(nullptr),
		m_inputManager(nullptr),
		m_entityManager(nullptr)
	{
	}

	WindowManager* m_windowManager;
	InputManager* m_inputManager;
	EntityManager* m_entityManager;
};
