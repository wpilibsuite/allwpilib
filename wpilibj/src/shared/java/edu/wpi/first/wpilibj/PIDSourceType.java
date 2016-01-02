/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * A description for the type of output value to provide to a PIDController
 */
public enum PIDSourceType {
  kDisplacement(0),
  kRate(1);

  /**
   * The integer value representing this enumeration
   */
  public final int value;

  private PIDSourceType(int value) {
    this.value = value;
  }
}
