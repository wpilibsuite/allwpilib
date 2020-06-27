/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal.sim;

import edu.wpi.first.hal.sim.mockdata.SimulatorJNI;

public final class SimHooks {
  private SimHooks() {
  }

  public static void setHALRuntimeType(int type) {
    SimulatorJNI.setRuntimeType(type);
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

  public static void pauseTiming() {
    SimulatorJNI.pauseTiming();
  }

  public static void resumeTiming() {
    SimulatorJNI.resumeTiming();
  }

  public static boolean isTimingPaused() {
    return SimulatorJNI.isTimingPaused();
  }

  public static void stepTiming(long delta) {
    SimulatorJNI.stepTiming(delta);
  }
}
