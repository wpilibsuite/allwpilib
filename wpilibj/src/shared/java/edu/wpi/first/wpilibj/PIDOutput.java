/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * This interface allows PIDController to write it's results to its output.
 *$
 * @author dtjones
 */
public interface PIDOutput {

  /**
   * Set the output to the value calculated by PIDController
   *$
   * @param output the value calculated by PIDController
   */
  public void pidWrite(double output);
}
