#pragma once

using ComponentType = unsigned int;

enum class Component
{
	Position = 0,
	Sprite = 1 << 0,
	State = 1 << 1,
	Movable = 1 << 2,
	Controller = 1 << 3,
	Collidable = 1 << 4
};
