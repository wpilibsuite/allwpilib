/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Interface for speed controlling devices.
 */
public interface SpeedController extends PIDOutput {

  /**
   * Common interface for getting the current set speed of a speed controller.
   *
   * @return The current set speed. Value is between -1.0 and 1.0.
   */
  double get();

  /**
   * Common interface for setting the speed of a speed controller.
   *
   * @param speed The speed to set. Value should be between -1.0 and 1.0.
   * @param syncGroup The update group to add this Set() to, pending
   *        UpdateSyncGroup(). If 0, update immediately.
   */
  void set(double speed, byte syncGroup);

  /**
   * Common interface for setting the speed of a speed controller.
   *
   * @param speed The speed to set. Value should be between -1.0 and 1.0.
   */
  void set(double speed);

  /**
   * Common interface for inverting direction of a speed controller.
   *
   * @param isInverted The state of inversion true is inverted.
   */
  void setInverted(boolean isInverted);

  /**
   * Common interface for returning if a speed controller is in the inverted
   * state or not.
   *$
   * @return isInverted The state of the inversion true is inverted.
   *
   */
  boolean getInverted();

  /**
   * Disable the speed controller
   */
  void disable();

  /**
   * Stops motor movement. Motor can be moved again by calling set without having
   * to re-enable the motor.
   */
  void stopMotor();
}
