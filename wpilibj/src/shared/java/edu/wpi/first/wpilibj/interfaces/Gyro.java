/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2014-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.interfaces;

/**
 * Interface for yaw rate gyros
 */
public interface Gyro {
  /**
   * Calibrate the gyro by running for a number of samples and computing the
   * center value. Then use the center value as the Accumulator center value for
   * subsequent measurements. It's important to make sure that the robot is not
   * moving while the centering calculations are in progress, this is typically
   * done when the robot is first turned on while it's sitting at rest before
   * the competition starts.
   */
  public void calibrate();

  /**
   * Reset the gyro. Resets the gyro to a heading of zero. This can be used if
   * there is significant drift in the gyro and it needs to be recalibrated
   * after it has been running.
   */
  public void reset();

  /**
   * Return the actual angle in degrees that the robot is currently facing.
   *
   * The angle is based on the current accumulator value corrected by the
   * oversampling rate, the gyro type and the A/D calibration values. The angle
   * is continuous, that is it will continue from 360 to 361 degrees. This
   * allows algorithms that wouldn't want to see a discontinuity in the gyro
   * output as it sweeps past from 360 to 0 on the second time around.
   *
   * @return the current heading of the robot in degrees. This heading is based
   *         on integration of the returned rate from the gyro.
   */
  public double getAngle();

  /**
   * Return the rate of rotation of the gyro
   *
   * The rate is based on the most recent reading of the gyro analog value
   *
   * @return the current rate in degrees per second
   */
  public double getRate();

  /**
   * Free the resources used by the gyro
   */
  public void free();
}
