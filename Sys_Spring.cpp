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
	req.set((unsigned int)ComponentType::Target);
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
		const auto& actor = m_systemManager->getActorManager()->getActor(id);
		const auto& springComp = actor->getComponent<Comp_Spring>(ComponentType::Spring);
		const auto& moveComp = actor->getComponent<Comp_Movement>(ComponentType::Movement);
		const auto& posComp = actor->getComponent<Comp_Position>(ComponentType::Position);
		const auto& targetComp = actor->getComponent<Comp_Target>(ComponentType::Target);

		// save the previous attach->anchor vector
		const sf::Vector2f prevOffset = springComp->getAnchor() - springComp->getAttach();
		// set the spring attach and anchor
		springComp->setAttach(posComp->getPosition());
		springComp->setAnchor(targetComp->getTarget());

		// compute the spring vector
		const sf::Vector2f springForce = calculateSpringForce(springComp->getAnchor(), springComp->getAttach(), moveComp->getVelocity(), springComp->getStrength(), springComp->getLength(), springComp->getDampingCoeff());
		// apply the spring force to the actor
		moveComp->accelerate(springForce * deltaTime);
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

		// draw small circle at the attach point
		sf::CircleShape circle(2);
		circle.setFillColor(sf::Color::Yellow);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		circle.setPosition(springComp->getAttach());
		windowManager->getRenderWindow()->draw(circle);

		// draw the spring
		sf::Vertex line[] =
		{
			sf::Vertex(springComp->getAnchor(), sf::Color::Yellow),
			sf::Vertex(springComp->getAttach(), sf::Color::Yellow)
		};
		windowManager->getRenderWindow()->draw(line, 2, sf::Lines);
	}
}

void Sys_Spring::notify(const Message& msg)
{
}

sf::Vector2f Sys_Spring::calculateSpringForce(const sf::Vector2f& anchor, const sf::Vector2f& pos, const sf::Vector2f& vel, const float& strength, const float& length, const float& dampingCoeff)
{
	if (pos == anchor) return sf::Vector2f(0, 0);
	// compute the spring vector
	const sf::Vector2f displacement = pos - anchor;
	// compute the distance from the anchor
	const float currentLength = sqrt(displacement.x * displacement.x + displacement.y * displacement.y);
	// normalize the spring vector
	const sf::Vector2f dir = displacement / currentLength;
	// stretch of the spring
	float stretch = currentLength - length;
	// calculate force
	sf::Vector2f force = -strength * stretch * dir;
	// apply damping force
	force -= dampingCoeff * vel;
	return force;
}
