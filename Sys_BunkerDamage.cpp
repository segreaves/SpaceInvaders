#include "Sys_BunkerDamage.h"
#include "SysManager.h"

Sys_BunkerDamage::Sys_BunkerDamage(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();

	m_damageTexture.loadFromFile(Utils::getWorkingDirectory() + "assets/graphics/crack.png");
	m_damageSprite.setTexture(m_damageTexture);
	m_damageSprite.setScale(1.25f, 1.25f);
	m_damageSprite.setOrigin(m_damageSprite.getLocalBounds().width / 2, m_damageSprite.getLocalBounds().height / 2);
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
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Collision);
	req.set((unsigned int)ComponentType::SpriteSheet);
	req.set((unsigned int)ComponentType::Bunker);
	m_requirements.emplace_back(req);
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
	sf::Texture* texture = const_cast<sf::Texture*>(spriteComp->getSpriteSheet()->getTexture());
	if (!texture) return;
	sf::Image imageBunker = texture->copyToImage();
	sf::Sprite* bunkerSprite = const_cast<sf::Sprite*>(spriteComp->getSpriteSheet()->getSprite());
	sf::FloatRect spriteRect = bunkerSprite->getGlobalBounds();
	sf::FloatRect intersect;
	// get the intersection of the other actor's collider and the bunker sprite
	if (!spriteRect.intersects(collider, intersect)) return;
	// get the intersection in relation to the sprite's rectangle
	sf::FloatRect intersection(intersect.left - spriteRect.left, intersect.top - spriteRect.top, intersect.width, intersect.height);

	// get the intersection start position, width, and height in relation to the texture
	unsigned int xStart = intersection.left / bunkerSprite->getScale().x;
	unsigned int yStart = intersection.top / bunkerSprite->getScale().y;
	unsigned int width = intersection.width / spriteRect.width * texture->getSize().x;
	unsigned int height = intersection.height / spriteRect.height * texture->getSize().y;

	// check for collision against non-transparent pixels
	if (pixelCollision(xStart, width, yStart, height, imageBunker))
	{
		// create a FloatRect of pixels to turn off. This rectangle must be centered on the original bullet position on x and on the collision pixel on y
		Comp_Position* posComp = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Position>(ComponentType::Position);
		sf::Vector2f collisionPos(posComp->getPosition().x, posComp->getPosition().y);

		// place the m_damageSprite at the collision position
		m_damageSprite.setPosition(collisionPos);
		// rotate the sprite 90 degrees a random number of times
		m_damageSprite.setRotation(90.f * (rand() % 4));
		// flip the sprite horizontally a random number of times
		if (rand() % 2) m_damageSprite.setScale(-m_damageSprite.getScale().x, m_damageSprite.getScale().y);

		// turn off all pixels that overlap between the two sprites
		const sf::Image& imageDamage = m_damageTexture.copyToImage();

		// get the intersection of the two sprites
		sf::FloatRect spriteOverlap;
		if (!spriteRect.intersects(m_damageSprite.getGlobalBounds(), spriteOverlap)) return;
		// get the intersection in relation to the sprite's rectangle
		sf::FloatRect spriteIntersection(spriteOverlap.left - spriteRect.left, spriteOverlap.top - spriteRect.top, spriteOverlap.width, spriteOverlap.height);

		// get the intersection start position, width, and height in relation to the texture
		unsigned int xImageStart = spriteIntersection.left / bunkerSprite->getScale().x;
		unsigned int yImageStart = spriteIntersection.top / bunkerSprite->getScale().y;
		unsigned int imageWidth = spriteIntersection.width / spriteRect.width * texture->getSize().x;
		unsigned int imageHeight = spriteIntersection.height / spriteRect.height * texture->getSize().y;

		// loop through the overlapping area of both sprites
		for (unsigned int x = xImageStart; x <= xImageStart + imageWidth; x++) {
			for (unsigned int y = yImageStart; y <= yImageStart + imageHeight; y++) {
				// calculate the corresponding coordinates on sprite1
				sf::Vector2f coords = convertToScreenCoords(sf::Vector2i(x, y), bunkerSprite);
				sf::Vector2i pixelPos = convertToPixelCoords(coords, &m_damageSprite);

				// check if the coordinates are within sprite2's bounds
				if (pixelPos.x < imageDamage.getSize().x && pixelPos.y < imageDamage.getSize().y) {
					// Get the pixel color from both images
					sf::Color pixelBunker = imageBunker.getPixel(x, y);
					sf::Color pixelDamage = imageDamage.getPixel(pixelPos.x, pixelPos.y);

					// If both pixels are non-transparent, modify the pixel in the first image
					if (pixelBunker.a > 0 && pixelDamage.a > 0) {
						// Set the pixel in the first image to transparent
						imageBunker.setPixel(x, y, sf::Color(0, 0, 0, 0));
					}
				}
			}
		}
		texture->update(imageBunker);
		
		// message bullet of collision
		Message msg((MessageType)ActorMessageType::Collision);
		msg.m_sender = actorId;
		msg.m_receiver = otherId;
		m_systemManager->getMessageHandler()->dispatch(msg);
	}
}

bool Sys_BunkerDamage::pixelCollision(unsigned int xStart, unsigned int width, unsigned int yStart, unsigned int height, sf::Image& image)
{
	for (unsigned int x = xStart; x <= xStart + width; x++)
		for (unsigned int y = yStart; y <= yStart + height; y++)
			if (image.getPixel(x, y).a > 0)
				return true;
	return false;
}

sf::Vector2i Sys_BunkerDamage::convertToPixelCoords(const sf::Vector2f& coords, const sf::Sprite* sprite)
{
	sf::Vector2f pos = sprite->getPosition();
	sf::Vector2f scale = sprite->getScale();
	sf::Vector2f origin = sprite->getOrigin();
	sf::Vector2f offset = coords - pos;
	offset.x /= scale.x;
	offset.y /= scale.y;
	offset.x += origin.x;
	offset.y += origin.y;
	return sf::Vector2i(offset);
}

sf::Vector2f Sys_BunkerDamage::convertToScreenCoords(const sf::Vector2i& coords, const sf::Sprite* sprite)
{
	sf::Vector2f pos = sprite->getPosition();
	sf::Vector2f scale = sprite->getScale();
	sf::Vector2f origin = sprite->getOrigin();
	sf::Vector2f offset(coords);
	offset.x -= origin.x;
	offset.y -= origin.y;
	offset.x *= scale.x;
	offset.y *= scale.y;
	offset += pos;
	return offset;
}
