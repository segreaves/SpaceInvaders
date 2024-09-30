#pragma once
#include "Comp.h"

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
