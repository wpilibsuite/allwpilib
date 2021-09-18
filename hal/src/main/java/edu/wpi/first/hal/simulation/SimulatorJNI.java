// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class SimulatorJNI extends JNIWrapper {
  public static native void setRuntimeType(int type);

  public static native void waitForProgramStart();

  public static native void setProgramStarted();

  public static native boolean getProgramStarted();

  public static native void restartTiming();

  public static native void pauseTiming();

  public static native void resumeTiming();

  public static native boolean isTimingPaused();

  public static native void stepTiming(long delta);

  public static native void stepTimingAsync(long delta);

  public static native void resetHandles();
}
