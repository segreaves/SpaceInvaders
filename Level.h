#pragma once
#include <SFML/Graphics.hpp>
#include "Direction.h"
#include <unordered_map>

class Context;

/// <summary>
/// Contains level bounds.
/// Up/Down bounds are shifted away from view space to allow for bullets to be disabled off screen.
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