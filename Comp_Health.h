#pragma once
#include "Comp.h"

/// <summary>
/// Records the health of an actor.
/// </summary>
class Comp_Health : public Comp
{
public:
	int getHealth() { return m_health; }
	int takeDamage() { return --m_health; }
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_health;
	}

	int m_health;
};
