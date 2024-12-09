#pragma once
#include "Comp.h"
#include "IDrawable.h"
#include "ParticleSystem.h"

/// <summary>
/// Allows the creation of a particle system actor. Used for the player
/// and UFO explosions.
/// </summary>
class Comp_Particles : public Comp, public IDrawable
{
public:

	void updatePosition(const sf::Vector2f& position)
	{
		m_particleSystem.setEmitterPosition(position);
	}

	void draw(sf::RenderWindow* window)
	{
		m_particleSystem.draw(window);
	}

	ParticleSystem* getParticleSystem() { return &m_particleSystem; }
private:
	void load(std::stringstream& ss) override
	{
		int numParticles;
		float particleSize, duration, minSpeed, maxSpeed;
		ss >> numParticles >> particleSize >> duration >> minSpeed >> maxSpeed;
		m_particleSystem.setNumParticles(numParticles);
		m_particleSystem.setDuration(duration);
		m_particleSystem.setMinSpeed(minSpeed);
		m_particleSystem.setMaxSpeed(maxSpeed);
		m_particleSystem.setParticleSize(particleSize);
	}
	ParticleSystem m_particleSystem;
};