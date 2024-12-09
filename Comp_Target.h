#pragma once
#include <SFML/System/Vector2.hpp>
#include "Comp.h"

/// <summary>
/// Defines the desired target position of an actor.
/// </summary>
class Comp_Target : public Comp
{
public:
	sf::Vector2f getTarget() { return m_target; }
	void setTarget(sf::Vector2f target) { m_target = target; }
	void setTarget(const float& x, const float& y) { m_target = sf::Vector2f(x, y); }
private:
	void load(std::stringstream& ss) override
	{
	}
	sf::Vector2f m_target;
};