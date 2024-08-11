#pragma once
#include "CompType.h"

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
};