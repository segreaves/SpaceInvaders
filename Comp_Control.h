#pragma once
#include <SFML/System/Vector2.hpp>
#include "Comp.h"

class Comp_Control : public Comp
{
public:
	void setMovementInput(const sf::Vector2f& movementInput)
	{
		m_movementInput = movementInput;
		float magnitude = sqrt(pow(movementInput.x, 2) + pow(movementInput.y, 2));
		if (magnitude > 1)
		{
			m_movementInput.x /= magnitude;
			m_movementInput.y /= magnitude;
		}
	}

	const sf::Vector2f& getMovementInput() const
	{
		return m_movementInput;
	}
private:
	sf::Vector2f m_movementInput;
};