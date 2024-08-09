#pragma once
#include "Comp.h"

class Comp_Sprite : public Comp
{
public:
	Comp_Sprite();
	~Comp_Sprite();

	void awake() override;
	void onDestroy() override;
	void update(const float& deltaTime) override;
};
