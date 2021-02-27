// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.ADXRS450_Gyro;

/** Class to control a simulated ADXRS450 gyroscope. */
@SuppressWarnings({"TypeName", "AbbreviationAsWordInName"})
public class ADXRS450_GyroSim {
  private final SimDouble m_simAngle;
  private final SimDouble m_simRate;

  /**
   * Constructs from an ADXRS450_Gyro object.
   *
   * @param gyro ADXRS450_Gyro to simulate
   */
  public ADXRS450_GyroSim(ADXRS450_Gyro gyro) {
    SimDeviceSim wrappedSimDevice = new SimDeviceSim("Gyro:ADXRS450" + "[" + gyro.getPort() + "]");
    m_simAngle = wrappedSimDevice.getDouble("angle_x");
    m_simRate = wrappedSimDevice.getDouble("rate_x");
  }

  /**
   * Sets the angle in degrees (clockwise positive).
   *
   * @param angleDegrees The angle.
   */
  public void setAngle(double angleDegrees) {
    m_simAngle.set(angleDegrees);
  }

  /**
   * Sets the angular rate in degrees per second (clockwise positive).
   *
   * @param rateDegreesPerSecond The angular rate.
   */
  public void setRate(double rateDegreesPerSecond) {
    m_simRate.set(rateDegreesPerSecond);
  }
}
