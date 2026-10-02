// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.encoder;

import org.wpilib.framework.TimedRobot;
import org.wpilib.hardware.rotation.Encoder;

/**
 * Encoder snippets for wpilib-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/encoders-software.html
 */
@SuppressWarnings("deprecation")
public class Robot extends TimedRobot {
  // Initializes an encoder on DIO pins 0 and 1
  // Defaults to 4X decoding and non-inverted
  Encoder encoder = new Encoder(0, 1);

  // Initializes an encoder on DIO pins 0 and 1
  // 2X encoding and non-inverted
  Encoder encoder2x = new Encoder(0, 1, false, Encoder.EncodingType.X2);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    // Configures the encoder to return a distance of 4 for every 256 pulses
    // Also changes the units of getRate
    encoder.setDistancePerPulse(4.0 / 256.0);
    // Reverses the direction of the encoder
    encoder.setReverseDirection(true);

    encoder2x.getRate();
  }

  @Override
  public void teleopPeriodic() {
    // Gets the distance traveled
    encoder.getDistance();

    // Gets the current rate of the encoder
    encoder.getRate();

    // Gets whether the encoder is stopped
    encoder.getStopped();

    // Gets the last direction in which the encoder moved
    encoder.getDirection();

    // Gets the current period of the encoder

    // Resets the encoder to read a distance of zero
    encoder.reset();
  }
}
