// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.snippets.adxlaccelerometers;

import edu.wpi.first.wpilibj.ADXL345_I2C;
import edu.wpi.first.wpilibj.ADXL345_SPI;
import edu.wpi.first.wpilibj.ADXL362;
import edu.wpi.first.wpilibj.I2C;
import edu.wpi.first.wpilibj.SPI;
import edu.wpi.first.wpilibj.TimedRobot;

/**
 * ADXL345, 362 Accelerometer snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
public class Robot extends TimedRobot {
  // Creates an ADXL345 accelerometer object on the MXP I2C port
  // with a measurement range from -8 to 8 G's
  ADXL345_I2C m_accelerometer345I2C = new ADXL345_I2C(I2C.Port.kMXP, ADXL345_I2C.Range.k8G);

  // Creates an ADXL345 accelerometer object on the MXP SPI port
  // with a measurement range from -8 to 8 G's
  ADXL345_SPI m_accelerometer345SPI = new ADXL345_SPI(SPI.Port.kMXP, ADXL345_SPI.Range.k8G);

  // Creates an ADXL362 accelerometer object on the MXP SPI port
  // with a measurement range from -8 to 8 G's
  ADXL362 m_accelerometer362 = new ADXL362(SPI.Port.kMXP, ADXL362.Range.k8G);

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

    // Gets the current acceleration in the X axis
    m_accelerometer345SPI.getX();
    // Gets the current acceleration in the Y axis
    m_accelerometer345SPI.getY();
    // Gets the current acceleration in the Z axis
    m_accelerometer345SPI.getZ();

    // Gets the current acceleration in the X axis
    m_accelerometer362.getX();
    // Gets the current acceleration in the Y axis
    m_accelerometer362.getY();
    // Gets the current acceleration in the Z axis
    m_accelerometer362.getZ();
  }
}
