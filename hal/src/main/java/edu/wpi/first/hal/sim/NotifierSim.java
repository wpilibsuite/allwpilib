/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.sim;

import edu.wpi.first.hal.sim.mockdata.NotifierDataJNI;

public final class NotifierSim {
  private NotifierSim() {
  }

  public static long getNextTimeout() {
    return NotifierDataJNI.getNextTimeout();
  }

  public static int getNumNotifiers() {
    return NotifierDataJNI.getNumNotifiers();
  }
}
