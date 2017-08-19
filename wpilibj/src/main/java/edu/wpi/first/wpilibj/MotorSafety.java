/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Shuts off motors when their outputs aren't updated often enough.
 */
public interface MotorSafety {
  double DEFAULT_SAFETY_EXPIRATION = 0.1;

  void setExpiration(double timeout);

  double getExpiration();

  boolean isAlive();

  void stopMotor();

  void setSafetyEnabled(boolean enabled);

  boolean isSafetyEnabled();

  String getDescription();
}
