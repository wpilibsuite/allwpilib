// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import edu.wpi.first.hal.simulation.SimulatorJNI;

public final class SimHooks {
  private SimHooks() {}

  /**
   * Override the HAL runtime type (simulated/real).
   *
   * @param type runtime type
   */
  public static void setHALRuntimeType(int type) {
    SimulatorJNI.setRuntimeType(type);
  }

  public static void waitForProgramStart() {
    SimulatorJNI.waitForProgramStart();
  }

  public static void setProgramStarted() {
    SimulatorJNI.setProgramStarted();
  }

  public static boolean getProgramStarted() {
    return SimulatorJNI.getProgramStarted();
  }

  /** Restart the simulator time. */
  public static void restartTiming() {
    SimulatorJNI.restartTiming();
  }

  /** Pause the simulator time. */
  public static void pauseTiming() {
    SimulatorJNI.pauseTiming();
  }

  /** Resume the simulator time. */
  public static void resumeTiming() {
    SimulatorJNI.resumeTiming();
  }

  /**
   * Check if the simulator time is paused.
   *
   * @return true if paused
   */
  public static boolean isTimingPaused() {
    return SimulatorJNI.isTimingPaused();
  }

  /**
   * Advance the simulator time and wait for all notifiers to run.
   *
   * @param deltaSeconds the amount to advance (in seconds)
   */
  public static void stepTiming(double deltaSeconds) {
    SimulatorJNI.stepTiming((long) (deltaSeconds * 1e6));
  }

  /**
   * Advance the simulator time and return immediately.
   *
   * @param deltaSeconds the amount to advance (in seconds)
   */
  public static void stepTimingAsync(double deltaSeconds) {
    SimulatorJNI.stepTimingAsync((long) (deltaSeconds * 1e6));
  }
}
