/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.FRCNetComm.tInstances;
import edu.wpi.first.wpilibj.hal.FRCNetComm.tResourceType;
import edu.wpi.first.wpilibj.hal.HAL;

/**
 * TimedRobot implements the IterativeRobotBase robot program framework.
 *
 * <p>The TimedRobot class is intended to be subclassed by a user creating a robot program.
 *
 * <p>periodic() functions from the base class are called on an interval by a Notifier instance.
 */
public class TimedRobot extends IterativeRobotBase {
  public static final double DEFAULT_PERIOD = 0.02;

  private double m_period = DEFAULT_PERIOD;

  // Prevents loop from starting if user calls setPeriod() in robotInit()
  private boolean m_startLoop = false;

  private Notifier m_loop = new Notifier(() -> {
    loopFunc();
  });

  public TimedRobot() {
    // HAL.report(tResourceType.kResourceType_Framework, tInstances.kFramework_Periodic);
    HAL.report(tResourceType.kResourceType_Framework, tInstances.kFramework_Iterative);
  }

  /**
   * Provide an alternate "main loop" via startCompetition().
   */
  public void startCompetition() {
    robotInit();

    // Tell the DS that the robot is ready to be enabled
    HAL.observeUserProgramStarting();

    // Loop forever, calling the appropriate mode-dependent function
    m_startLoop = true;
    m_loop.startPeriodic(m_period);
    while (true) {
      try {
        Thread.sleep(1000 * 60 * 60 * 24);
      } catch (InterruptedException ex) {
        Thread.currentThread().interrupt();
      }
    }
  }

  /**
   * Set time period between calls to Periodic() functions.
   *
   * @param period Period in seconds.
   */
  public void setPeriod(double period) {
    m_period = period;

    if (m_startLoop) {
      m_loop.startPeriodic(m_period);
    }
  }
}
