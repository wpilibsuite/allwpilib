// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <frc/GenericHID.h>
#include <wpi/deprecated.h>

#include "Button.h"

namespace frc2 {
/**
 * A class used to bind command scheduling to joystick button presses.  Can be
 * composed with other buttons with the operators in Trigger.
 *
 * This class is provided by the NewCommands VendorDep
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
  WPI_IGNORE_DEPRECATED
  explicit JoystickButton(frc::GenericHID* joystick, int buttonNumber)
      : Button([joystick, buttonNumber] {
          return joystick->GetRawButton(buttonNumber);
        }) {}
  WPI_UNIGNORE_DEPRECATED
};
}  // namespace frc2
