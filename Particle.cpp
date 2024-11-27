#include "Particle.h"

Particle::Particle()
{
	m_velocity = sf::Vector2f();
}

Particle::Particle(sf::Vector2f direction)
{
	m_velocity = direction;
}

void Particle::update(float deltaTime)
{
	m_position += m_velocity * deltaTime;
}

void Particle::setPosition(sf::Vector2f position)
{
	m_position = position;
}

sf::Vector2f Particle::getPosition()
{
	return m_position;
}
