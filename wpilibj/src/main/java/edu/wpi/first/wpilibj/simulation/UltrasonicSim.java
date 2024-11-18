// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.wpilibj.simulation;

import org.wpilib.hal.SimBoolean;
import org.wpilib.hal.SimDouble;
import org.wpilib.math.util.Units;
import org.wpilib.wpilibj.Ultrasonic;

/** Class to control a simulated {@link org.wpilib.wpilibj.Ultrasonic}. */
public class UltrasonicSim {
  private final SimBoolean m_simRangeValid;
  private final SimDouble m_simRange;

  /**
   * Constructor.
   *
   * @param ultrasonic The real ultrasonic to simulate
   */
  public UltrasonicSim(Ultrasonic ultrasonic) {
    // ping parameter is unused
    this(-1, ultrasonic.getEchoChannel());
  }

  /**
   * Constructor.
   *
   * @param ping unused.
   * @param echo the ultrasonic's echo channel.
   */
  public UltrasonicSim(@SuppressWarnings("unused") int ping, int echo) {
    SimDeviceSim simDevice = new SimDeviceSim("Ultrasonic", echo);
    m_simRangeValid = simDevice.getBoolean("Range Valid");
    m_simRange = simDevice.getDouble("Range (in)");
  }

  /**
   * Sets if the range measurement is valid.
   *
   * @param valid True if valid
   */
  public void setRangeValid(boolean valid) {
    m_simRangeValid.set(valid);
  }

  /**
   * Sets the range measurement.
   *
   * @param inches The range in inches.
   */
  public void setRangeInches(double inches) {
    m_simRange.set(inches);
  }

  /**
   * Sets the range measurement.
   *
   * @param meters The range in meters.
   */
  public void setRangeMeters(double meters) {
    m_simRange.set(Units.metersToInches(meters));
  }
}
