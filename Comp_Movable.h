#pragma once
#include <SFML/System/Vector2.hpp>
#include "Comp.h"
#include <cmath>

class Comp_Movable : public Comp
{
public:
	const float& getMaxSpeed() const { return m_maxSpeed; }

	void setMaxSpeed(const float& maxSpeed)
	{
		m_maxSpeed = maxSpeed;
	}

	const sf::Vector2f& getVelocity() const { return m_velocity; }

	void setVelocity(const sf::Vector2f& velocity)
	{
		m_velocity = velocity;
	}

	void addVelocity(const sf::Vector2f& velocity)
	{
		m_velocity += velocity;
	}

	const sf::Vector2f& getAcceleration() const { return m_acceleration; }

	void setAcceleration(const sf::Vector2f& acceleration)
	{
		m_acceleration = acceleration;
	}

	void setAcceleration(const float& accelerationX, const float& accelerationY)
	{
		m_acceleration.x = accelerationX;
		m_acceleration.y = accelerationY;
	}

	void accelerate(const sf::Vector2f& movementInput)
	{
		m_acceleration = movementInput * m_maxSpeed;
	}

	const float& getFrictionCoefficient() const { return m_frictionCoefficient; }

	void setFrictionCoefficient(const float& frictionCoefficient)
	{
		m_frictionCoefficient = frictionCoefficient;
	}

	void applyBaseFriction(const sf::Vector2f& velocity)
	{
		if (sqrt(pow(m_velocity.x, 2) + pow(m_velocity.y, 2)) == 0) return;
		sf::Vector2f friction = m_frictionCoefficient * velocity;
		m_velocity = m_velocity - friction;
	}
private:
	float m_maxSpeed = 10000.f;
	float m_frictionCoefficient = 25.f;
	sf::Vector2f m_velocity;
	sf::Vector2f m_acceleration;
};