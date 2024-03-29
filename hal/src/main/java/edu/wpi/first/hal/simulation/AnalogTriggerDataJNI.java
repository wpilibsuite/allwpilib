// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

/** JNI for analog trigger data. */
public class AnalogTriggerDataJNI extends JNIWrapper {
  public static native int registerInitializedCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedCallback(int index, int uid);

  public static native boolean getInitialized(int index);

  public static native void setInitialized(int index, boolean initialized);

  public static native int registerTriggerLowerBoundCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelTriggerLowerBoundCallback(int index, int uid);

  public static native double getTriggerLowerBound(int index);

  public static native void setTriggerLowerBound(int index, double triggerLowerBound);

  public static native int registerTriggerUpperBoundCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelTriggerUpperBoundCallback(int index, int uid);

  public static native double getTriggerUpperBound(int index);

  public static native void setTriggerUpperBound(int index, double triggerUpperBound);

  public static native void resetData(int index);

  public static native int findForChannel(int channel);

  /** Utility class. */
  private AnalogTriggerDataJNI() {}
}
