#pragma once
#include "Comp.h"

/// <summary>
/// Holds the torque spring data of an actor (used to stabilize the actor after the
/// torque is applied by the ShipSway system).
/// </summary>
class Comp_TorqueSpring : public Comp
{
public:
    void setAngle(float angle) { m_currentAngle = angle; }
    float getAngle() const { return m_currentAngle; }
    float getStiffness() const { return m_stiffness; }
    float getDamping() const { return m_damping; }

    void rotate(float angle) { m_currentAngle += angle; }
private:
    void load(std::stringstream& ss) override
    {
        ss >> m_stiffness >> m_damping;
    }

    float m_currentAngle = 0.f;
    float m_stiffness = 0.f;
    float m_damping = 0.f;
};
