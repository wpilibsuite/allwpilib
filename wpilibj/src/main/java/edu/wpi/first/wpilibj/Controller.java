// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

/**
 * An interface for controllers. Controllers run control loops, the most command are PID controllers
 * and there variants, but this includes anything that is controlling an actuator in a separate
 * thread.
 *
 * @deprecated None of the 2020 FRC controllers use this.
 */
@Deprecated(since = "2020", forRemoval = true)
public interface Controller {
  /**
   * Allows the control loop to run.
   */
  void enable();

  /**
   * Stops the control loop from running until explicitly re-enabled by calling {@link #enable()}.
   */
  void disable();
}
