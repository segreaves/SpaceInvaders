#include "Sys_BunkerControl.h"
#include "SysManager.h"
#include "WindowManager.h"
#include <functional>
#include <cmath>
#include <numbers>

Sys_BunkerControl::Sys_BunkerControl(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();

	m_damageTexture.loadFromFile(Utils::getWorkingDirectory() + "assets/graphics/crack.png");
	m_damageSprite.setTexture(m_damageTexture);
	m_damageSprite.setScale(1.25f, 1.25f);
	m_damageSprite.setOrigin(m_damageSprite.getLocalBounds().width / 2, m_damageSprite.getLocalBounds().height / 2);
}

Sys_BunkerControl::~Sys_BunkerControl()
{
	unsubscribeFromChannels();
}

void Sys_BunkerControl::start()
{
}

void Sys_BunkerControl::setupRequirements()
{
	Bitmask req;
	req.set((unsigned int)ComponentType::Position);
	req.set((unsigned int)ComponentType::Collision);
	req.set((unsigned int)ComponentType::SpriteSheet);
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
}

void Sys_BunkerControl::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_BunkerControl::debugOverlay(WindowManager* windowManager)
{
	//drawGrids(windowManager);
#ifdef DEBUG
	for (auto& rect : m_bulletsIntersects)
		windowManager->getRenderWindow()->draw(rect);
	for (auto& rect : m_bulletTips)
		windowManager->getRenderWindow()->draw(rect);
	for (auto& rect : m_disabledCells)
		windowManager->getRenderWindow()->draw(rect);
	m_bulletsIntersects.clear();
	m_bulletTips.clear();
#endif
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

void Sys_BunkerControl::handleBunkerDamage(const ActorId& actorId, const ActorId& otherId)
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

	// convert intersection to pixel coordinates
	intersection.left /= bunkerSprite->getScale().x;
	intersection.top /= bunkerSprite->getScale().y;
	intersection.width /= bunkerSprite->getScale().x;
	intersection.height /= bunkerSprite->getScale().y;

	unsigned int xStart = intersection.left;
	unsigned int yStart = intersection.top;
	unsigned int width = intersection.width;
	unsigned int height = intersection.height;

	// check for collision against non-transparent pixels
	if (pixelCollision(xStart, width, yStart, height, imageBunker))
	{
		// stamp the damage sprite to the original bullet position
		Comp_Position* posComp = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Position>(ComponentType::Position);
		sf::Vector2f collisionPos(posComp->getPosition().x, posComp->getPosition().y);

		// place the m_damageSprite at the collision position
		m_damageSprite.setPosition(collisionPos);
		
		applyDamageToBunker(bunkerSprite, texture, spriteRect, intersect, imageBunker);
		
		// message other actor of collision
		Message msg((MessageType)ActorMessageType::Collision);
		msg.m_sender = actorId;
		msg.m_receiver = otherId;
		m_systemManager->getMessageHandler()->dispatch(msg);
	}
}

void Sys_BunkerControl::damageBunker(const ActorId& actorId, const ActorId& otherId)
{
	sf::FloatRect collider = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Collision>(ComponentType::Collision)->getAABB();
	Comp_Grid* gridComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Grid>(ComponentType::Grid);
	Comp_Bunker* bunkerComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_Bunker>(ComponentType::Bunker);
	Comp_SpriteSheet* spriteComp = m_systemManager->getActorManager()->getActor(actorId)->getComponent<Comp_SpriteSheet>(ComponentType::SpriteSheet);
	sf::Texture* texture = const_cast<sf::Texture*>(spriteComp->getSpriteSheet()->getTexture());
	if (!texture) return;
	sf::Image image = texture->copyToImage();
	sf::Sprite* bunkerSprite = const_cast<sf::Sprite*>(spriteComp->getSpriteSheet()->getSprite());
	sf::FloatRect spriteRect = bunkerSprite->getGlobalBounds();
	sf::FloatRect intersect;
	// get the intersection of the other actor's collider and the bunker sprite
	if (!spriteRect.intersects(collider, intersect)) return;
	// get the intersection in relation to the sprite
	sf::FloatRect intersection(intersect.left - spriteRect.left, intersect.top - spriteRect.top, intersect.width, intersect.height);
	// convert intersection to pixel coordinates
	intersection.left /= bunkerSprite->getScale().x;
	intersection.top /= bunkerSprite->getScale().y;
	intersection.width /= bunkerSprite->getScale().x;
	intersection.height /= bunkerSprite->getScale().y;

	unsigned int xStart = intersection.left;
	unsigned int yStart = intersection.top;
	unsigned int width = intersection.width;
	unsigned int height = intersection.height;

#ifdef DEBUG
	sf::RectangleShape rectIntersect(sf::Vector2f(intersect.width, intersect.height));
	rectIntersect.setPosition(sf::Vector2f(intersect.left, intersect.top));
	rectIntersect.setFillColor(sf::Color::Yellow);
	m_bulletsIntersects.emplace_back(rectIntersect);
#endif

	// check for collision against non-transparent pixels
	srand(static_cast<unsigned int>(time(nullptr)));// seed the random number generator
	constexpr float PI = 3.14159265358979323846f;
	if (pixelCollision(xStart, width, yStart, height, image))
	{
		// position at which to initiate explosion
		sf::Vector2f collisionPos = m_systemManager->getActorManager()->getActor(otherId)->getComponent<Comp_Position>(ComponentType::Position)->getPosition();
		
#ifdef DEBUG
		sf::RectangleShape rectTip(sf::Vector2f(4.f, 4.f));
		rectTip.setPosition(collisionPos);
		rectTip.setOrigin(rectTip.getSize().x / 2, rectTip.getSize().y / 2);
		rectTip.setFillColor(sf::Color::Red);
		m_bulletTips.emplace_back(rectTip);
#endif
		const unsigned int rayNum = 10;
		for (int i = 0; i < rayNum; i++)
		{
			// create a ray in a random direction
			float angle = 360.f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float length = 10.f * static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			// convert to radians so the direction vector will always be length 1
			float radians = angle * PI / 180.f;
			sf::Vector2f direction(cos(radians), sin(radians));
			Ray ray(collisionPos, direction);
			// get the intersection of the ray with the grid
			std::vector<sf::Vector2i> intersect = gridComp->getGrid()->getIntersect(ray, length);
			// disable colliding cells
			for (auto& cell : intersect)
			{
				if (gridComp->isEnabled(cell))
				{
					gridComp->disableCell(cell);
#ifdef DEBUG
					sf::RectangleShape rect(sf::Vector2f(gridComp->getGrid()->getCellSize(), gridComp->getGrid()->getCellSize()));
					rect.setPosition(gridComp->getGrid()->getCellCoords(cell));
					rect.setFillColor(sf::Color::White);
					m_disabledCells.emplace_back(rect);
#endif
					// turn off corresponding pixels in the texture
					turnOffPixels(cell.x * gridComp->getGrid()->getCellSize(), gridComp->getGrid()->getCellSize(), cell.y * gridComp->getGrid()->getCellSize(), gridComp->getGrid()->getCellSize(), image);
				}
			}
		}

		texture->update(image);

		// message other actor of collision
		Message msg((MessageType)ActorMessageType::Collision);
		msg.m_sender = actorId;
		msg.m_receiver = otherId;
		m_systemManager->getMessageHandler()->dispatch(msg);
	}
}

