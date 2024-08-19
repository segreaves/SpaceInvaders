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
private:
	Context* m_context;
};