#pragma once
#include "Context.h"
#include "State.h"
#include "StateType.h"
#include <vector>
#include <unordered_map>

class StateManager
{
public:
	StateManager(Context* context);
	~StateManager();

	void update(const float& deltaTime);
	void draw();
	void lateUpdate();

	void processRequests();

	Context* getContext();
	bool hasState(const StateType& stateType);
	void switchTo(const StateType& toState);
	void remove(const StateType& removeState);
private:
	void createState(const StateType& newState);
	void removeState(const StateType& removeState);
	void drawStates();

	template<class T>
	void registerState(const StateType& stateType)
	{
		m_stateFactory[stateType] = [this]() -> State* { return new T(this); };
	}

	Context* m_context;
	std::vector<std::pair<StateType, State*>> m_states;
	std::vector<StateType> m_toRemove;
	std::unordered_map<StateType, std::function<State* (void)>> m_stateFactory;
};
