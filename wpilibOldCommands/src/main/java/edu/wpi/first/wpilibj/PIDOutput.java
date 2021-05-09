// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

/**
 * This interface allows PIDController to write it's results to its output.
 *
 * @deprecated Use DoubleConsumer and new PIDController class.
 */
@FunctionalInterface
@Deprecated(since = "2020", forRemoval = true)
public interface PIDOutput {
  /**
   * Set the output to the value calculated by PIDController.
   *
   * @param output the value calculated by PIDController
   */
  void pidWrite(double output);
}
