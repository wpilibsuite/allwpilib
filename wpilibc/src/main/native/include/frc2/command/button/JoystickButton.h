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
/**
 * A class used to bind command scheduling to joystick button presses.  Can be
 * composed with other buttons with the operators in Trigger.
 *
 * @see Trigger
 */
class JoystickButton : public Button {
 public:
  /**
   * Creates a JoystickButton that commands can be bound to.
   *
   * @param joystick The joystick on which the button is located.
   * @param buttonNumber The number of the button on the joystic.
   */
  explicit JoystickButton(frc::GenericHID* joystick, int buttonNumber)
      : m_joystick{joystick}, m_buttonNumber{buttonNumber} {}

  bool Get() const override { return m_joystick->GetRawButton(m_buttonNumber); }

 private:
  frc::GenericHID* m_joystick;
  int m_buttonNumber;
};
}  // namespace frc2
