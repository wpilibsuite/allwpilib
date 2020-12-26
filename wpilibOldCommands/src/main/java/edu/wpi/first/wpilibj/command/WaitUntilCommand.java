// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.command;

import edu.wpi.first.wpilibj.Timer;

/**
 * WaitUntilCommand - waits until an absolute game time. This will wait until the game clock reaches
 * some value, then continue to the next command.
 */
public class WaitUntilCommand extends Command {
  private final double m_time;

  public WaitUntilCommand(double time) {
    super("WaitUntil(" + time + ")");
    m_time = time;
  }

  /**
   * Check if we've reached the actual finish time.
   */
  @Override
  public boolean isFinished() {
    return Timer.getMatchTime() >= m_time;
  }
}
