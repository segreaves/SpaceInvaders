#include "SpriteSheet.h"

SpriteSheet::SpriteSheet(TextureManager* textureManager) :
	m_textureManager(textureManager),
	m_spriteScale(1.0f, 1.0f),
	m_totalFrames(0),
	m_frame(0)
{
}

SpriteSheet::~SpriteSheet()
{
	releaseSheet();
}

void SpriteSheet::draw(sf::RenderWindow* window)
{
	window->draw(m_sprite);
}

void SpriteSheet::frameStep()
{
	m_frame = ++m_frame % m_totalFrames;
}

void SpriteSheet::resetFrame()
{
	m_frame = 0;
}

void SpriteSheet::cropSprite()
{
	sf::IntRect rect(
		(m_spriteSize.x * m_frame) + m_sheetPadding.x + (m_spriteSpacing.x * m_frame),
		m_sheetPadding.y,
		m_spriteSize.x,
		m_spriteSize.y
	);
	m_sprite.setTextureRect(rect);
}

int SpriteSheet::getTotalFrames() const
{
	return m_totalFrames;
}

const sf::Vector2u& SpriteSheet::getSpriteSize() const { return m_spriteSize; }

const sf::Vector2f& SpriteSheet::getSpriteScale() const
{
	return m_spriteScale;
}

const sf::Vector2f& SpriteSheet::getSpritePosition() const { return m_sprite.getPosition(); }

void SpriteSheet::setSpriteSize(const sf::Vector2u& size)
{
	m_spriteSize = size;
	m_sprite.setOrigin(m_spriteSize.x / 2.f, m_spriteSize.y / 2.f);
}

void SpriteSheet::setSpritePosition(const sf::Vector2f& position)
{
	m_sprite.setPosition(position);
}

void SpriteSheet::setSheetPadding(const sf::Vector2f& padding)
{
	m_sheetPadding = padding;
}

void SpriteSheet::setSpriteSpacing(const sf::Vector2f& spacing)
{
	m_spriteSpacing = spacing;
}

const sf::Vector2f& SpriteSheet::getSheetPadding() const { return m_sheetPadding; }

const sf::Vector2f& SpriteSheet::getSpriteSpacing() const { return m_spriteSpacing; }

bool SpriteSheet::loadSheet(const std::string& filePath)
{
	std::ifstream file;
	file.open(Utils::getWorkingDirectory() + filePath);
	if (!file.is_open())
	{
		std::cerr << "SpriteSheet failed to load file: " << filePath << std::endl;
		return false;
	}
	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '#') continue;
		std::stringstream ss(line);
		std::string type;
		ss >> type;
		if (type == "Texture")
		{
			if (m_texture != "")
			{
				std::cerr << "Duplicate textures in: " << filePath << std::endl;
				continue;
			}
			std::string texture;
			ss >> texture;
			if (!m_textureManager->requireResource(texture))
			{
				std::cerr << "SpriteSheet could not set up the texture: " << texture << std::endl;
				continue;
			}
			m_texture = texture;
			m_sprite.setTexture(*m_textureManager->getResource(m_texture));
		}
		else if (type == "Frames")
		{
			ss >> m_totalFrames;
		}
		else if (type == "Size")
		{
			ss >> m_spriteSize.x >> m_spriteSize.y;
			setSpriteSize(m_spriteSize);
		}
		else if (type == "Scale")
		{
			ss >> m_spriteScale.x >> m_spriteScale.y;
			m_sprite.setScale(m_spriteScale);
		}
		else if (type == "Padding")
		{
			ss >> m_sheetPadding.x >> m_sheetPadding.y;
		}
		else if (type == "Spacing")
		{
			ss >> m_spriteSpacing.x >> m_spriteSpacing.y;
		}
	}
	file.close();
	return true;
}

void SpriteSheet::releaseSheet()
{
	m_textureManager->releaseResource(m_texture);
}
