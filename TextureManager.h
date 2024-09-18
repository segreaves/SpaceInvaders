#pragma once
#include "ResourceManager.h"
#include <SFML/Graphics/Texture.hpp>

class TextureManager : public ResourceManager<TextureManager, sf::Texture>
{
public:
	TextureManager() : ResourceManager("assets/profiles/textures.dat")
	{
	}

	sf::Texture* load(const std::string& path)
	{
		sf::Texture* texture = new sf::Texture();
		if (!texture->loadFromFile(Utils::getWorkingDirectory() + path))
		{
			delete texture;
			texture = nullptr;
			std::cerr << "TextureManager failed to load texture: " << path << std::endl;
		}
		return texture;
	}
};