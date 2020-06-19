/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.NotifierDataJNI;

/**
 * Class to control simulated notifiers.
 */
public final class NotifierSim {
  private NotifierSim() {
  }

  /**
   * Gets the timeout of the next notifier.
   *
   * @return Timestamp
   */
  public static long getNextTimeout() {
    return NotifierDataJNI.getNextTimeout();
  }

  /**
   * Gets the total number of notifiers.
   *
   * @return Count
   */
  public static int getNumNotifiers() {
    return NotifierDataJNI.getNumNotifiers();
  }
}
