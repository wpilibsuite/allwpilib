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
class POVButton : public Button {
 public:
  POVButton(frc::GenericHID* joystick, int angle, int povNumber)
      : m_joystick{joystick}, m_angle{angle}, m_povNumber{povNumber} {}
  POVButton(frc::GenericHID* joystick, int angle)
      : POVButton{joystick, angle, 0} {}
  bool Get() const override {
    return m_joystick->GetPOV(m_povNumber) == m_angle;
  }

 private:
  frc::GenericHID* m_joystick;
  int m_angle;
  int m_povNumber;
};
}  // namespace frc2
