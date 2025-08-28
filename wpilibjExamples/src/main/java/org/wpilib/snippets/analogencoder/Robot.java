// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.analogencoder;

import org.wpilib.AnalogEncoder;
import org.wpilib.TimedRobot;

/**
 * AnalogEncoder snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
public class Robot extends TimedRobot {
  // Initializes an analog encoder on Analog Input pin 0
  AnalogEncoder m_encoder = new AnalogEncoder(0);

  // Initializes an analog encoder on DIO pins 0 to return a value of 4 for
  // a full rotation, with the encoder reporting 0 half way through rotation (2
  // out of 4)
  AnalogEncoder m_encoderFR = new AnalogEncoder(0, 4.0, 2.0);

  /** Called once at the beginning of the robot program. */
  public Robot() {}

  @Override
  public void teleopPeriodic() {
    // Gets the rotation
    m_encoder.get();

    m_encoderFR.get();
  }
}
