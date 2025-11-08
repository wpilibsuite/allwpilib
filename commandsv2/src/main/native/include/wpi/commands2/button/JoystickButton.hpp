// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include "wpi/commands2/button/Trigger.hpp"
#include "wpi/driverstation/GenericHID.hpp"

namespace wpi::cmd {
/**
 * A class used to bind command scheduling to joystick button presses.  Can be
 * composed with other buttons with the operators in Trigger.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see Trigger
 */
class JoystickButton : public Trigger {
 public:
  /**
   * Creates a JoystickButton that commands can be bound to.
   *
   * @param joystick The joystick on which the button is located.
   * @param buttonNumber The number of the button on the joystick.
   */
  explicit JoystickButton(wpi::GenericHID* joystick, int buttonNumber)
      : Trigger([joystick, buttonNumber] {
          return joystick->GetRawButton(buttonNumber);
        }) {}
};
}  // namespace wpi::cmd
