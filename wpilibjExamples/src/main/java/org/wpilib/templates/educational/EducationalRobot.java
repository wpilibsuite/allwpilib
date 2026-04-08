// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.templates.educational;

import org.wpilib.driverstation.DriverStation;
import org.wpilib.framework.RobotBase;
import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.DriverStationJNI;
import org.wpilib.hardware.hal.RobotMode;
import org.wpilib.internal.DriverStationModeThread;
import org.wpilib.util.WPIUtilJNI;

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

  private volatile boolean m_exit;

  @Override
  public void startCompetition() {
    // Create an opmode per robot mode
    DriverStation.addOpMode(RobotMode.AUTONOMOUS, "Auto");
    DriverStation.addOpMode(RobotMode.TELEOPERATED, "Teleop");
    DriverStation.addOpMode(RobotMode.TEST, "Test");
    DriverStation.publishOpModes();

    final ControlWord word = new ControlWord();
    DriverStationModeThread modeThread = new DriverStationModeThread(word);

    int event = WPIUtilJNI.createEvent(false, false);

    DriverStation.provideRefreshedDataEventHandle(event);

    // Tell the DS that the robot is ready to be enabled
    DriverStationJNI.observeUserProgramStarting();

    while (!Thread.currentThread().isInterrupted() && !m_exit) {
      DriverStation.refreshControlWordFromCache(word);
      modeThread.inControl(word);
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
        while (isAutonomousEnabled()) {
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
        while (isTeleopEnabled()) {
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
