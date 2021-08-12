// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.templates.robotbaseskeleton;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.RobotBase;
import edu.wpi.first.wpilibj.livewindow.LiveWindow;
import edu.wpi.first.wpilibj.shuffleboard.Shuffleboard;

/**
 * The VM is configured to automatically run this class. If you change the name of this class or the
 * package after creating this project, you must also update the build.gradle file in the project.
 */
public class Robot extends RobotBase {
  public void robotInit() {}

  public void disabled() {}

  public void autonomous() {}

  public void teleop() {}

  public void test() {}

  private volatile boolean m_exit;

  @Override
  public void startCompetition() {
    robotInit();

    // Tell the DS that the robot is ready to be enabled
    HAL.observeUserProgramStarting();

    while (!Thread.currentThread().isInterrupted() && !m_exit) {
      if (isDisabled()) {
        DriverStation.inDisabled(true);
        disabled();
        DriverStation.inDisabled(false);
        while (isDisabled()) {
          DriverStation.waitForData();
        }
      } else if (isAutonomous()) {
        DriverStation.inAutonomous(true);
        autonomous();
        DriverStation.inAutonomous(false);
        while (isAutonomousEnabled()) {
          DriverStation.waitForData();
        }
      } else if (isTest()) {
        LiveWindow.setEnabled(true);
        Shuffleboard.enableActuatorWidgets();
        DriverStation.inTest(true);
        test();
        DriverStation.inTest(false);
        while (isTest() && isEnabled()) {
          DriverStation.waitForData();
        }
        LiveWindow.setEnabled(false);
        Shuffleboard.disableActuatorWidgets();
      } else {
        DriverStation.inTeleop(true);
        teleop();
        DriverStation.inTeleop(false);
        while (isTeleopEnabled()) {
          DriverStation.waitForData();
        }
      }
    }
  }

  @Override
  public void endCompetition() {
    m_exit = true;
  }
}
