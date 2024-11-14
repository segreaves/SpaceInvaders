#pragma once
#include "Comp.h"

class Comp_Mass : public Comp
{
public:
	float getMass() const { return m_mass; }
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_mass;
	}
	float m_mass;
};