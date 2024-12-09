#pragma once
#include "ComponentType.h"
#include <sstream>

/// <summary>
/// Base class for all components. Only requires a load method to be implemented.
/// </summary>
class Comp
{
public:
	virtual ~Comp()
	{
	}

	friend std::stringstream& operator >>(std::stringstream& ss, Comp& comp)
	{
		comp.load(ss);
		return ss;
	}
private:
	virtual void load(std::stringstream& ss) = 0;
};