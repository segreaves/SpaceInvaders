#pragma once
#include "Comp.h"

/// <summary>
/// Allows the Sys_Gravity system to apply gravity to an actor based on its mass.
/// </summary>
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