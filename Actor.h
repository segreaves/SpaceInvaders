#pragma once
#include "Comp.h"
#include "ECS_Params.h"
#include <unordered_map>

class Actor
{
public:
	void onCreate();
	void onDestroy();

	void update(const float& deltaTime);

	void addComponent(Comp* component, const Bitmask& mask);
	void removeComponent(const Bitmask& mask);
	void removeAllComponents();
private:
	std::unordered_map<Bitmask, Comp*> m_components;
};
