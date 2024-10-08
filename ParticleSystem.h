#pragma once
#include <SFML/Graphics.hpp>
#include "Particle.h"

class ParticleSystem
{
public:
	void initialize();
	void draw(sf::RenderWindow* window);
	void emitParticles();
	void update(float deltaTime);
	bool isEnabled() { return m_isEnabled; }

	void setNumParticles(const int& numParticles) { m_numParticles = numParticles; }
	void setDuration(float duration) { m_duration = duration; }
	void setEmitterPosition(sf::Vector2f position) { m_emitterPosition = position; }
	void setMinSpeed(float speed) { m_minSpeed = speed; }
	void setParticleSize(float size) { m_particleSize = size; }
private:
	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	unsigned int m_numParticles;
	float m_particleSize;
	float m_duration = 1.f;
	float m_minSpeed = 50.f;
	sf::Vector2f m_emitterPosition;
	bool m_isEnabled = false;
};