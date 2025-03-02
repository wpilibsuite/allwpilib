// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

/** JNI for PWM data. */
public class PWMDataJNI extends JNIWrapper {
  public static native int registerInitializedCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedCallback(int index, int uid);

  public static native boolean getInitialized(int index);

  public static native void setInitialized(int index, boolean initialized);

  public static native int registerPulseMicrosecondCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelPulseMicrosecondCallback(int index, int uid);

  public static native int getPulseMicrosecond(int index);

  public static native void setPulseMicrosecond(int index, int microsecondPulseTime);

  public static native int registerOutputPeriodCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelOutputPeriodCallback(int index, int uid);

  public static native int getOutputPeriod(int index);

  public static native void setOutputPeriod(int index, int period);

  public static native void resetData(int index);

  /** Utility class. */
  private PWMDataJNI() {}
}
