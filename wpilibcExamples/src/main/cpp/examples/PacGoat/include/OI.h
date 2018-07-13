/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Buttons/JoystickButton.h>
#include <Joystick.h>

#include "Triggers/DoubleButton.h"

class OI {
 public:
  OI();
  frc::Joystick& GetJoystick();

 private:
  frc::Joystick m_joystick{0};

  frc::JoystickButton m_l1{&m_joystick, 11};
  frc::JoystickButton m_l2{&m_joystick, 9};
  frc::JoystickButton m_r1{&m_joystick, 12};
  frc::JoystickButton m_r2{&m_joystick, 10};

  DoubleButton m_sticks{&m_joystick, 2, 3};
};
