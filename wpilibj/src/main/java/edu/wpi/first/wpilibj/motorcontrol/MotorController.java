// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.motorcontrol;

import edu.wpi.first.wpilibj.RobotController;
import edu.wpi.first.wpilibj.SpeedController;

/** Interface for motor controlling devices. */
@SuppressWarnings("removal")
public interface MotorController extends SpeedController {
  default void setVoltage(double outputVolts) {
    set(outputVolts / RobotController.getBatteryVoltage());
  }
}
