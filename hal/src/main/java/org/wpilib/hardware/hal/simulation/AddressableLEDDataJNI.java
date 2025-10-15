// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal.simulation;

import edu.wpi.first.hal.JNIWrapper;

/** JNI for addressable LED data. */
public class AddressableLEDDataJNI extends JNIWrapper {
  public static native int registerInitializedCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedCallback(int index, int uid);

  public static native boolean getInitialized(int index);

  public static native void setInitialized(int index, boolean initialized);

  public static native int registerStartCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelStartCallback(int index, int uid);

  public static native int getStart(int index);

  public static native void setStart(int index, int start);

  public static native int registerLengthCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelLengthCallback(int index, int uid);

  public static native int getLength(int index);

  public static native void setLength(int index, int length);

  public static native int registerDataCallback(ConstBufferCallback callback);

  public static native void cancelDataCallback(int uid);

  public static native byte[] getData(int start, int length);

  public static native void setData(int start, byte[] data);

  public static native void resetData(int index);

  /** Utility class. */
  private AddressableLEDDataJNI() {}
}
