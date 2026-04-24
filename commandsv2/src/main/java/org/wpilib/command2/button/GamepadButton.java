// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command2.button;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import org.wpilib.driverstation.Gamepad;

/**
 * A {@link Trigger} that gets its state from a {@link Gamepad}.
 *
 * <p>This class is provided by the Commands v2 VendorDep
 */
public class GamepadButton extends Trigger {
  /**
   * Creates a gamepad button for triggering commands.
   *
   * @param gamepad The gamepad on which the button is located.
   * @param button The button
   */
  public GamepadButton(Gamepad gamepad, Gamepad.Button button) {
    super(() -> gamepad.getButton(button));
    requireNonNullParam(gamepad, "gamepad", "GamepadButton");
  }
}
