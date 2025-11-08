// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.xrp;

import edu.wpi.first.wpilibj.AnalogInput;

/** This class represents the reflectance sensor pair on an XRP robot. */
public class XRPReflectanceSensor {
  private final AnalogInput m_leftSensor = new AnalogInput(0);
  private final AnalogInput m_rightSensor = new AnalogInput(1);

  /**
   * Constructs an XRPReflectanceSensor.
   *
   * <p>Only one instance of a XRPReflectanceSensor is supported.
   */
  public XRPReflectanceSensor() {}

  /**
   * Returns the reflectance value of the left sensor.
   *
   * @return value between 0.0 (white) and 1.0 (black).
   */
  public double getLeftReflectanceValue() {
    return m_leftSensor.getVoltage() / 5.0;
  }

  /**
   * Returns the reflectance value of the right sensor.
   *
   * @return value between 0.0 (white) and 1.0 (black).
   */
  public double getRightReflectanceValue() {
    return m_rightSensor.getVoltage() / 5.0;
  }
}
