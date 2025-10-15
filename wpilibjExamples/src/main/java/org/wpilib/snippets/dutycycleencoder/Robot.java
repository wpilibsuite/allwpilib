// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.dutycycleencoder;

import org.wpilib.hardware.rotation.DutyCycleEncoder;
import org.wpilib.opmode.TimedRobot;

/**
 * DutyCycleEncoder snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
public class Robot extends TimedRobot {
  // Initializes a duty cycle encoder on DIO pins 0
  DutyCycleEncoder m_encoder = new DutyCycleEncoder(0);

  // Initializes a duty cycle encoder on DIO pins 0 to return a value of 4 for
  // a full rotation, with the encoder reporting 0 half way through rotation (2
  // out of 4)
  DutyCycleEncoder m_encoderFR = new DutyCycleEncoder(0, 4.0, 2.0);

  /** Called once at the beginning of the robot program. */
  public Robot() {}

  @Override
  public void teleopPeriodic() {
    // Gets the rotation
    m_encoder.get();

    // Gets if the encoder is connected
    m_encoder.isConnected();

    m_encoderFR.get();
  }
}
