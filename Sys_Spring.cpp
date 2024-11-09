#include "Sys_Spring.h"
#include "SysManager.h"
#include "WindowManager.h"

Sys_Spring::Sys_Spring(SysManager* systemManager) :
	Sys(systemManager)
{
	onCreate();
}

Sys_Spring::~Sys_Spring()
{
	onDestroy();
}

void Sys_Spring::start()
{
}

void Sys_Spring::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Movement);
	req.set((unsigned int)ComponentType::Spring);
	m_requirements.emplace_back(req);
}

void Sys_Spring::subscribeToChannels()
{
}

void Sys_Spring::unsubscribeFromChannels()
{
}

void Sys_Spring::update(const float& deltaTime)
{
	for (auto& id : m_actorIds)
	{
		auto actor = m_systemManager->getActorManager()->getActor(id);
		auto springComp = actor->getComponent<Comp_Spring>(ComponentType::Spring);
		auto moveComp = actor->getComponent<Comp_Movement>(ComponentType::Movement);
		auto posComp = actor->getComponent<Comp_Position>(ComponentType::Position);

		// compute the spring vector
		const sf::Vector2f springForce = calculateSpringForce(springComp->getAnchor(), posComp->getPosition(), moveComp->getVelocity(), springComp->getStrength(), springComp->getLength(), springComp->getDampingCoeff());
		
		// rotate the anchor point by the actor's angle and move to actor's position
		sf::Vector2f attach = springComp->getAttachPos(posComp->getPosition(), posComp->getAngleDegrees());
		// calculate the angle from the attach point to the anchor
		const float targetAngle = atan2(springComp->getAnchor().y - attach.y, springComp->getAnchor().x - attach.x);
		// compute the spring torque
		const float springTorque = calculateSpringTorque(posComp->getAngleDegrees(), targetAngle, moveComp->getAngularVelocity(), springComp->getStrength());

		// apply the spring force to the actor
		moveComp->accelerate(springForce);
		moveComp->addTorque(springTorque);
	}
}

void Sys_Spring::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_Spring::debugOverlay(WindowManager* windowManager)
{
	for (auto& id : m_actorIds)
	{
		auto actor = m_systemManager->getActorManager()->getActor(id);
		auto springComp = actor->getComponent<Comp_Spring>(ComponentType::Spring);
		auto posComp = actor->getComponent<Comp_Position>(ComponentType::Position);

		// draw the spring
		sf::Vertex line[] =
		{
			sf::Vertex(springComp->getAnchor(), sf::Color::Red),
			sf::Vertex(springComp->getAttachPos(posComp->getPosition(), posComp->getAngleDegrees()), sf::Color::Red)
		};
		windowManager->getRenderWindow()->draw(line, 2, sf::Lines);
	}
}

void Sys_Spring::notify(const Message& msg)
{
}

sf::Vector2f Sys_Spring::calculateSpringForce(const sf::Vector2f& anchor, const sf::Vector2f& pos, const sf::Vector2f& vel, const float& strength, const float& length, const float& dampingCoeff)
{
	// compute the spring vector
	const sf::Vector2f springVector = pos - anchor;
	// compute the distance from the anchor
	const float distance = sqrt(springVector.x * springVector.x + springVector.y * springVector.y);
	// normalize the spring vector
	const sf::Vector2f v = springVector / distance;
	// dot product between the spring vector and the velocity
	const float damping = (vel.x * v.x + vel.y * v.y) * dampingCoeff;
	// compute the spring force
	const float force = strength * (length - distance) - damping;
	// apply the force along the spring's vector
	const sf::Vector2f springForce = v * force;
	return springForce;
}

float Sys_Spring::calculateSpringTorque(const float& currentAngle, const float& targetAngle, const float& angularVelocity, const float& strength)
{
	// compute the angular displacement
	float angleDisplacement = targetAngle - currentAngle;

	// normalize the angle to the range [-pi, pi]
	while (angleDisplacement > M_PI) angleDisplacement -= 2 * M_PI;
	while (angleDisplacement < -M_PI) angleDisplacement += 2 * M_PI;

	// compute the damping torque
	float dampingTorque = angularVelocity;

	// compute the spring torque
	float springTorque = strength * angleDisplacement - dampingTorque;

	return springTorque;
}
