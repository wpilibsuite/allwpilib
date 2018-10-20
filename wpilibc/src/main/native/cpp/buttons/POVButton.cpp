
#include "frc/buttons/POVButton.h"

using namespace frc;

POVButton::POVButton(GenericHID* joystick, int angle, int povNumber) : m_angle(angle), m_povNumber(povNumber), m_joystick(joystick) {}

bool POVButton::Get() { return m_joystick->GetPOV(m_povNumber) == m_angle; }
