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
	void setMovementInput(const sf::Vector2f& movementInput)
	{
		m_movementInput = movementInput;
	}

	const sf::Vector2f& getMovementInput() const { return m_movementInput; }

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
	void load(std::stringstream& ss) override
	{
		ss >> m_maxSpeed >> m_maxAcceleration;
	}

	sf::Vector2f m_movementInput;
	float m_maxAcceleration;
	float m_maxSpeed;
};