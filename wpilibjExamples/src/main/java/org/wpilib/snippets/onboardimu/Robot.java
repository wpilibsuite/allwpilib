// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.snippets.onboardimu;

import org.wpilib.OnboardIMU;
import org.wpilib.OnboardIMU.MountOrientation;
import org.wpilib.TimedRobot;

/**
 * on board IMU snippets for frc-docs.
 * https://docs.wpilib.org/en/stable/docs/software/hardware-apis/sensors/accelerometers-software.html
 */
public class Robot extends TimedRobot {
  // Creates an object for the on board IMU
  OnboardIMU m_iMU = new OnboardIMU(MountOrientation.kFlat);

  /** Called once at the beginning of the robot program. */
  public Robot() {}

  @Override
  public void teleopPeriodic() {
    // Gets the current acceleration in the X axis
    m_iMU.getAccelX();
    // Gets the current acceleration in the Y axis
    m_iMU.getAccelY();
    // Gets the current acceleration in the Z axis
    m_iMU.getAccelZ();

    // Gets the current angle in the X axis
    m_iMU.getAngleX();
    // Gets the current angle in the Y axis
    m_iMU.getAngleY();
    // Gets the current angle in the Z axis
    m_iMU.getAngleZ();

    // Gets the current angle as a Rotation2D
    m_iMU.getRotation2d();
  }
}
