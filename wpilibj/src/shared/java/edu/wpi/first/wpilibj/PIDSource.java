/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;
import edu.wpi.first.wpilibj.PIDSourceType;

/**
 * This interface allows for PIDController to automatically read from this
 * object
 *$
 * @author dtjones
 */
public interface PIDSource {
  /**
   * Set which parameter of the device you are using as a process control
   * variable.
   *
   * @param pidSource
   *            An enum to select the parameter.
   */
  public void setPIDSourceType(PIDSourceType pidSource);

  /**
   * Get which parameter of the device you are using as a process control
   * variable.
   *
   * @return the currently selected PID source parameter
   */
  public PIDSourceType getPIDSourceType();

  /**
   * Get the result to use in PIDController
   *$
   * @return the result to use in PIDController
   */
  public double pidGet();
}
