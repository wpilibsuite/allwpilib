// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static edu.wpi.first.math.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.wpilibj.GenericHID;

/** A {@link Button} that gets its state from a {@link GenericHID}. */
public class JoystickButton extends Button {
  private final GenericHID m_joystick;
  private final int m_buttonNumber;

  /**
   * Creates a joystick button for triggering commands.
   *
   * @param joystick The GenericHID object that has the button (e.g. Joystick, KinectStick, etc)
   * @param buttonNumber The button number (see {@link GenericHID#getRawButton(int) }
   */
  public JoystickButton(GenericHID joystick, int buttonNumber) {
    requireNonNullParam(joystick, "joystick", "JoystickButton");

    m_joystick = joystick;
    m_buttonNumber = buttonNumber;
  }

  /**
   * Gets the value of the joystick button.
   *
   * @return The value of the joystick button
   */
  @Override
  public boolean get() {
    return m_joystick.getRawButton(m_buttonNumber);
  }
}
