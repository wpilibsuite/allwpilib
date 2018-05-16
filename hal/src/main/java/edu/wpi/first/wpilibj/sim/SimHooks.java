/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.sim;

import edu.wpi.first.hal.sim.mockdata.SimulatorJNI;

public class SimHooks {
  private SimHooks() {
  }

  public static void waitForProgramStart() {
    SimulatorJNI.waitForProgramStart();
  }

  public static void setProgramStarted() {
    SimulatorJNI.setProgramStarted();
  }

  public static void restartTiming() {
    SimulatorJNI.restartTiming();
  }
}
