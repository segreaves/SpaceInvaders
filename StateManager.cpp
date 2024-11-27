#include "StateManager.h"
#include "State_Intro.h"
#include "State_Game.h"
#include "State_Paused.h"
#include "State_GameOver.h"
#include <SFML/OpenGL.hpp>

StateManager::StateManager(Context* context) :
	m_context(context)
{
	registerState<State_Intro>(StateType::Intro);
	registerState<State_Game>(StateType::Game);
	registerState<State_Paused>(StateType::Paused);
	registerState<State_GameOver>(StateType::GameOver);
}

StateManager::~StateManager()
{
	for (auto& state : m_states)
		delete state.second;
}

void StateManager::update(const float& deltaTime)
{
	if (m_states.empty()) return;
	m_states.back().second->update(deltaTime);
}

void StateManager::draw()
{
	if (m_states.empty()) { return; }
	drawStates();
}

void StateManager::lateUpdate()
{
	processRequests();
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
	m_context->m_soundManager->switchState(toState);
	for (auto it = m_states.begin(); it != m_states.end(); it++)
	{
		if (it->first == toState)
		{
			m_states.back().second->deactivate();
			StateType type = it->first;
			State* state = it->second;
			m_states.erase(it);
			m_states.emplace_back(type, state);
			state->activate();
			return;
		}
	}
	if (!m_states.empty())
		m_states.back().second->deactivate();
	createState(toState);
	m_states.back().second->activate();
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
			delete it->second;
			m_states.erase(it);
			m_context->m_soundManager->removeState(removeState);
			return;
		}
	}
}

void StateManager::drawStates()
{
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
