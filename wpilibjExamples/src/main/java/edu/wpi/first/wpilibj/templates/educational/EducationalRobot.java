// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.templates.educational;

import edu.wpi.first.hal.HAL;
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

  @SuppressWarnings("PMD.CyclomaticComplexity")
  @Override
  public void startCompetition() {
    robotInit();

    // Tell the DS that the robot is ready to be enabled
    HAL.observeUserProgramStarting();

    while (!Thread.currentThread().isInterrupted() && !m_exit) {
      if (isDisabled()) {
        m_ds.InDisabled(true);
        disabled();
        m_ds.InDisabled(false);
        while (isDisabled()) {
          m_ds.waitForData();
        }
      } else if (isAutonomous()) {
        m_ds.InAutonomous(true);
        autonomous();
        m_ds.InAutonomous(false);
        m_ds.InDisabled(true);
        while (isAutonomousEnabled()) {
          m_ds.waitForData();
        }
        m_ds.InDisabled(false);
      } else if (isTest()) {
        m_ds.InTest(true);
        test();
        m_ds.InTest(false);
        m_ds.InDisabled(true);
        while (isTest() && isEnabled()) {
          m_ds.waitForData();
        }
        m_ds.InDisabled(false);
      } else {
        m_ds.InOperatorControl(true);
        teleop();
        m_ds.InOperatorControl(false);
        m_ds.InDisabled(true);
        while (isOperatorControlEnabled()) {
          m_ds.waitForData();
        }
        m_ds.InDisabled(false);
      }
    }
  }

  @Override
  public void endCompetition() {
    m_exit = true;
  }
}
