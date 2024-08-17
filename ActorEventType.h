#pragma once

enum class ActorEventType
{
	Spawned = 0,
	Despawned,
	Moving_Left,
	Moving_Right,
	Moving_Up,
	Moving_Down,
	CollidingOnX,
	CollidingOnY
};
