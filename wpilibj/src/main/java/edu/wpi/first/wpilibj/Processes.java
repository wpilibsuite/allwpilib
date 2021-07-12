// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.ProcessesJNI;

public final class Processes {
  /**
   * Sets the process priority for the specified process.
   *
   * @param process A substring of the process name.
   * @param realTime Set to true to set a real-time priority, false for standard priority.
   * @param priority Priority to set the process to. For real-time, this is 1-99 with 99 being
   *     highest. For non-real-time, this is forced to 0. See "man 7 sched" for more details.
   * @return True on success.
   */
  public static boolean setProcessPriority(String process, boolean realTime, int priority) {
    return ProcessesJNI.setProcessPriority(process, realTime, priority);
  }

  private Processes() {}
}
