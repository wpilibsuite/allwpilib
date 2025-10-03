// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.commands3.button;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.wpilibj.DriverStation.POVDirection;
import edu.wpi.first.wpilibj.GenericHID;
import org.wpilib.commands3.Trigger;

/** A {@link Trigger} that gets its state from a POV on a {@link GenericHID}. */
public class POVButton extends Trigger {
  /**
   * Creates a POV button for triggering commands.
   *
   * @param joystick The GenericHID object that has the POV
   * @param angle The desired angle
   * @param povNumber The POV number (see {@link GenericHID#getPOV(int)})
   */
  public POVButton(GenericHID joystick, POVDirection angle, int povNumber) {
    super(() -> joystick.getPOV(povNumber) == angle);
    requireNonNullParam(joystick, "joystick", "POVButton");
  }

  /**
   * Creates a POV button for triggering commands. By default, acts on POV 0
   *
   * @param joystick The GenericHID object that has the POV
   * @param angle The desired angle
   */
  public POVButton(GenericHID joystick, POVDirection angle) {
    this(joystick, angle, 0);
  }
}
