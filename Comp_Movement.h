#pragma once
#include <SFML/System/Vector2.hpp>
#include "Comp.h"
#include <cmath>

class Comp_Movement : public Comp
{
public:
	const sf::Vector2f& getVelocity() const { return m_velocity; }

	void setVelocity(const sf::Vector2f& velocity)
	{
		m_velocity = velocity;
	}

	void setVelocity(const float& velocityX, const float& velocityY)
	{
		m_velocity.x = velocityX;
		m_velocity.y = velocityY;
	}

	void addVelocity(const sf::Vector2f& velocity)
	{
		m_velocity += velocity;
	}

	void updatePrevVelocity()
	{
		m_prevVelocity = m_velocity;
	}

	const sf::Vector2f& getAcceleration() const { return m_acceleration; }

	void setAcceleration(const sf::Vector2f& acceleration)
	{
		m_acceleration = acceleration;
	}

	void setAcceleration(const float& accelerationX, const float& accelerationY)
	{
		m_acceleration = sf::Vector2f(accelerationX, accelerationY);
	}

	void accelerate(const sf::Vector2f& acceleration)
	{
		m_acceleration += acceleration;
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

	void setCollidingOnX(const bool& collidingOnX)
	{
		m_collidingOnX = collidingOnX;
	}

	const bool& getCollidingOnX() const { return m_collidingOnX; }

	void setCollidingOnY(const bool& collidingOnY)
	{
		m_collidingOnY = collidingOnY;
	}

	const bool& getCollidingOnY() const { return m_collidingOnY; }

	void resetCollisionFlags()
	{
		m_collidingOnX = false;
		m_collidingOnY = false;
	}
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_frictionCoefficient;
	}

	float m_frictionCoefficient = 100.f;
	sf::Vector2f m_prevVelocity;
	sf::Vector2f m_velocity;
	sf::Vector2f m_acceleration;
	bool m_collidingOnX = false;
	bool m_collidingOnY = false;
};