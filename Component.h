#pragma once

using ComponentType = unsigned int;

enum class Component
{
	None = 0,
	Position = 1 << 0,
	Sprite = 1 << 1,
	State = 1 << 2,
	Movable = 1 << 3,
	Controller = 1 << 4,
	Collidable = 1 << 5
};
