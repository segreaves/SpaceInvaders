#pragma once
#include <SFML/Graphics.hpp>
#include "S_Base.h"

class WindowManager;

class S_Renderer : public S_Base
{
public:
	S_Renderer(SystemManager* systemManager);

	void update(const float& deltaTime) override;
	void handleEvent(const EntityId& entityId, const EntityEvent& msg) override;
	void notify(const Message& msg) override;

	void draw(WindowManager* windowManager);
};