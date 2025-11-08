// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.adxlaccelerometers;

import org.wpilib.hardware.accelerometer.ADXL345_I2C;
import org.wpilib.hardware.bus.I2C;
import org.wpilib.opmode.TimedRobot;

/**
 * ADXL345, 362 Accelerometer snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
public class Robot extends TimedRobot {
  // Creates an ADXL345 accelerometer object on the MXP I2C port
  // with a measurement range from -8 to 8 G's
  ADXL345_I2C m_accelerometer345I2C = new ADXL345_I2C(I2C.Port.kPort0, ADXL345_I2C.Range.k8G);

  /** Called once at the beginning of the robot program. */
  public Robot() {}

  @Override
  public void teleopPeriodic() {
    // Gets the current acceleration in the X axis
    m_accelerometer345I2C.getX();
    // Gets the current acceleration in the Y axis
    m_accelerometer345I2C.getY();
    // Gets the current acceleration in the Z axis
    m_accelerometer345I2C.getZ();
  }
}
