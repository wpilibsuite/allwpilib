// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.xrp;

import org.wpilib.AnalogInput;

/** This class represents the ultrasonic rangefinder on an XRP robot. */
public class XRPRangefinder {
  private final AnalogInput m_rangefinder = new AnalogInput(2);

  /**
   * Constructs an XRPRangefinder.
   *
   * <p>Only one instance of a XRPRangefinder is supported.
   */
  public XRPRangefinder() {}

  /**
   * Get the measured distance in meters. Distance further than 4m will be reported as 4m.
   *
   * @return distance in meters
   */
  public double getDistance() {
    return (m_rangefinder.getVoltage() / 5.0) * 4.0;
  }
}
