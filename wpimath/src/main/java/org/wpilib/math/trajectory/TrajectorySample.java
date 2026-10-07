// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.trajectory;

/** Represents a single sample in a {@link Trajectory}. */
public interface TrajectorySample {
  /**
   * Gets the time of the sample relative to the trajectory start, in seconds.
   *
   * @return the time of the sample in seconds.
   */
  double time();
}
