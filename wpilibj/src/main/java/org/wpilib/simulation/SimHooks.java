// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.simulation;

import org.wpilib.hardware.hal.ControlWord;
import org.wpilib.hardware.hal.simulation.SimulatorJNI;

/** Simulation hooks. */
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

  /** Waits until the user program has started. */
  public static void waitForProgramStart() {
    SimulatorJNI.waitForProgramStart();
  }

  /**
   * Sets flag that indicates if the user program has started.
   *
   * @param started true if started
   */
  public static void setProgramStarted(boolean started) {
    SimulatorJNI.setProgramStarted(started);
  }

  /**
   * Returns true if the user program has started.
   *
   * @return True if the user program has started.
   */
  public static boolean getProgramStarted() {
    return SimulatorJNI.getProgramStarted();
  }

  /**
   * Sets the user program state (control word).
   *
   * @param controlWord control word
   */
  public static void setProgramState(ControlWord controlWord) {
    SimulatorJNI.setProgramState(controlWord.getNative());
  }

  /**
   * Gets the user program state (control word).
   *
   * @param controlWord control word (output)
   */
  public static void getProgramState(ControlWord controlWord) {
    SimulatorJNI.getProgramState(controlWord);
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
   * @param delta the amount to advance in seconds
   */
  public static void stepTiming(double delta) {
    SimulatorJNI.stepTiming((long) (delta * 1e6));
  }

  /**
   * Advance the simulator time and return immediately.
   *
   * @param delta the amount to advance in seconds
   */
  public static void stepTimingAsync(double delta) {
    SimulatorJNI.stepTimingAsync((long) (delta * 1e6));
  }
}
