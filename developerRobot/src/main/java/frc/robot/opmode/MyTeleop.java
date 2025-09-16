// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package frc.robot.opmode;

import edu.wpi.first.wpilibj.opmode.PeriodicOpMode;
import edu.wpi.first.wpilibj.opmode.Teleoperated;

@Teleoperated
public class MyTeleop extends PeriodicOpMode {
  int m_disabledCount;
  int m_periodicCount;

  public MyTeleop() {
    System.out.println("MyTeleop constructed");
  }

  @Override
  public void close() {
    System.out.println("MyTeleop closed");
  }

  @Override
  public void disabledPeriodic() {
    System.out.println("MyTeleop disabled periodic " + m_disabledCount);
    m_disabledCount++;
  }

  @Override
  public void periodic() {
    System.out.println("MyTeleop periodic " + m_periodicCount);
    m_periodicCount++;
  }
}
