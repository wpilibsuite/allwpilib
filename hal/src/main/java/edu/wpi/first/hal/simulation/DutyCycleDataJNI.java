// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class DutyCycleDataJNI extends JNIWrapper {
  public static native int registerInitializedCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedCallback(int index, int uid);

  public static native boolean getInitialized(int index);

  public static native void setInitialized(int index, boolean initialized);

  public static native int registerFrequencyCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelFrequencyCallback(int index, int uid);

  public static native int getFrequency(int index);

  public static native void setFrequency(int index, int frequency);

  public static native int registerOutputCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelOutputCallback(int index, int uid);

  public static native double getOutput(int index);

  public static native void setOutput(int index, double output);

  public static native void resetData(int index);

  public static native int findForChannel(int channel);
}
