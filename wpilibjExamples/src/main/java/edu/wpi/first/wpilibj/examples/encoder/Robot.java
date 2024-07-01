// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.examples.encoder;

import edu.wpi.first.wpilibj.CounterBase;
import edu.wpi.first.wpilibj.Encoder;
import edu.wpi.first.wpilibj.TimedRobot;
import edu.wpi.first.wpilibj.smartdashboard.SmartDashboard;

/**
 * Sample program displaying the value of a quadrature encoder on the SmartDashboard. Quadrature
 * Encoders are digital sensors which can detect the amount the encoder has rotated since starting
 * as well as the direction in which the encoder shaft is rotating. However, encoders can not tell
 * you the absolute position of the encoder shaft (ie, it considers where it starts to be the zero
 * position, no matter where it starts), and so can only tell you how much the encoder has rotated
 * since starting. Depending on the precision of an encoder, it will have fewer or greater ticks per
 * revolution; the number of ticks per revolution will affect the conversion between ticks and
 * distance, as specified by DistancePerPulse. One of the most common uses of encoders is in the
 * drivetrain, so that the distance that the robot drives can be precisely controlled during the
 * autonomous mode.
 */
public class Robot extends TimedRobot {
  /**
   * The Encoder object is constructed with 4 parameters, the last two being optional. The first two
   * parameters (1, 2 in this case) refer to the ports on the roboRIO which the encoder uses.
   * Because a quadrature encoder has two signal wires, the signal from two DIO ports on the roboRIO
   * are used. The third (optional) parameter is a boolean which defaults to false. If you set this
   * parameter to true, the direction of the encoder will be reversed, in case it makes more sense
   * mechanically. The final (optional) parameter specifies encoding rate (k1X, k2X, or k4X) and
   * defaults to k4X. Faster (k4X) encoding gives greater positional precision but more noise in the
   * rate.
   */
  private final Encoder m_encoder = new Encoder(1, 2, false, CounterBase.EncodingType.k4X);

  /** Called once at the beginning of the robot program. */
  public Robot() {
    /*
     * Defines the number of samples to average when determining the rate.
     * On a quadrature encoder, values range from 1-255;
     * larger values result in smoother but potentially
     * less accurate rates than lower values.
     */
    m_encoder.setSamplesToAverage(5);

    /*
     * Defines how far the mechanism attached to the encoder moves per pulse. In
     * this case, we assume that a 360 count encoder is directly
     * attached to a 3 inch diameter (1.5inch radius) wheel,
     * and that we want to measure distance in inches.
     */
    m_encoder.setDistancePerPulse(1.0 / 360.0 * 2.0 * Math.PI * 1.5);

    /*
     * Defines the lowest rate at which the encoder will
     * not be considered stopped, for the purposes of
     * the GetStopped() method. Units are in distance / second,
     * where distance refers to the units of distance
     * that you are using, in this case inches.
     */
    m_encoder.setMinRate(1.0);
  }

  @Override
  public void teleopPeriodic() {
    SmartDashboard.putNumber("Encoder Distance", m_encoder.getDistance());
    SmartDashboard.putNumber("Encoder Rate", m_encoder.getRate());
  }
}
