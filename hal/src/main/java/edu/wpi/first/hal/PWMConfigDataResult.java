// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/** Structure for holding the config data result for PWM. */
@SuppressWarnings("MemberName")
public class PWMConfigDataResult {
  PWMConfigDataResult(
      double max, double deadbandMax, double center, double deadbandMin, double min) {
    this.max = max;
    this.deadbandMax = deadbandMax;
    this.center = center;
    this.deadbandMin = deadbandMin;
    this.min = min;
  }

  /** The maximum PWM value. */
  public double max;

  /** The deadband maximum PWM value. */
  public double deadbandMax;

  /** The center PWM value. */
  public double center;

  /** The deadband minimum PWM value. */
  public double deadbandMin;

  /** The minimum PWM value. */
  public double min;
}
