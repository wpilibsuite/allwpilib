// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class PWMDataJNI extends JNIWrapper {
  public static native int registerInitializedCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedCallback(int index, int uid);

  public static native boolean getInitialized(int index);

  public static native void setInitialized(int index, boolean initialized);

  public static native int registerRawValueCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelRawValueCallback(int index, int uid);

  public static native int getRawValue(int index);

  public static native void setRawValue(int index, int rawValue);

  public static native int registerSpeedCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelSpeedCallback(int index, int uid);

  public static native double getSpeed(int index);

  public static native void setSpeed(int index, double speed);

  public static native int registerPositionCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelPositionCallback(int index, int uid);

  public static native double getPosition(int index);

  public static native void setPosition(int index, double position);

  public static native int registerPeriodScaleCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelPeriodScaleCallback(int index, int uid);

  public static native int getPeriodScale(int index);

  public static native void setPeriodScale(int index, int periodScale);

  public static native int registerZeroLatchCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelZeroLatchCallback(int index, int uid);

  public static native boolean getZeroLatch(int index);

  public static native void setZeroLatch(int index, boolean zeroLatch);

  public static native void resetData(int index);
}
