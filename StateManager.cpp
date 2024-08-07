#include "StateManager.h"
#include "State_Intro.h"
#include "State_Menu.h"
#include "State_Game.h"
#include <iostream>

StateManager::StateManager(Context* context) :
	m_context(context)
{
	registerState<State_Intro>(StateType::Intro);
	registerState<State_Menu>(StateType::Menu);
	registerState<State_Game>(StateType::Game);
}

StateManager::~StateManager()
{
	for (auto& state : m_states)
	{
		state.second->onDestroy();
		delete state.second;
	}
}

void StateManager::update(const float& deltaTime)
{
	if (m_states.empty()) return;
	m_states.back().second->update(deltaTime);
}

void StateManager::draw()
{
	if (m_states.empty()) { return; }
	if (m_states.back().second->isTransparent() && m_states.size() > 1)
	{
		auto itr = m_states.end();
		while (itr != m_states.begin())
		{
			if (itr != m_states.end())
			{
				if (!itr->second->isTransparent()) break;
			}
			itr--;
		}
		for (; itr != m_states.end(); itr++)
		{
			itr->second->draw();
		}
	}
	else m_states.back().second->draw();
}

void StateManager::processRequests()
{
	if (m_toRemove.empty()) return;
	while (m_toRemove.begin() != m_toRemove.end())
	{
		removeState(*m_toRemove.begin());
		m_toRemove.erase(m_toRemove.begin());
	}
}

Context* StateManager::getContext()
{
	return m_context;
}

bool StateManager::hasState(const StateType& stateType)
{
	for (auto it = m_states.begin(); it < m_states.end(); it++)
	{
		if (it->first == stateType)
		{
			auto removed = std::find(m_toRemove.begin(), m_toRemove.end(), stateType);
			return removed == m_toRemove.end() ? false : true;
		}
	}
	return false;
}

void StateManager::switchTo(const StateType& toState)
{
	for (auto it = m_states.begin(); it != m_states.end(); it++)
	{
		if (it->first == toState)
		{
			m_states.back().second->deactivate();
			StateType itType = it->first;
			State* itState = it->second;
			m_states.erase(it);
			m_states.emplace_back(itType, itState);
			itState->activate();
			m_context->m_inputManager->setCurrentState(toState);
			return;
		}
	}
	if (!m_states.empty()) m_states.back().second->deactivate();
	createState(toState);
	m_states.back().second->activate();
	m_context->m_inputManager->setCurrentState(toState);
}

void StateManager::createState(const StateType& newState)
{
	auto it = m_stateFactory.find(newState);
	if (it == m_stateFactory.end()) return;
	State* state = it->second();
	m_states.emplace_back(newState, state);
	state->onCreate();
}

void StateManager::remove(const StateType& removeState)
{
	m_toRemove.push_back(removeState);
}

void StateManager::removeState(const StateType& removeState)
{
	for (auto it = m_states.begin(); it != m_states.end(); it++)
	{
		if (it->first == removeState)
		{
			//it->second->onDestroy();
			delete it->second;
			m_states.erase(it);
		}
	}
}
