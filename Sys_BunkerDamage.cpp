#include "Sys_BunkerDamage.h"
#include "SysManager.h"

Sys_BunkerDamage::Sys_BunkerDamage(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_BunkerDamage::~Sys_BunkerDamage()
{
	unsubscribeFromChannels();
}

void Sys_BunkerDamage::start()
{
}

void Sys_BunkerDamage::setupRequirements()
{
	m_requirements.set((unsigned int)ComponentType::Position);
	m_requirements.set((unsigned int)ComponentType::Collision);
	m_requirements.set((unsigned int)ComponentType::Sprite);
	m_requirements.set((unsigned int)ComponentType::Bunker);
}

void Sys_BunkerDamage::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_BunkerDamage::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_BunkerDamage::update(const float& deltaTime)
{
}

void Sys_BunkerDamage::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_BunkerDamage::debugOverlay(WindowManager* windowManager)
{
}

void Sys_BunkerDamage::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	switch ((ActorMessageType)msg.m_type)
	{
	case ActorMessageType::Collision:
		handleBunkerDamage(msg.m_receiver, msg.m_sender);
		break;
	}
}

void Sys_BunkerDamage::handleBunkerDamage(const ActorId& actorId, const ActorId& otherId)
{
	sf::FloatRect collider = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Collision>(ComponentType::Collision)->getAABB();
	Comp_SpriteSheet* spriteComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
	// get the texture of the sprite
	sf::Texture* texture = const_cast<sf::Texture*>(spriteComp->getSpriteSheet()->getTexture());
	if (!texture) return;
	// get the image of the texture
	sf::Image image = texture->copyToImage();
	// get the sprite's rectangle in global coordinates
	sf::FloatRect spriteRect = spriteComp->getSpriteSheet()->getSprite()->getGlobalBounds();
	// get the intersection of the sprite's rectangle and the collider
	sf::FloatRect intersect;
	if (!spriteRect.intersects(collider, intersect)) return;
	// get the intersection in relation to the sprite's rectangle
	sf::FloatRect intersection(intersect.left - spriteRect.left, intersect.top - spriteRect.top, intersect.width, intersect.height);
	//intersection.left -= spriteRect.left;
	//intersection.top -= spriteRect.top;

	// get the intersection start position in relation to the texture
	unsigned int xStart = intersection.left / spriteComp->getSpriteSheet()->getSpriteScale().x;
	unsigned int yStart = intersection.top / spriteComp->getSpriteSheet()->getSpriteScale().y;
	// get the intersection width and height in relation to the texture
	unsigned int width = intersection.width / spriteRect.width * texture->getSize().x;
	unsigned int height = intersection.height / spriteRect.height * texture->getSize().y;

	// iterate over pixels in intersection and determine if any are not transparent
	sf::Vector2i pixelCollisionCoord = pixelCollision(xStart, width, yStart, height, image);
	if (pixelCollisionCoord != sf::Vector2i(-1, -1))
	{
		std::cout << "Collision at: " << pixelCollisionCoord.x << ", " << pixelCollisionCoord.y << std::endl;
		// main explosion
		// create a FloatRect of pixels to turn off. This rectangle must be centered on the original bullet position on x and on the collision pixel on y
		Comp_Position* posComp = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Position>(ComponentType::Position);
		int explosionSize = 8;
		sf::FloatRect explosionRect = sf::FloatRect(posComp->getPosition().x - explosionSize / 2, posComp->getPosition().y - explosionSize / 2, explosionSize, explosionSize);

		// get the intersection of the sprite's rectangle and the explosion rectangle
		sf::FloatRect explosionIntersection;
		if (!spriteRect.intersects(explosionRect, explosionIntersection)) return;
		// get the intersection in relation to the sprite's rectangle
		explosionIntersection.left -= spriteRect.left;
		explosionIntersection.top -= spriteRect.top;
		// get the intersection start position in relation to the texture
		unsigned int explosionXStart = explosionIntersection.left / spriteComp->getSpriteSheet()->getSpriteScale().x;
		unsigned int explosionYStart = explosionIntersection.top / spriteComp->getSpriteSheet()->getSpriteScale().y;
		// get the intersection width and height in relation to the texture
		unsigned int explosionWidth = explosionIntersection.width / spriteRect.width * texture->getSize().x;
		unsigned int explosionHeight = explosionIntersection.height / spriteRect.height * texture->getSize().y;
		// iterate over the pixels in the intersection
		turnOffPixels(explosionXStart, explosionWidth, explosionYStart, explosionHeight, image);
		// update the texture with the modified image
		texture->update(image);
		// message bullet of collision
		m_systemManager->addEvent(otherId, (EventId)ActorEventType::Despawned);

		/*// iterate over the pixels in the intersection
		turnOffPixels(xStart, width, yStart, height, image);
		m_systemManager->addEvent(msg.m_sender, (EventId)ActorEventType::Despawned);
		// update the texture with the modified image
		texture->update(image);*/
	}
}

sf::Vector2i Sys_BunkerDamage::pixelCollision(unsigned int xStart, unsigned int width, unsigned int yStart, unsigned int height, sf::Image& image)
{
	sf::Vector2i collisionPixel(-1, -1);
	for (unsigned int x = xStart; x <= xStart + width; x++)
		for (unsigned int y = yStart; y <= yStart + height; y++)
			if (image.getPixel(x, y).a > 0)
			{
				collisionPixel = sf::Vector2i(x, y);
				return collisionPixel;
			}
	return collisionPixel;
}

void Sys_BunkerDamage::turnOffPixels(unsigned int xStart, unsigned int width, unsigned int yStart, unsigned int height, sf::Image& image)
{
	for (unsigned int x = xStart; x <= xStart + width; x++)
		for (unsigned int y = yStart; y <= yStart + height; y++)
			if (image.getPixel(x, y).a > 0)// if pixel is not transparent
				image.setPixel(x, y, sf::Color(255, 255, 255, 0));// set pixel to transparent
}
