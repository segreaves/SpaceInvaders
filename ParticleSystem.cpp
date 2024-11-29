#include "ParticleSystem.h"
#include "Params.h"
#include <iostream>
#define QUAD_SIZE 4

ParticleSystem::ParticleSystem()
	: m_numParticles(100)
	, m_particleSize(m_numParticles)
	, m_minSpeed(0)
	, m_maxSpeed(1)
	, m_timer(1)
	, m_isEnabled(false)
	, m_emitterPosition(0, 0),
	m_rng(m_rd()),
	m_angleDist(0, 360),
	m_speedDist(0, 1),
	m_duration(1)
{
}

void ParticleSystem::initialize()
{
	m_vertices.clear();
	m_particles.clear();
	m_vertices.setPrimitiveType(sf::Quads);
	m_vertices.resize(m_numParticles * QUAD_SIZE);
	m_particles.resize(m_numParticles);

	for (unsigned int i = 0; i < m_numParticles; i++)
	{
		float angle = m_angleDist(m_rng);
		float speed = m_speedDist(m_rng) * (m_maxSpeed - m_minSpeed) + m_minSpeed;

		sf::Vector2f direction = sf::Vector2f(cos(angle) * speed, sin(angle) * speed);
		m_particles[i] = Particle(direction);
	}
}

void ParticleSystem::draw(sf::RenderWindow* window)
{
	window->draw(m_vertices);
}

void ParticleSystem::emitParticles()
{
	m_isEnabled = true;
	m_timer = m_duration;

	int currentVertex = 0;
	for (auto it = m_particles.begin(); it != m_particles.end(); it++)
	{
		m_vertices[currentVertex].color = APP_COLOR;
		m_vertices[currentVertex + 1].color = APP_COLOR;
		m_vertices[currentVertex + 2].color = APP_COLOR;
		m_vertices[currentVertex + 3].color = APP_COLOR;
		currentVertex += QUAD_SIZE;
		it->setPosition(m_emitterPosition);
	}
}

void ParticleSystem::update(float deltaTime)
{
	m_timer -= deltaTime;
	std::vector<Particle>::iterator it;
	int currentVertex = 0;
	for (it = m_particles.begin(); it != m_particles.end(); it++)
	{
		it->update(deltaTime);
		m_vertices[currentVertex].position     = it->getPosition() + sf::Vector2f(-m_particleSize / 2.f,  m_particleSize / 2.f);
		m_vertices[currentVertex + 1].position = it->getPosition() + sf::Vector2f( m_particleSize / 2.f,  m_particleSize / 2.f);
		m_vertices[currentVertex + 2].position = it->getPosition() + sf::Vector2f( m_particleSize / 2.f, -m_particleSize / 2.f);
		m_vertices[currentVertex + 3].position = it->getPosition() + sf::Vector2f(-m_particleSize / 2.f, -m_particleSize / 2.f);
		currentVertex += QUAD_SIZE;
	}
	if (m_timer <= 0)
		m_isEnabled = false;
}
