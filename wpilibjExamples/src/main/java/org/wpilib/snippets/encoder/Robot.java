// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.encoder;

import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.rotation.Encoder;

/**
 * Encoder snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
@SuppressWarnings("deprecation")
public class Robot extends TimedRobot {
  // Initializes an encoder on DIO pins 0 and 1
  // Defaults to 4X decoding and non-inverted
  Encoder m_encoder = new Encoder(0, 1);

  // Initializes an encoder on DIO pins 0 and 1
  // 2X encoding and non-inverted
  Encoder m_encoder2x = new Encoder(0, 1, false, Encoder.EncodingType.k2X);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // Configures the encoder to return a distance of 4 for every 256 pulses
    // Also changes the units of getRate
    m_encoder.setDistancePerPulse(4.0 / 256.0);
    // Configures the encoder to consider itself stopped after .1 seconds
    m_encoder.setMaxPeriod(0.1);
    // Configures the encoder to consider itself stopped when its rate is below 10
    m_encoder.setMinRate(10);
    // Reverses the direction of the encoder
    m_encoder.setReverseDirection(true);
    // Configures an encoder to average its period measurement over 5 samples
    // Can be between 1 and 127 samples
    m_encoder.setSamplesToAverage(5);

    m_encoder2x.getRate();
  }

  @Override
  public void teleopPeriodic() {
    // Gets the distance traveled
    m_encoder.getDistance();

    // Gets the current rate of the encoder
    m_encoder.getRate();

    // Gets whether the encoder is stopped
    m_encoder.getStopped();

    // Gets the last direction in which the encoder moved
    m_encoder.getDirection();

    // Gets the current period of the encoder
    m_encoder.getPeriod();

    // Resets the encoder to read a distance of zero
    m_encoder.reset();
  }
}
