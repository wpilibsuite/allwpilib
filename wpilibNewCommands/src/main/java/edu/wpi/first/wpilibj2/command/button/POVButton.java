// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command.button;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.wpilibj.GenericHID;

/**
 * A {@link Button} that gets its state from a POV on a {@link GenericHID}.
 *
 * <p>This class is provided by the NewCommands VendorDep
 */
@SuppressWarnings("deprecation")
public class POVButton extends Button {
  /**
   * Creates a POV button for triggering commands.
   *
   * @param joystick The GenericHID object that has the POV
   * @param angle The desired angle in degrees (e.g. 90, 270)
   * @param povNumber The POV number (see {@link GenericHID#getPOV(int)})
   */
  public POVButton(GenericHID joystick, int angle, int povNumber) {
    super(() -> joystick.getPOV(povNumber) == angle);
    requireNonNullParam(joystick, "joystick", "POVButton");
  }

  /**
   * Creates a POV button for triggering commands. By default, acts on POV 0
   *
   * @param joystick The GenericHID object that has the POV
   * @param angle The desired angle (e.g. 90, 270)
   */
  public POVButton(GenericHID joystick, int angle) {
    this(joystick, angle, 0);
  }
}
