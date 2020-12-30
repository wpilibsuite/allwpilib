// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.interfaces;

/** Interface for 3-axis accelerometers. */
public interface Accelerometer {
  enum Range {
    k2G,
    k4G,
    k8G,
    k16G
  }

  /**
   * Common interface for setting the measuring range of an accelerometer.
   *
   * @param range The maximum acceleration, positive or negative, that the accelerometer will
   *     measure. Not all accelerometers support all ranges.
   */
  void setRange(Range range);

  /**
   * Common interface for getting the x axis acceleration.
   *
   * @return The acceleration along the x axis in g-forces
   */
  double getX();

  /**
   * Common interface for getting the y axis acceleration.
   *
   * @return The acceleration along the y axis in g-forces
   */
  double getY();

  /**
   * Common interface for getting the z axis acceleration.
   *
   * @return The acceleration along the z axis in g-forces
   */
  double getZ();
}
