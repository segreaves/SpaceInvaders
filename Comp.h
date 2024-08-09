#pragma once

class Comp
{
public:
	Comp()
	{
		awake();
	}

	~Comp()
	{
		onDestroy();
	}

	virtual void awake() {};
	virtual void onDestroy() {};
	virtual void update(const float& deltaTime) = 0;
};