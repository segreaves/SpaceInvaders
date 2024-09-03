#include "Sys_BunkerDegradation.h"
#include "SysManager.h"

Sys_BunkerDegradation::Sys_BunkerDegradation(SysManager* systemManager) :
	Sys(systemManager)
{
	setupRequirements();
	subscribeToChannels();
}

Sys_BunkerDegradation::~Sys_BunkerDegradation()
{
	unsubscribeFromChannels();
}

void Sys_BunkerDegradation::start()
{
}

void Sys_BunkerDegradation::setupRequirements()
{
	m_requirements.set((unsigned int)ComponentType::Position);
	m_requirements.set((unsigned int)ComponentType::Collision);
	m_requirements.set((unsigned int)ComponentType::Sprite);
	m_requirements.set((unsigned int)ComponentType::Bunker);
}

void Sys_BunkerDegradation::subscribeToChannels()
{
	m_systemManager->getMessageHandler()->subscribe(ActorMessageType::Collision, this);
}

void Sys_BunkerDegradation::unsubscribeFromChannels()
{
	m_systemManager->getMessageHandler()->unsubscribe(ActorMessageType::Collision, this);
}

void Sys_BunkerDegradation::update(const float& deltaTime)
{
}

void Sys_BunkerDegradation::handleEvent(const ActorId& actorId, const ActorEventType& eventId)
{
}

void Sys_BunkerDegradation::debugOverlay(WindowManager* windowManager)
{
}

void Sys_BunkerDegradation::notify(const Message& msg)
{
	if (!hasActor(msg.m_receiver)) return;
	switch ((ActorMessageType)msg.m_type)
	{
	case ActorMessageType::Collision:
		sf::FloatRect collider = m_systemManager->getActorManager()->getActor(msg.m_sender)->getComponent<Comp_Collision>(ComponentType::Collision)->getAABB();
		Comp_Sprite* spriteComp = m_systemManager->getActorManager()->getActor(msg.m_receiver)->getComponent<Comp_Sprite>(ComponentType::Sprite);
		// get the texture of the sprite
		sf::Texture* texture = const_cast<sf::Texture*>(spriteComp->getSprite()->getTexture());
		if (!texture) return;
		// get the image of the texture
		sf::Image image = texture->copyToImage();
		// get sprite transform
		const sf::Transform& transform = spriteComp->getSprite()->getTransform();
		// get the bounding box in texture coordinates
		sf::FloatRect boundingBox = transform.transformRect(collider);
		// ensure the bounding box is within the texture
		boundingBox.left = std::max(0.f, boundingBox.left);
		boundingBox.top = std::max(0.f, boundingBox.top);
		boundingBox.width = std::min(boundingBox.width, (float)texture->getSize().x - boundingBox.left);
		boundingBox.height = std::min(boundingBox.height, (float)texture->getSize().y - boundingBox.top);
		const int textureWidth = texture->getSize().x;
		const int textureHeight = texture->getSize().y;
		// iterate over the pixels in the bounding box
		for (unsigned int y = boundingBox.top; y < boundingBox.top + boundingBox.height; ++y)
		{
			for (unsigned int x = boundingBox.left; x < boundingBox.left + boundingBox.width; ++x)
			{
				// get the pixel color
				sf::Color pixel = image.getPixel(x, y);
				// if the pixel is not transparent
				if (pixel.a > 0)
				{
					// set the pixel to transparent
					image.setPixel(x, y, sf::Color::Transparent);
				}
			}
		}
		// update the texture with the modified image
		texture->update(image);
		break;
	}
}
