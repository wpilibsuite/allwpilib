/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command;

import edu.wpi.first.wpilibj.Timer;

/**
 * Class that holds scheduling state for a command.  Used internally by the
 * {@link CommandScheduler}.
 */
class CommandState {
  //The time since this command was initialized.
  private double m_startTime = -1;

  //Whether or not it is interruptible.
  private final boolean m_interruptible;

  CommandState(boolean interruptible) {
    m_interruptible = interruptible;
    startTiming();
    startRunning();
  }

  private void startTiming() {
    m_startTime = Timer.getFPGATimestamp();
  }

  synchronized void startRunning() {
    m_startTime = -1;
  }

  boolean isInterruptible() {
    return m_interruptible;
  }

  double timeSinceInitialized() {
    return m_startTime != -1 ? Timer.getFPGATimestamp() - m_startTime : -1;
  }
}
