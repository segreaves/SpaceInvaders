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
		sf::FloatRect collider = m_systemManager->getActorManager()->getActor(msg.m_sender)->getComponent<Comp_Collision>(ComponentType::Collision)->getAABB();
		Comp_Sprite* spriteComp = m_systemManager->getActorManager()->getActor(msg.m_receiver)->getComponent<Comp_Sprite>(ComponentType::Sprite);
		// get the texture of the sprite
		sf::Texture* texture = const_cast<sf::Texture*>(spriteComp->getSprite()->getTexture());
		if (!texture) return;
		// get the image of the texture
		sf::Image image = texture->copyToImage();
		// get the sprite's rectangle in global coordinates
		sf::FloatRect spriteRect = spriteComp->getSprite()->getGlobalBounds();
		// get the intersection of the sprite's rectangle and the collider
		sf::FloatRect intersection;
		if (!spriteRect.intersects(collider, intersection)) return;
		// get the intersection in relation to the sprite's rectangle
		intersection.left -= spriteRect.left;
		intersection.top -= spriteRect.top;

		// get the intersection start position in relation to the texture
		unsigned int xStart = intersection.left / spriteComp->getScale().x;
		unsigned int yStart = intersection.top / spriteComp->getScale().y;
		// get the intersection width and height in relation to the texture
		unsigned int width = intersection.width / spriteRect.width * texture->getSize().x;
		unsigned int height = intersection.height / spriteRect.height * texture->getSize().y;
		
		// iterate over the pixels in the intersection
		for (unsigned int x = xStart; x < xStart + width; x++)
			for (unsigned int y = yStart; y < yStart + height; y++)
				if (image.getPixel(x, y).a > 0)// if pixel is not transparent
					image.setPixel(x, y, sf::Color(255, 255, 255, 0));// set pixel to transparent
		// update the texture with the modified image
		texture->update(image);
		break;
	}
}
