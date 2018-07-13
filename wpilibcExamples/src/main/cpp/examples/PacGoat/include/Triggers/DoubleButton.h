/*----------------------------------------------------------------------------*/
/* Use and modify this template how you wish, it's public domain.             */
/*----------------------------------------------------------------------------*/

#pragma once

#include <Buttons/Trigger.h>

namespace frc {
class Joystick;
}  // namespace frc

class DoubleButton : public frc::Trigger {
 public:
  DoubleButton(frc::Joystick* joy, int button1, int button2);

  bool Get();

 private:
  frc::Joystick& m_joy;
  int m_button1;
  int m_button2;
};
