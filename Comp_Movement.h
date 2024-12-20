#pragma once
#include <SFML/System/Vector2.hpp>
#include "Comp.h"
#include <cmath>

/// <summary>
/// Records an actors movement speed and acceleration.
/// </summary>
class Comp_Movement : public Comp
{
public:
	const sf::Vector2f& getVelocity() const { return m_velocity; }
	void setVelocity(const sf::Vector2f& velocity) { m_velocity = velocity; }
	void setVelocity(const float& velocityX, const float& velocityY)
	{
		m_velocity.x = velocityX;
		m_velocity.y = velocityY;
	}
	void addVelocity(const sf::Vector2f& velocity)
	{
		m_velocity += velocity;
	}

	sf::Vector2f getPrevVelocity() { return m_prevVelocity; }
	void setPrevVelocity(const sf::Vector2f& prevVelocity) { m_prevVelocity = prevVelocity; }
	void setPrevVelocity(const float& x, const float& y) { m_prevVelocity = sf::Vector2f(x, y); }
	void updateSpeedChange()
	{
		m_speedChange = m_velocity - m_prevVelocity;
		m_prevVelocity = m_velocity;
	}
	sf::Vector2f getSpeedChange() { return m_speedChange; }
	void setSpeedChange(const sf::Vector2f& speedChange) { m_speedChange = speedChange; }
	void setSpeedChange(const float& x, const float& y) { m_speedChange = sf::Vector2f(x, y); }

	const sf::Vector2f& getAcceleration() const { return m_acceleration; }
	void setAcceleration(const sf::Vector2f& acceleration)
	{
		m_acceleration = acceleration;
	}
	void setAcceleration(const float& accelerationX, const float& accelerationY) { m_acceleration = sf::Vector2f(accelerationX, accelerationY); }
	void accelerate(const sf::Vector2f& acceleration)
	{
		m_acceleration += acceleration;
	}

	const float& getAngularVelocity() const { return m_angularVelocity; }
	void setAngularVelocity(const float& angularVelocity) { m_angularVelocity = angularVelocity; }
	void addAngularVelocity(const float& angularVelocity) { m_angularVelocity += angularVelocity; }
	const float& getTorque() const { return m_torque; }
	void setTorque(const float& torque) { m_torque = torque; }
	void addTorque(const float& torque) { m_torque += torque; }
	float getTorque() { return m_torque; }

	const float& getFrictionCoefficient() const { return m_frictionCoefficient; }

	void setFrictionCoefficient(const float& frictionCoefficient)
	{
		m_frictionCoefficient = frictionCoefficient;
	}

	void applyAngularDampening(const float& angularVelocity)
	{
		m_angularVelocity -= m_angularFrictionCoefficient * angularVelocity;
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
		ss >> m_frictionCoefficient >> m_angularFrictionCoefficient;
	}

	float m_frictionCoefficient = 0.f;
	float m_angularFrictionCoefficient = 0.f;
	sf::Vector2f m_velocity = sf::Vector2f(0.f, 0.f);
	sf::Vector2f m_prevVelocity = sf::Vector2f(0.f, 0.f);
	sf::Vector2f m_acceleration = sf::Vector2f(0.f, 0.f);
	sf::Vector2f m_speedChange = sf::Vector2f(0.f, 0.f);
	float m_angularVelocity;
	float m_torque;
	bool m_collidingOnX = false;
	bool m_collidingOnY = false;
};