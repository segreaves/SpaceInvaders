#pragma once
#include <SFML/Graphics.hpp>

class Particle
{
public:
	Particle(sf::Vector2f direction);

	void update(float deltaTime);
	void setPosition(sf::Vector2f position);
	sf::Vector2f getPosition();
private:
	sf::Vector2f m_position;
	sf::Vector2f m_velocity;
};