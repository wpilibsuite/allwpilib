// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.button;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import org.wpilib.command3.Trigger;
import org.wpilib.driverstation.GenericHID;

/** A {@link Trigger} that gets its state from a {@link GenericHID}. */
public class JoystickButton extends Trigger {
  /**
   * Creates a joystick button for triggering commands.
   *
   * @param joystick The GenericHID object that has the button (e.g. Joystick, KinectStick, etc)
   * @param buttonNumber The button number (see {@link GenericHID#getRawButton(int) }
   */
  public JoystickButton(GenericHID joystick, int buttonNumber) {
    super(() -> joystick.getRawButton(buttonNumber));
    requireNonNullParam(joystick, "joystick", "JoystickButton");
  }
}
