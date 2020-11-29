/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.wpilibj.ADXRS450_Gyro;

/**
 * Class to control a simulated ADXRS450 gyroscope.
 */
@SuppressWarnings("TypeName")
public class ADXRS450_GyroSim {
  private final SimDeviceSim m_wrappedSimDevice;
  private final SimDouble m_simAngle;
  private final SimDouble m_simRate;

  /**
   * Constructs from an ADXRS450_Gyro object.
   *
   * @param gyro ADXRS450_Gyro to simulate
   */
  public ADXRS450_GyroSim(ADXRS450_Gyro gyro) {
    m_wrappedSimDevice = new SimDeviceSim("ADXRS450_Gyro" + "[" + gyro.getPort() + "]");
    m_simAngle = m_wrappedSimDevice.getDouble("Angle");
    m_simRate = m_wrappedSimDevice.getDouble("Rate");
  }

  public void setDisplayName(String displayName) {
    m_wrappedSimDevice.setDisplayName(displayName);
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
