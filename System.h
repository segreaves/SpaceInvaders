#pragma once

enum class System
{
	None = 0,
	Renderer = 1 << 0,
	Movement = 1 << 1,
	Collision = 1 << 2,
	Control = 1 << 3,
	State = 1 << 4,
	Animation = 1 << 5
};
