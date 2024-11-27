#pragma once
#include <SFML/Graphics.hpp>
#include "Particle.h"
#include <random>

class ParticleSystem
{
public:
	ParticleSystem();

	void initialize();
	void draw(sf::RenderWindow* window);
	void emitParticles();
	void update(float deltaTime);
	bool isEnabled() { return m_isEnabled; }

	void setNumParticles(const int& numParticles) { m_numParticles = numParticles; }
	void setDuration(float duration) { m_duration = duration; }
	void setEmitterPosition(sf::Vector2f position) { m_emitterPosition = position; }
	void setMinSpeed(float speed) { m_minSpeed = speed; }
	void setMaxSpeed(float speed) { m_maxSpeed = speed; }
	void setParticleSize(float size) { m_particleSize = size; }
private:
	unsigned int m_numParticles;
	std::vector<Particle> m_particles;
	sf::VertexArray m_vertices;
	float m_particleSize;
	float m_timer;
	float m_duration;
	float m_minSpeed;
	float m_maxSpeed;
	sf::Vector2f m_emitterPosition;
	bool m_isEnabled;
	std::mt19937 m_rng;
	std::random_device m_rd;
	std::uniform_real_distribution<float> m_angleDist;
	std::uniform_real_distribution<float> m_speedDist;
};