#pragma once
#include "Comp.h"
#include "IDrawable.h"
#include "ParticleSystem.h"

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
		float numParticles, particleSize, duration, minSpeed;
		ss >> numParticles >> particleSize >> duration >> minSpeed;
		m_particleSystem.setNumParticles(numParticles);
		m_particleSystem.setDuration(duration);
		m_particleSystem.setMinSpeed(minSpeed);
		m_particleSystem.setParticleSize(particleSize);
	}
	ParticleSystem m_particleSystem;
};