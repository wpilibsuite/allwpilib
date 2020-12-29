// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class AnalogOutDataJNI extends JNIWrapper {
  public static native int registerVoltageCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelVoltageCallback(int index, int uid);

  public static native double getVoltage(int index);

  public static native void setVoltage(int index, double voltage);

  public static native int registerInitializedCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedCallback(int index, int uid);

  public static native boolean getInitialized(int index);

  public static native void setInitialized(int index, boolean initialized);

  public static native void resetData(int index);
}
