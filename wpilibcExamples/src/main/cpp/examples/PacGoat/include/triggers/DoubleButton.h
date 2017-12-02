/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/buttons/Trigger.h>

namespace frc {
class Joystick;
}  // namespace frc

class DoubleButton : public frc::Trigger {
 public:
  DoubleButton(frc::Joystick* joy, int button1, int button2);

  bool Get() override;
  bool GetPressed() override;
  bool GetReleased() override;

 private:
  frc::Joystick& m_joy;
  int m_button1;
  int m_button2;
  bool m_button1Pressed = false;
  bool m_button2Pressed = false;
  bool m_button1Released = false;
  bool m_button2Released = false;
};
