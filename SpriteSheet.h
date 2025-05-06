#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "TextureManager.h"
#include "OriginType.h"

/// <summary>
/// Holds a pointer to the objects sprite. Can be used to animate sprites through the
/// cropSprite() and frameStep() methods.
/// </summary>
class SpriteSheet
{
public:
	SpriteSheet(TextureManager* textureManager, bool sharedMemory);
	~SpriteSheet();

	void draw(sf::RenderWindow* window);

	void frameStep();
	void resetFrame();
	void cropSprite();
	int getTotalFrames() const;
	const sf::Sprite* getSprite() const;
	const sf::Texture* getTexture() const;
	const sf::Vector2i& getSpriteSize() const;
	const sf::Vector2f& getSpriteScale() const;
	const sf::Vector2f& getSpritePosition() const;
	void setSpriteSize(const sf::Vector2i& size);
	void setSpritePosition(const sf::Vector2f& position);
	void setSpriteRotation(const float& rotation);
	void setSpriteScale(const sf::Vector2f& scale);
	void setSheetPadding(const sf::Vector2i& padding);
	void setSpriteSpacing(const sf::Vector2i& spacing);
	void setSpriteOrigin(OriginType origin);
	void setSpriteColor(const sf::Color& color);
	void setSmooth(const bool& smooth);
	const sf::Vector2i& getSheetPadding() const;
	const sf::Vector2i& getSpriteSpacing() const;
	const sf::IntRect& getCropRect() const;
	sf::FloatRect globalRectToPixelRect(const sf::FloatRect& rect) const;

	bool loadSheet(const std::string& filePath);
	void releaseSheet();
private:
	TextureManager* m_textureManager;
	OriginType m_origin;
	sf::Texture* m_texture;
	std::string m_textureId;
	sf::Sprite m_sprite;
	sf::Vector2i m_spriteSize;
	sf::Vector2f m_spriteScale;
	sf::Vector2i m_sheetPadding;
	sf::Vector2i m_spriteSpacing;
	sf::IntRect m_cropRect;
	int m_totalFrames;
	int m_frame;
	bool m_sharedMemory;
};