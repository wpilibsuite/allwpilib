// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.math.util.Units;
import edu.wpi.first.wpilibj.SharpIR;

public class SharpIRSim {
  private final SimDouble m_simRange;

  /**
   * Constructor.
   *
   * @param sharpIR The real sensor to simulate
   */
  public SharpIRSim(SharpIR sharpIR) {
    this(sharpIR.getChannel());
  }

  /**
   * Constructor.
   *
   * @param channel Analog channel for this sensor
   */
  public SharpIRSim(int channel) {
    SimDeviceSim simDevice = new SimDeviceSim("SharpIR", channel);
    m_simRange = simDevice.getDouble("Range (cm)");
  }

  public void setRangeInches(double inches) {
    m_simRange.set(Units.inchesToMeters(inches) * 100.0);
  }

  public void setRangeCm(double cm) {
    m_simRange.set(cm);
  }
}
