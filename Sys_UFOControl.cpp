#include "Sys_UFOControl.h"
#include "SysManager.h"
#include "WindowManager.h"
#include "SoundType.h"

Sys_UFOControl::Sys_UFOControl(SysManager* systemManager) :
	Sys(systemManager),
	m_movingRight(false),
	m_gen(m_rd()),
	m_unifFloat(10.f, 30.f),
	m_unifInt(0, 1),
	m_ufoTimer(0),
	m_ufoDuration(0),
	m_startOffset(100, 25),
	m_ufoEnabled(false)
{
	onCreate();
}

Sys_UFOControl::~Sys_UFOControl()
{
	onDestroy();
}

void Sys_UFOControl::start()
{
	initializeTimer();
}

void Sys_UFOControl::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Collision);
	req.set((unsigned int)ComponentType::Movement);
	req.set((unsigned int)ComponentType::Target);
	req.set((unsigned int)ComponentType::UFO);
	req.set((unsigned int)ComponentType::SpriteSheet);
	m_requirements.emplace_back(req);
}

void Sys_UFOControl::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_UFOControl::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_UFOControl::update(const float& deltaTime)
{
	if (m_ufoEnabled)
		handleUFO(deltaTime);
	else
	{
		if (m_ufoTimer > 0)
			m_ufoTimer -= deltaTime;
		else
		{
			initializeUFO();
			initializeTimer();
		}
	}
}

void Sys_UFOControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
	if (!hasActor(actorId)) return;
	switch (eventId)
	{
	case ActorEventType::Despawned:
		onUFODeath(actorId);
		break;
	}
}

void Sys_UFOControl::debugOverlay(WindowManager* windowManager)
{
	ActorManager* actorManager = m_systemManager->getActorManager();
	const auto& actor = actorManager->getActor(m_systemManager->getLevelManager()->getUFOId());
	const auto& targetComp = actor->getComponent<Comp_Target>(ComponentType::Target);
	sf::CircleShape target(2.5f);
	target.setOrigin(target.getRadius(), target.getRadius());
	target.setFillColor(sf::Color::Red);
	target.setPosition(targetComp->getTarget());
	windowManager->getRenderWindow()->draw(target);
}

void Sys_UFOControl::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	switch ((ActorMessageType)msg.m_type)
	{
	case ActorMessageType::Collision:
	{
		auto actor = m_systemManager->getActorManager()->getActor(msg.m_receiver);
		auto other = m_systemManager->getActorManager()->getActor(msg.m_sender);
		if (other->getTag() == "bullet_player")// UFO collided with player bullet, UFO dies
		{
			m_systemManager->addEvent(msg.m_receiver, (EventId)ActorEventType::Despawned);
		}
		break;
	}
	}
}

void Sys_UFOControl::initializeTimer()
{
	m_ufoDuration = m_unifFloat(m_gen);
	m_ufoTimer = m_ufoDuration;
}

void Sys_UFOControl::initializeUFO()
{
	m_movingRight = m_unifInt(m_gen);
	auto ufoId = m_systemManager->getLevelManager()->getUFOId();
	m_systemManager->getActorManager()->enableActor(ufoId);
	auto ufo = m_systemManager->getActorManager()->getActor(ufoId);
	const auto& posComp = ufo->getComponent<Comp_Position>(ComponentType::Position);
	const auto& targetComp = ufo->getComponent<Comp_Target>(ComponentType::Target);
	targetComp->setTarget(m_movingRight ? sf::Vector2f(-m_startOffset.x, m_startOffset.y) : sf::Vector2f(m_systemManager->getLevelManager()->getViewSpace().getSize().x + m_startOffset.x, m_startOffset.y));
	posComp->setPosition(targetComp->getTarget() + sf::Vector2f(0, 50));
	m_ufoEnabled = true;
}

void Sys_UFOControl::handleUFO(const float& deltaTime)
{
	const auto& targetComp = m_systemManager->getActorManager()->getActor(m_systemManager->getLevelManager()->getUFOId())->getComponent<Comp_Target>(ComponentType::Target);
	const auto& colComp = m_systemManager->getActorManager()->getActor(m_systemManager->getLevelManager()->getUFOId())->getComponent<Comp_Collision>(ComponentType::Collision);
	sf::Vector2f updatedPos = targetComp->getTarget() + sf::Vector2f(m_movingRight ? m_ufoSpeed : -m_ufoSpeed, 0) * deltaTime;
	targetComp->setTarget(updatedPos);
	// if UFO is out of bounds, disable it
	if (m_movingRight && updatedPos.x > m_systemManager->getLevelManager()->getViewSpace().getSize().x + colComp->getAABB().width / 2.f ||
		!m_movingRight && updatedPos.x < -colComp->getAABB().width / 2.f)
	{
		m_ufoEnabled = false;
		m_systemManager->getActorManager()->disableActor(m_systemManager->getLevelManager()->getUFOId());
	}
}

void Sys_UFOControl::onUFODeath(const ActorId& id)
{
	// play explosion sound
	Message msg((MessageType)ActorMessageType::Sound);
	msg.m_sender = id;
	msg.m_receiver = id;
	msg.m_int = (int)SoundType::UFOExplode;
	m_systemManager->getMessageHandler()->dispatch(msg);
	// enable UFO explosion particle system
	ActorId explosionId = m_systemManager->getLevelManager()->getUFOExplosionId();
	const auto& explosionPos = m_systemManager->getActorManager()->getActor(explosionId)->getComponent<Comp_Position>(ComponentType::Position);
	const auto& ufoPos = m_systemManager->getActorManager()->getActor(id)->getComponent<Comp_Position>(ComponentType::Position);
	explosionPos->setPosition(ufoPos->getPosition());
	const auto& particlesComp = m_systemManager->getActorManager()->getActor(explosionId)->getComponent<Comp_Particles>(ComponentType::Particles);
	particlesComp->getParticleSystem()->initialize();
	particlesComp->getParticleSystem()->setEmitterPosition(explosionPos->getPosition());
	particlesComp->getParticleSystem()->emitParticles();
	m_systemManager->getActorManager()->enableActor(explosionId);
	// disable UFO
	m_ufoEnabled = false;
	m_systemManager->getActorManager()->disableActor(id);
}
