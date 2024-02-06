// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.util;

/** A class to configure a PIDController with */
public class PIDConstants {
  /** P */
  public final double kp;
  /** I */
  public final double ki;
  /** D */
  public final double kd;
  /** Integral Sum */
  public final double IZone;
  /** Period */
  public final double period;

  /* Create a new PIDConstants object
   *
   * @param kp P
   * @param ki I
   * @param kd D
   * @param IZone Integral range
   * @throws IllegalArgumentException if kp &lt; 0
   * @throws IllegalArgumentException if ki &lt; 0
   * @throws IllegalArgumentException if kd &lt; 0
   * @throws IllegalArgumentException if period &lt;= 0
   */
  public PIDConstants(double kp, double ki, double kd, double IZone, double period) {
    this.kp = kp;
    this.ki = ki;
    this.kd = kd;
    this.IZone = IZone;
    this.period = period;

    if (kp < 0.0) {
      throw new IllegalArgumentException("Kp must be a non-negative number!");
    }
    if (ki < 0.0) {
      throw new IllegalArgumentException("Ki must be a non-negative number!");
    }
    if (kd < 0.0) {
      throw new IllegalArgumentException("Kd must be a non-negative number!");
    }
    if (period < 0.0) {
      throw new IllegalArgumentException("period must be a non-negative number!");
    }
  }

  /* Create a new PIDConstants object with period set to a default of 0.2
   *
   * @param kp P
   * @param ki I
   * @param kd D
   * @param IZone Integral range
   * @throws IllegalArgumentException if kp &lt; 0
   * @throws IllegalArgumentException if ki &lt; 0
   * @throws IllegalArgumentException if kd &lt; 0
   */
  public PIDConstants(double kp, double ki, double kd, double IZone) {
    this.kp = kp;
    this.ki = ki;
    this.kd = kd;
    this.IZone = IZone;
    this.period = 0.2;

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

  /**
   * Create a new PIDConstants object with IZone set to a default 1 and period set to a default 0.2
   *
   * @param kp P
   * @param ki I
   * @param kd D
   * @throws IllegalArgumentException if kp &lt; 0
   * @throws IllegalArgumentException if ki &lt; 0
   * @throws IllegalArgumentException if kd &lt; 0
   */
  public PIDConstants(double kp, double ki, double kd) {
    this.kp = kp;
    this.ki = ki;
    this.kd = kd;
    this.IZone = 1.0;
    this.period = 0.2;

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
