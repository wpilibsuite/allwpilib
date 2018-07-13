/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Buttons/JoystickButton.h>
#include <Joystick.h>

class OI {
 public:
  OI();
  frc::Joystick& GetJoystick();

 private:
  frc::Joystick m_joy{0};

  // Create some buttons
  frc::JoystickButton m_dUp{&m_joy, 5};
  frc::JoystickButton m_dRight{&m_joy, 6};
  frc::JoystickButton m_dDown{&m_joy, 7};
  frc::JoystickButton m_dLeft{&m_joy, 8};
  frc::JoystickButton m_l2{&m_joy, 9};
  frc::JoystickButton m_r2{&m_joy, 10};
  frc::JoystickButton m_l1{&m_joy, 11};
  frc::JoystickButton m_r1{&m_joy, 12};
};
