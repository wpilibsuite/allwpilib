// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.util;

/** A class to configure a PIDController with */
public class PIDConstants {
  // Factor for "proportion" control
  public final double kp;

  // Factor for "integral" control
  public final double ki;

  // Factor for "derivative" control
  public final double kd;

  /**
   * Create a new PIDConstants object.
   *
   * @param kp The proportional coefficient. Must be >= 0.
   * @param ki The integral coefficient. Must be >= 0.
   * @param kd The derivative coefficient. Must be >= 0.
   * @throws IllegalArgumentException if kp &lt; 0
   * @throws IllegalArgumentException if ki &lt; 0
   * @throws IllegalArgumentException if kd &lt; 0
   */
  public PIDConstants(double kp, double ki, double kd) {
    this.kp = kp;
    this.ki = ki;
    this.kd = kd;

    if (kp < 0.0) {
      throw new IllegalArgumentException("Kp must be a non-negative number!");
    }
    if (ki < 0.0) {
      throw new IllegalArgumentException("Ki must be a non-negative number!");
    }
    if (kd < 0.0) {
      throw new IllegalArgumentException("Kd must be a non-negative number!");
    }
  }
}
