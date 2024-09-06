#pragma once
#include <SFML/System/Vector2.hpp>
#include "Comp.h"

/// <summary>
/// Class used to store the control components of an actor.
/// This class is accessed by the controls system to move the actor.
/// </summary>
class Comp_Control : public Comp
{
public:
	void load(std::stringstream& ss) override
	{
		ss >> m_maxSpeed;
		ss >> m_maxAcceleration;
	}

	void setMovementInput(const sf::Vector2f& movementInput)
	{
		m_movementInput = m_movementDirection = movementInput;
		float magnitude = sqrt(pow(m_movementDirection.x, 2) + pow(m_movementDirection.y, 2));
		if (magnitude > 1)
		{
			m_movementDirection.x /= magnitude;
			m_movementDirection.y /= magnitude;
		}
	}

	const sf::Vector2f& getMovementInput() const { return m_movementInput; }

	const sf::Vector2f& getMovementDirection() const { return m_movementDirection; }

	void setMaxAcceleration(const float& maxAcceleration)
	{
		m_maxAcceleration = maxAcceleration;
	}

	float getMaxAcceleration() const { return m_maxAcceleration; }

	void setMaxSpeed(const float& maxSpeed)
	{
		m_maxSpeed = maxSpeed;
	}

	float getMaxSpeed() const { return m_maxSpeed; }
private:
	sf::Vector2f m_movementInput;
	sf::Vector2f m_movementDirection;
	float m_maxAcceleration;
	float m_maxSpeed;
};