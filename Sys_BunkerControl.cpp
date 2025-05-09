#include "Sys_BunkerControl.h"
#include "SysManager.h"
#include "WindowManager.h"
#include "SoundType.h"
#include <functional>
#include <cmath>
#include <numbers>

Sys_BunkerControl::Sys_BunkerControl(SysManager* systemManager) :
	Sys(systemManager),
	m_rows(0),
	m_cols(0),
	m_bunkerDamaged(false),
	m_damageTimer(0),
	m_damageDuration(0.05f),
	m_gen(m_rd()), // seed rng
	m_unifAngleDist(0, 360), // angle distribution
	m_unifLengthDist(0, 10) // length distribution
{
	onCreate();
}

Sys_BunkerControl::~Sys_BunkerControl()
{
	onDestroy();
}

void Sys_BunkerControl::start()
{
}

void Sys_BunkerControl::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Collision);
	req.set((unsigned int)ComponentType::Bunker);
	req.set((unsigned int)ComponentType::Grid);
	m_requirements.emplace_back(req);
}

void Sys_BunkerControl::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_BunkerControl::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_BunkerControl::update(const float& deltaTime)
{
	if (m_damageTimer > 0)
		m_damageTimer -= deltaTime;
	else
		m_bunkerDamaged = false;
}

void Sys_BunkerControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_BunkerControl::debugOverlay(WindowManager* windowManager)
{
}

void Sys_BunkerControl::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	switch ((ActorMessageType)msg.m_type)
	{
	case ActorMessageType::Collision:
		damageBunker(msg.m_receiver, msg.m_sender);
		break;
	}
}

void Sys_BunkerControl::damageBunker(const ActorId& actorId, const ActorId& otherId)
{
	sf::FloatRect collider = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Collision>(ComponentType::Collision)->getAABB();
	auto gridComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Grid>(ComponentType::Grid);
	SpriteSheet* spriteSheet = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet)->getSpriteSheet();
	sf::Texture* texture = const_cast<sf::Texture*>(spriteSheet->getTexture());
	if (!texture) return;
	sf::Image image = texture->copyToImage();
	sf::Sprite* bunkerSprite = const_cast<sf::Sprite*>(spriteSheet->getSprite());

	sf::FloatRect intersect;
	sf::FloatRect spriteGlobalRect = bunkerSprite->getGlobalBounds();
	// get the intersection of the other actor's collider and the bunker sprite
	if (!spriteGlobalRect.intersects(collider, intersect)) return;
	
	sf::FloatRect localIntersect = spriteSheet->globalRectToPixelRect(intersect);

	// convert the local coordinates to pixel coordinates
	sf::Vector2i start = sf::Vector2i(
		static_cast<int>(localIntersect.left),
		static_cast<int>(localIntersect.top));
	sf::Vector2i end = sf::Vector2i(
		static_cast<int>(std::ceil(localIntersect.left + localIntersect.width)),
		static_cast<int>(std::ceil(localIntersect.top + localIntersect.height)));

	// check for collision against non-transparent pixels. If true, instantiate damage
	if (nonTransparentPixels(start.x, end.x, start.y, end.y, image))
	{
		// turn off pixels in the overlap between the two actors
		turnOffPixels(start.x, end.x, start.y, end.y, image);

		// position at which to initiate explosion
		sf::Vector2f rayOrigin = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Position>(ComponentType::Position)->getPosition();
		const unsigned int rayNum = 20;
		for (int i = 0; i < rayNum; i++)
		{
			// create a ray in a random direction
			const auto angle = m_unifAngleDist(m_gen);
			const auto length = m_unifLengthDist(m_gen);
			sf::Vector2f direction(cos(angle), sin(angle));
			// normalize direction
			direction /= std::sqrt(direction.x * direction.x + direction.y * direction.y);
			Ray ray(rayOrigin, direction);
			// get the intersection of the ray with the grid
			std::vector<sf::Vector2i> cellsHit = gridComp->getGrid()->getIntersect(ray, length);
			// disable colliding cells
			for (auto& cell : cellsHit)
			{
				// turn off corresponding pixels in the texture
				sf::FloatRect cellRect = gridComp->getGrid()->getCellRect(cell.x, cell.y);
				sf::FloatRect cellLocalIntersect = spriteSheet->globalRectToPixelRect(cellRect);
				sf::Vector2i cellStart = sf::Vector2i(
					static_cast<int>(cellLocalIntersect.left),
					static_cast<int>(cellLocalIntersect.top));
				sf::Vector2i cellEnd = sf::Vector2i(
					static_cast<unsigned int>(std::ceil(cellLocalIntersect.left + cellLocalIntersect.width)),
					static_cast<unsigned int>(std::ceil(cellLocalIntersect.top + cellLocalIntersect.height)));
				turnOffPixels(cellStart.x, cellEnd.x, cellStart.y, cellEnd.y, image);
			}
		}

		texture->update(image);

		// message other actor of collision
		Message msg((MessageType)ActorMessageType::Collision);
		msg.m_sender = actorId;
		msg.m_receiver = otherId;
		m_systemManager->getMessageHandler()->dispatch(msg);

		// if the bunker has not been damaged, set the damage flag and start the damage timer
		if (!m_bunkerDamaged)
		{
			m_bunkerDamaged = true;
			m_damageTimer = m_damageDuration;
			// play bunker damage sound
			Message soundMsg((MessageType)ActorMessageType::Sound);
			soundMsg.m_sender = msg.m_receiver;
			soundMsg.m_receiver = msg.m_receiver;
			soundMsg.m_int = static_cast<int>(SoundType::BunkerHit);
			soundMsg.m_xy = XY(100.f, 1.f);
			m_systemManager->getMessageHandler()->dispatch(soundMsg);
		}
	}
}

bool Sys_BunkerControl::nonTransparentPixels(unsigned int xStart, unsigned int xEnd, unsigned int yStart, unsigned int yEnd, sf::Image& image)
{
	for (unsigned int x = xStart; x < xEnd; x++)
		for (unsigned int y = yStart; y < yEnd; y++)
			if (x >= 0 && x < image.getSize().x && y >= 0 && y < image.getSize().y && image.getPixel(x, y).a > 0)
				return true;
	return false;
}

void Sys_BunkerControl::turnOffPixels(unsigned int xStart, unsigned int xEnd, unsigned int yStart, unsigned int yEnd, sf::Image& image)
{
	for (unsigned int x = xStart; x < xEnd; x++)
		for (unsigned int y = yStart; y < yEnd; y++)
			if (x >= 0 && x < image.getSize().x && y >= 0 && y < image.getSize().y)
				image.setPixel(x, y, sf::Color::Transparent);
}