void Sys_BunkerControl::applyDamageToBunker(sf::Sprite* bunkerSprite, sf::Texture* texture, const sf::FloatRect& spriteRect, const sf::FloatRect& intersect, sf::Image& image)
{
	// rotate the sprite 90 degrees a random number of times
	m_damageSprite.setRotation((rand() % 360));
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
				sf::Color pixelBunker = image.getPixel(x, y);
				sf::Color pixelDamage = imageDamage.getPixel(pixelPos.x, pixelPos.y);

				// If both pixels are non-transparent, modify the pixel in the first image
				if (pixelBunker.a > 0 && pixelDamage.a > 0) {
					// Set the pixel in the first image to transparent
					image.setPixel(x, y, sf::Color(0, 0, 0, 0));
				}
			}
		}
	}
	texture->update(image);
}


bool Sys_BunkerControl::pixelCollision(unsigned int xStart, unsigned int width, unsigned int yStart, unsigned int height, sf::Image& image)
{
	for (unsigned int x = xStart; x <= xStart + width; x++)
		for (unsigned int y = yStart; y <= yStart + height; y++)
			if (x < image.getSize().x && y < image.getSize().y)
				return image.getPixel(x, y).a > 0;
	return false;
}

void Sys_BunkerControl::turnOffPixels(unsigned int xStart, unsigned int width, unsigned int yStart, unsigned int height, sf::Image& image)
{
	for (unsigned int x = xStart; x <= xStart + width; x++)
		for (unsigned int y = yStart; y <= yStart + height; y++)
			if (x < image.getSize().x && y < image.getSize().y)
				image.setPixel(x, y, sf::Color(0, 0, 0, 0));
}

sf::Vector2i Sys_BunkerControl::convertToPixelCoords(const sf::Vector2f& globalCoords, const sf::Sprite* sprite)
{
	sf::Vector2f pos = sprite->getPosition();
	sf::Vector2f scale = sprite->getScale();
	sf::Vector2f origin = sprite->getOrigin();
	sf::Vector2f offset = globalCoords - pos;
	offset.x /= scale.x;
	offset.y /= scale.y;
	offset.x += origin.x;
	offset.y += origin.y;
	return sf::Vector2i(offset);
}

sf::Vector2f Sys_BunkerControl::convertToScreenCoords(const sf::Vector2i& pixelCoords, const sf::Sprite* sprite)
{
	sf::Vector2f pos = sprite->getPosition();
	sf::Vector2f scale = sprite->getScale();
	sf::Vector2f origin = sprite->getOrigin();
	sf::Vector2f offset(pixelCoords);
	offset.x -= origin.x;
	offset.y -= origin.y;
	offset.x *= scale.x;
	offset.y *= scale.y;
	offset += pos;
	return offset;
}

void Sys_BunkerControl::drawCells(sf::RenderWindow* window, Grid* grid)
{
	if (!grid) return;
	sf::Color cellColor = sf::Color::White;
	cellColor.a = 50;
	for (int i = 0; i < grid->rows(); i++)
	{
		for (int j = 0; j < grid->cols(); j++)
		{
			sf::RectangleShape rect(sf::Vector2f(grid->getCellSize(), grid->getCellSize()));
			rect.setPosition(grid->getCellCoords(sf::Vector2i(j, i)));
			rect.setFillColor(cellColor);
			window->draw(rect);
		}
	}
}

void Sys_BunkerControl::drawGrids(WindowManager* windowManager)
{
	for (auto& id : m_actorIds)
	{
		Comp_Grid* gridComp = m_systemManager->getActorManager()->getActor(id)->getComponent<Comp_Grid>(ComponentType::Grid);
		drawCells(windowManager->getRenderWindow(), gridComp->getGrid());
	}
}
