/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;

/**
 * IterativeRobot implements the IterativeRobotBase robot program framework.
 *
 * <p>The IterativeRobot class is intended to be subclassed by a user creating a robot program.
 *
 * <p>periodic() functions from the base class are called each time a new packet is received from
 * the driver station.
 *
 * @deprecated Use TimedRobot instead. It's a drop-in replacement that provides more regular
 *     execution periods.
 */
@Deprecated
public class IterativeRobot extends IterativeRobotBase {
  private static final double kPacketPeriod = 0.02;
  private volatile boolean m_exit;

  /**
   * Create a new IterativeRobot.
   */
  public IterativeRobot() {
    super(kPacketPeriod);

    HAL.report(tResourceType.kResourceType_Framework, tInstances.kFramework_Iterative);
  }

  /**
   * Provide an alternate "main loop" via startCompetition().
   */
  @Override
  public void startCompetition() {
    robotInit();

    // Tell the DS that the robot is ready to be enabled
    HAL.observeUserProgramStarting();

    // Loop forever, calling the appropriate mode-dependent function
    while (!Thread.currentThread().isInterrupted()) {
      // Wait for new data to arrive
      m_ds.waitForData();
      if (m_exit) {
        break;
      }

      loopFunc();
    }
  }

  /**
   * Ends the main loop in startCompetition().
   */
  @Override
  public void endCompetition() {
    m_exit = true;
    m_ds.wakeupWaitForData();
  }
}
