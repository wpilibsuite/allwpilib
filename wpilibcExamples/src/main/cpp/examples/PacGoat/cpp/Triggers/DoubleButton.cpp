/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#include "Triggers/DoubleButton.h"

#include <Joystick.h>

DoubleButton::DoubleButton(frc::Joystick* joy, int button1, int button2)
    : m_joy(*joy) {
  m_button1 = button1;
  m_button2 = button2;
}

bool DoubleButton::Get() {
  return m_joy.GetRawButton(m_button1) && m_joy.GetRawButton(m_button2);
}
