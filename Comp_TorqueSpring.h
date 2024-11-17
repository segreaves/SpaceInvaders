#pragma once
#include "Comp.h"

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

    float m_currentAngle;
    float m_stiffness;
    float m_damping;
    float m_angularVelocity;
};
