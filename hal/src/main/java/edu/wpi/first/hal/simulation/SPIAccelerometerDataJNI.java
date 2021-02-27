// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class SPIAccelerometerDataJNI extends JNIWrapper {
  public static native int registerActiveCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelActiveCallback(int index, int uid);

  public static native boolean getActive(int index);

  public static native void setActive(int index, boolean active);

  public static native int registerRangeCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelRangeCallback(int index, int uid);

  public static native int getRange(int index);

  public static native void setRange(int index, int range);

  public static native int registerXCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelXCallback(int index, int uid);

  public static native double getX(int index);

  public static native void setX(int index, double x);

  public static native int registerYCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelYCallback(int index, int uid);

  public static native double getY(int index);

  public static native void setY(int index, double y);

  public static native int registerZCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelZCallback(int index, int uid);

  public static native double getZ(int index);

  public static native void setZ(int index, double z);

  public static native void resetData(int index);
}
