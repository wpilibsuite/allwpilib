// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.analoginput;

import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.discrete.AnalogInput;

/**
 * AnalogInput snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/analog-inputs-software.html
 */
public class Robot extends TimedRobot {
  // Initializes an AnalogInput on port 0
  AnalogInput analog = new AnalogInput(0);

  /** Called once at the beginning of the robot program. */
  public Robot() {}

  @Override
  public void teleopPeriodic() {
    // Gets the raw instantaneous measured value from the analog input, without
    // applying any calibration and ignoring oversampling and averaging
    // settings.
    analog.getValue();

    // Gets the instantaneous measured voltage from the analog input.
    // Oversampling and averaging settings are ignored
    analog.getVoltage();
  }
}
