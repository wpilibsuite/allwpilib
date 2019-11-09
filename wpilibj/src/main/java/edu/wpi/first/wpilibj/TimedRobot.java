/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.FRCNetComm.tInstances;
import edu.wpi.first.hal.FRCNetComm.tResourceType;
import edu.wpi.first.hal.HAL;
import edu.wpi.first.hal.NotifierJNI;

/**
 * TimedRobot implements the IterativeRobotBase robot program framework.
 *
 * <p>The TimedRobot class is intended to be subclassed by a user creating a robot program.
 *
 * <p>periodic() functions from the base class are called on an interval by a Notifier instance.
 */
public class TimedRobot extends IterativeRobotBase {
  public static final double kDefaultPeriodSeconds = 0.02;

  // The C pointer to the notifier object. We don't use it directly, it is
  // just passed to the JNI bindings.
  private final int m_notifier = NotifierJNI.initializeNotifier();

  // The absolute expiration time in seconds
  private double m_expirationTimeSeconds;

  /**
   * Constructor for TimedRobot.
   */
  protected TimedRobot() {
    this(kDefaultPeriodSeconds);
  }

  /**
   * Constructor for TimedRobot.
   *
   * @param periodSeconds Period in seconds.
   */
  protected TimedRobot(double periodSeconds) {
    super(periodSeconds);

    HAL.report(tResourceType.kResourceType_Framework, tInstances.kFramework_Timed);
  }

  @Override
  @SuppressWarnings("NoFinalizer")
  protected void finalize() {
    NotifierJNI.stopNotifier(m_notifier);
    NotifierJNI.cleanNotifier(m_notifier);
  }

  /**
   * Provide an alternate "main loop" via startCompetition().
   */
  @Override
  @SuppressWarnings("UnsafeFinalization")
  public void startCompetition() {
    robotInit();

    // Tell the DS that the robot is ready to be enabled
    HAL.observeUserProgramStarting();

    m_expirationTimeSeconds = RobotController.getFPGATimeMicroSeconds() * 1e-6 + m_periodSeconds;
    updateAlarm();

    // Loop forever, calling the appropriate mode-dependent function
    while (true) {
      long curTime = NotifierJNI.waitForNotifierAlarm(m_notifier);
      if (curTime == 0) {
        break;
      }

      m_expirationTimeSeconds += m_periodSeconds;
      updateAlarm();

      loopFunc();
    }
  }

  /**
   * Ends the main loop in startCompetition().
   */
  @Override
  public void endCompetition() {
    NotifierJNI.stopNotifier(m_notifier);
  }

  /**
   * Get time period between calls to Periodic() functions.
   *
   * @return the time period between calls in seconds.
   */
  public double getPeriodSeconds() {
    return m_periodSeconds;
  }

  /**
   * Update the alarm hardware to reflect the next alarm.
   */
  @SuppressWarnings("UnsafeFinalization")
  private void updateAlarm() {
    NotifierJNI.updateNotifierAlarm(m_notifier, (long) (m_expirationTimeSeconds * 1e6));
  }
}
