#pragma once
#include <SFML/Graphics.hpp>
#include "Comp.h"

class Comp_Rotation : public Comp
{
public:
private:
	void load(std::stringstream& ss) override
	{
	}

	float m_rotation;
};