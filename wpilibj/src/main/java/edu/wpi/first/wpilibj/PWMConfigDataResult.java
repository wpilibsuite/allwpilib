/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Structure for holding the config data result for PWM.
 */
public class PWMConfigDataResult {

  PWMConfigDataResult(int max, int deadbandMax, int center, int deadbandMin, int min) {
    this.max = max;
    this.deadbandMax = deadbandMax;
    this.center = center;
    this.deadbandMin = deadbandMin;
    this.min = min;
  }

  /**
   * The maximum PWM value.
   */
  @SuppressWarnings("MemberName")
  public int max;

  /**
   * The deadband maximum PWM value.
   */
  @SuppressWarnings("MemberName")
  public int deadbandMax;

  /**
   * The center PWM value.
   */
  @SuppressWarnings("MemberName")
  public int center;

  /**
   * The deadband minimum PWM value.
   */
  @SuppressWarnings("MemberName")
  public int deadbandMin;

  /**
   * The minimum PWM value.
   */
  @SuppressWarnings("MemberName")
  public int min;
}
