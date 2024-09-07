#pragma once
#include "ComponentType.h"
#include <sstream>

class Comp
{
public:
	friend std::stringstream& operator >>(std::stringstream& ss, Comp& comp)
	{
		comp.load(ss);
		return ss;
	}
private:
	virtual void load(std::stringstream& ss) = 0;
};