// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

public class RelayDataJNI extends JNIWrapper {
  public static native int registerInitializedForwardCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedForwardCallback(int index, int uid);

  public static native boolean getInitializedForward(int index);

  public static native void setInitializedForward(int index, boolean initializedForward);

  public static native int registerInitializedReverseCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedReverseCallback(int index, int uid);

  public static native boolean getInitializedReverse(int index);

  public static native void setInitializedReverse(int index, boolean initializedReverse);

  public static native int registerForwardCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelForwardCallback(int index, int uid);

  public static native boolean getForward(int index);

  public static native void setForward(int index, boolean forward);

  public static native int registerReverseCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelReverseCallback(int index, int uid);

  public static native boolean getReverse(int index);

  public static native void setReverse(int index, boolean reverse);

  public static native void resetData(int index);
}
