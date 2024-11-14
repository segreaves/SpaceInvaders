#pragma once
#include "Comp.h"

class Comp_TorqueSpring : public Comp
{
public:
	void setAngle(float angle) { m_angle = angle; }
	float getAngle() const { return m_angle; }
	float getStiffness() const { return m_stiffness; }
	float getDampingCoeff() const { return m_dampingCoeff; }
	float getAngularVelocity() const { return m_angularVelocity; }
	void setAngularVelocity(float angularVelocity) { m_angularVelocity = angularVelocity; }
private:
	void load(std::stringstream& ss) override
	{
		ss >> m_stiffness >> m_dampingCoeff;
	}
	float m_angle;
	float m_stiffness;
	float m_dampingCoeff;
	float m_angularVelocity;
};