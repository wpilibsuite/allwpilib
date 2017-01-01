/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Structure for holding the values stored in an accumulator.
 */
public class AccumulatorResult {

  /**
   * The total value accumulated.
   */
  @SuppressWarnings("MemberName")
  public long value;
  /**
   * The number of sample vaule was accumulated over.
   */
  @SuppressWarnings("MemberName")
  public long count;
}
