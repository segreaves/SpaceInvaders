#pragma once
#include "Comp.h"
#include <SFML/System/Vector2.hpp>

class Comp_Invader : public Comp
{
public:
	void setTarget(sf::Vector2f target)
	{
		m_target = target;
	}

	sf::Vector2f getTarget() { return m_target; }
private:
	void load(std::stringstream& ss) override
	{
	}

	sf::Vector2f m_target;
};