/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.interfaces;

/**
 * Interface for 3-axis accelerometers
 */
public interface Accelerometer {
  public enum Range {
    k2G, k4G, k8G, k16G
  }

  /**
   * Common interface for setting the measuring range of an accelerometer.
   *
   * @param range The maximum acceleration, positive or negative, that the
   *        accelerometer will measure. Not all accelerometers support all
   *        ranges.
   */
  public void setRange(Range range);

  /**
   * Common interface for getting the x axis acceleration
   *
   * @return The acceleration along the x axis in g-forces
   */
  public double getX();

  /**
   * Common interface for getting the y axis acceleration
   *
   * @return The acceleration along the y axis in g-forces
   */
  public double getY();

  /**
   * Common interface for getting the z axis acceleration
   *
   * @return The acceleration along the z axis in g-forces
   */
  public double getZ();
}
