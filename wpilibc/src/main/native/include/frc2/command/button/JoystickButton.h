/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once
#include <frc/GenericHID.h>

#include "Button.h"

namespace frc2 {
class JoystickButton : public Button {
 public:
  explicit JoystickButton(frc::GenericHID* joystick, int buttonNumber)
      : m_joystick{joystick}, m_buttonNumber{buttonNumber} {}
  bool Get() const override { return m_joystick->GetRawButton(m_buttonNumber); }

 private:
  frc::GenericHID* m_joystick;
  int m_buttonNumber;
};
}  // namespace frc2
