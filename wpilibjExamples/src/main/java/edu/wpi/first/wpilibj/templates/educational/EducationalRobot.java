// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.templates.educational;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.RobotBase;

/** Educational robot base class. */
public class EducationalRobot extends RobotBase {
  public void robotInit() {}

  public void disabled() {}

  public void run() {}

  public void autonomous() {
    run();
  }

  public void teleop() {
    run();
  }

  public void test() {
    run();
  }

  private volatile boolean m_exit;

  @Override
  public void startCompetition() {
    robotInit();

    int event = WPIUtilJNI.createEvent(false, false);

    DriverStation.provideRefreshedDataEventHandle(event);

    // Tell the DS that the robot is ready to be enabled
    DriverStationJNI.observeUserProgramStarting();

    while (!Thread.currentThread().isInterrupted() && !m_exit) {
      DriverStation.refreshData();
      if (isDisabled()) {
        disabled();
        DriverStationJNI.observeUserProgramDisabled();
        while (isDisabled()) {
          try {
            WPIUtilJNI.waitForObject(event);
          } catch (InterruptedException e) {
          }
        }
      } else if (isAutonomous()) {
        autonomous();
        DriverStationJNI.observeUserProgramAutonomous();
        while (isAutonomousEnabled()) {
          try {
            WPIUtilJNI.waitForObject(event);
          } catch (InterruptedException e) {
          }
        }
      } else if (isTest()) {
        test();
        DriverStationJNI.observeUserProgramTest();
        while (isTest() && isEnabled()) {
          try {
            WPIUtilJNI.waitForObject(event);
          } catch (InterruptedException e) {
          }
        }
      } else {
        teleop();
        DriverStationJNI.observeUserProgramTeleop();
        while (isTeleopEnabled()) {
          try {
            WPIUtilJNI.waitForObject(event);
          } catch (InterruptedException e) {
          }
        }
      }
    }

    DriverStation.removeRefreshedDataEventHandle(event);
  }

  @Override
  public void endCompetition() {
    m_exit = true;
  }
}
