#pragma once
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include "TextureManager.h"

class SpriteSheet
{
public:
	SpriteSheet(TextureManager* textureManager);
	~SpriteSheet();

	void draw(sf::RenderWindow* window);

	void frameStep();
	void cropSprite();
	const sf::Vector2u& getSpriteSize() const;
	const sf::Vector2f& getSpritePosition() const;
	void setSpriteSize(const sf::Vector2u& size);
	void setSpritePosition(const sf::Vector2f& position);
	void setSheetPadding(const sf::Vector2f& padding);
	void setSpriteSpacing(const sf::Vector2f& spacing);
	const sf::Vector2f& getSheetPadding() const;
	const sf::Vector2f& getSpriteSpacing() const;

	bool loadSheet(const std::string& filePath);
private:
	TextureManager* m_textureManager;
	std::string m_texture;
	sf::Sprite m_sprite;
	sf::Vector2u m_spriteSize;
	sf::Vector2f m_spriteScale;
	sf::Vector2f m_sheetPadding;
	sf::Vector2f m_spriteSpacing;
	int m_totalFrames;
	int m_frame;
};