// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class AnalogGyroDataJNI extends JNIWrapper {
  public static native int registerAngleCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelAngleCallback(int index, int uid);

  public static native double getAngle(int index);

  public static native void setAngle(int index, double angle);

  public static native int registerRateCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelRateCallback(int index, int uid);

  public static native double getRate(int index);

  public static native void setRate(int index, double rate);

  public static native int registerInitializedCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedCallback(int index, int uid);

  public static native boolean getInitialized(int index);

  public static native void setInitialized(int index, boolean initialized);

  public static native void resetData(int index);
}
