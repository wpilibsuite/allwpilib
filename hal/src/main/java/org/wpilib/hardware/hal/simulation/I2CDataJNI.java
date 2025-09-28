// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal.simulation;

import org.wpilib.hardware.hal.JNIWrapper;

/** JNI for I2C data. */
public class I2CDataJNI extends JNIWrapper {
  public static native int registerInitializedCallback(
      int index, NotifyCallback callback, boolean initialNotify);

  public static native void cancelInitializedCallback(int index, int uid);

  public static native boolean getInitialized(int index);

  public static native void setInitialized(int index, boolean initialized);

  public static native int registerReadCallback(int index, BufferCallback callback);

  public static native void cancelReadCallback(int index, int uid);

  public static native int registerWriteCallback(int index, ConstBufferCallback callback);

  public static native void cancelWriteCallback(int index, int uid);

  public static native void resetData(int index);

  /** Utility class. */
  private I2CDataJNI() {}
}
