#pragma once
#include <SFML/Graphics.hpp>

/// <summary>
/// Particle class for the particle system. Holds the position and velocity of a particle.
/// </summary>
class Particle
{
public:
	Particle();
	Particle(sf::Vector2f direction);

	void update(float deltaTime);
	void setPosition(sf::Vector2f position);
	sf::Vector2f getPosition();
private:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};