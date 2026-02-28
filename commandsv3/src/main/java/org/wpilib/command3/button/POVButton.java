// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.command3.button;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import org.wpilib.command3.Context;
import org.wpilib.command3.Scheduler;
import org.wpilib.command3.Trigger;
import org.wpilib.driverstation.DriverStation.POVDirection;
import org.wpilib.driverstation.GenericHID;

/** A {@link Trigger} that gets its state from a POV on a {@link GenericHID}. */
public class POVButton extends Trigger {
  /**
   * Creates a POV button for triggering commands.
   *
   * @param scheduler The scheduler that should execute triggered commands.
   * @param context The context that must be true for edges to be considered
   * @param joystick The GenericHID object that has the POV
   * @param angle The desired angle
   * @param povNumber The POV number (see {@link GenericHID#getPOV(int)})
   */
  public POVButton(
      Scheduler scheduler,
      Context context,
      GenericHID joystick,
      POVDirection angle,
      int povNumber) {
    super(scheduler, context, () -> joystick.getPOV(povNumber) == angle);
    requireNonNullParam(joystick, "joystick", "POVButton");
  }

  /**
   * Creates a POV button for triggering commands. Uses the default scheduler and teleop context.
   *
   * @param joystick The GenericHID object that has the POV
   * @param angle The desired angle
   * @param povNumber The POV number (see {@link GenericHID#getPOV(int)})
   */
  public POVButton(GenericHID joystick, POVDirection angle, int povNumber) {
    this(Context.allTeleop, joystick, angle, povNumber);
  }

  /**
   * Creates a POV button for triggering commands. By default, acts on POV 0. Uses the default
   * scheduler and teleop context.
   *
   * @param joystick The GenericHID object that has the POV
   * @param angle The desired angle
   */
  public POVButton(GenericHID joystick, POVDirection angle) {
    this(joystick, angle, 0);
  }

  /**
   * Creates a POV button for triggering commands with the given context. Uses the default
   * scheduler.
   *
   * @param context The context that must be true for edges to be considered
   * @param joystick The GenericHID object that has the POV
   * @param angle The desired angle
   * @param povNumber The POV number (see {@link GenericHID#getPOV(int)})
   */
  public POVButton(Context context, GenericHID joystick, POVDirection angle, int povNumber) {
    super(context, () -> joystick.getPOV(povNumber) == angle);
    requireNonNullParam(joystick, "joystick", "POVButton");
  }
}
