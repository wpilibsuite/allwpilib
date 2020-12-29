// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

/**
 * This interface allows for PIDController to automatically read from this object.
 *
 * @deprecated Use DoubleSupplier and new PIDController class.
 */
@Deprecated(since = "2020", forRemoval = true)
public interface PIDSource {
  /**
   * Set which parameter of the device you are using as a process control variable.
   *
   * @param pidSource An enum to select the parameter.
   */
  void setPIDSourceType(PIDSourceType pidSource);

  /**
   * Get which parameter of the device you are using as a process control variable.
   *
   * @return the currently selected PID source parameter
   */
  PIDSourceType getPIDSourceType();

  /**
   * Get the result to use in PIDController.
   *
   * @return the result to use in PIDController
   */
  double pidGet();
}
