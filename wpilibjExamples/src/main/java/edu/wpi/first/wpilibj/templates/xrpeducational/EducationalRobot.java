// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.templates.xrpeducational;

import edu.wpi.first.hal.DriverStationJNI;
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.wpilibj.DriverStation;
import edu.wpi.first.wpilibj.RobotBase;
import edu.wpi.first.wpilibj.internal.DriverStationModeThread;

/** Educational robot base class. */
public class EducationalRobot extends RobotBase {
  public EducationalRobot() {}

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

  public boolean isDisabled() {
    return DriverStation.getOpModeId() == 0;
  }

  public boolean isAutonomous() {
    return DriverStation.getOpModeId() == m_autoMode;
  }

  public boolean isTeleop() {
    return DriverStation.getOpModeId() == m_teleopMode;
  }

  public boolean isTest() {
    return DriverStation.getOpModeId() == m_testMode;
  }

  private volatile boolean m_exit;
  private int m_autoMode = -1;
  private int m_teleopMode = -1;
  private int m_testMode = -1;

  @Override
  public void startCompetition() {
    DriverStationModeThread modeThread = new DriverStationModeThread();

    int event = WPIUtilJNI.createEvent(false, false);

    DriverStation.provideRefreshedDataEventHandle(event);

    m_autoMode = DriverStation.addOpModeOption("auto", "", "", 0);
    m_teleopMode = DriverStation.addOpModeOption("teleop", "", "", 0);
    m_testMode = DriverStation.addOpModeOption("test", "", "", 0);

    // Tell the DS that the robot is ready to be enabled
    DriverStationJNI.observeUserProgramStarting();

    while (!Thread.currentThread().isInterrupted() && !m_exit) {
      int mode = DriverStation.getOpModeId();
      modeThread.inOpMode(mode);
      if (isDisabled()) {
        disabled();
        while (isDisabled()) {
          try {
            WPIUtilJNI.waitForObject(event);
          } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
          }
        }
      } else if (isAutonomous()) {
        autonomous();
        while (isAutonomous()) {
          try {
            WPIUtilJNI.waitForObject(event);
          } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
          }
        }
      } else if (isTest()) {
        test();
        while (isTest() && isEnabled()) {
          try {
            WPIUtilJNI.waitForObject(event);
          } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
          }
        }
      } else {
        teleop();
        while (isTeleop()) {
          try {
            WPIUtilJNI.waitForObject(event);
          } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
          }
        }
      }
    }

    DriverStation.removeRefreshedDataEventHandle(event);
    modeThread.close();
  }

  @Override
  public void endCompetition() {
    m_exit = true;
  }
}
