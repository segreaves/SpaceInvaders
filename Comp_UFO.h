#pragma once
#include "Comp.h"

class Comp_UFO : public Comp
{
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_points;
	}
	int m_points;
};