#pragma once
#include "ResourceManager.h"
#include <SFML/Graphics/Texture.hpp>

/// <summary>
/// Loads textures and provides pointers to them when requested.
/// </summary>
class TextureManager : public ResourceManager<TextureManager, sf::Texture>
{
public:
	TextureManager() : ResourceManager("profiles/textures.dat")
	{
	}

	sf::Texture* load(const std::string& path)
	{
		sf::Texture* texture = new sf::Texture();
		if (!texture->loadFromFile(Utils::getAssetsDirectory() + path))
		{
			delete texture;
			texture = nullptr;
			std::cerr << "TextureManager failed to load texture: " << path << std::endl;
		}
		return texture;
	}
};