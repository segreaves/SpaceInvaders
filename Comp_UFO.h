#pragma once
#include "Comp.h"

/// <summary>
/// Identifies an actor as a UFO and records the points it is worth.
/// </summary>
class Comp_UFO : public Comp
{
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_points;
	}
	int m_points;
};