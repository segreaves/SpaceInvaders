#pragma once
#include "Comp.h"

class Comp_ShipSway : public Comp
{
public:
	void setSwayCoefficient(const float& swayCoefficient) { m_swayCoefficient = swayCoefficient; }
	float getSwayCoefficient() const { return m_swayCoefficient; }
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_swayCoefficient;
	}
	float m_swayCoefficient;
};