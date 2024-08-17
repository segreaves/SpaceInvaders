#pragma once
#include <SFML/Graphics.hpp>
#include "Direction.h"
#include <unordered_map>

class Context;

/// <summary>
/// Contains level bounds.
/// </summary>
class Level
{
public:
	Level(Context* context);
	~Level();

	sf::FloatRect* getColliders(Direction obj);
	std::unordered_map<Direction, sf::FloatRect>& getColliders();
private:
	std::unordered_map<Direction, sf::FloatRect> m_colliders;
	Context* m_context;
};