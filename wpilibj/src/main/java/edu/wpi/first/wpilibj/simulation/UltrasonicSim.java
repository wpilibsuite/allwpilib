// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.Ultrasonic;

public class UltrasonicSim {
  private final SimBoolean m_simRangeValid;
  private final SimDouble m_simRange;

  /**
   * Constructor.
   *
   * @param ultrasonic The real ultrasonic to simulate
   */
  public UltrasonicSim(Ultrasonic ultrasonic) {
    SimDeviceSim simDevice = new SimDeviceSim("Ultrasonic", ultrasonic.getEchoChannel());
    m_simRangeValid = simDevice.getBoolean("Range Valid");
    m_simRange = simDevice.getDouble("Range (in)");
  }

  public void setRangeValid(boolean valid) {
    m_simRangeValid.set(valid);
  }

  public void setRangeInches(double inches) {
    m_simRange.set(inches);
  }

  public void setRangeMeters(double meters) {
    m_simRange.set(Units.metersToInches(meters));
  }
}
