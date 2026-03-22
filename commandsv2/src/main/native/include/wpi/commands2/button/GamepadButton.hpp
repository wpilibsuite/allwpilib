// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include "wpi/commands2/button/Trigger.hpp"
#include "wpi/driverstation/Gamepad.hpp"

namespace wpi::cmd {
/**
 * A class used to bind command scheduling to gamepad button presses.  Can be
 * composed with other buttons with the operators in Trigger.
 *
 * This class is provided by the NewCommands VendorDep
 *
 * @see Trigger
 */
class GamepadButton : public Trigger {
 public:
  /**
   * Creates a GamepadButton that commands can be bound to.
   *
   * @param gamepad The gamepad on which the button is located.
   * @param button The button
   */
  explicit GamepadButton(wpi::Gamepad* gamepad,
                         enum wpi::Gamepad::Button button)
      : Trigger([gamepad, button] { return gamepad->GetButton(button); }) {}
};
}  // namespace wpi::cmd
