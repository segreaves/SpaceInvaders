#pragma once
#include <SFML/System/Vector2.hpp>
#include "Comp.h"

class Comp_Target : public Comp
{
public:
	sf::Vector2f getTarget() { return m_target; }
	void setTarget(sf::Vector2f target) { m_target = target; }
private:
	void load(std::stringstream& ss) override
	{
	}
	sf::Vector2f m_target;
};