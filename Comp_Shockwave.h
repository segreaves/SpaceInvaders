#pragma once
#include "Comp.h"

class Comp_Shockwave : public Comp
{
public:
	float getForce() const { return force; }
private:
	void load(std::stringstream& ss) override
	{
		ss >> force;
	}

	float force;
};