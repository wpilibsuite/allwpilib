// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class AddressableLEDJNI extends JNIWrapper {
  public static native int initialize(int pwmHandle);

  public static native void free(int handle);

  public static native void setLength(int handle, int length);

  public static native void setData(int handle, byte[] data);

  public static native void setBitTiming(
      int handle, int lowTime0, int highTime0, int lowTime1, int highTime1);

  public static native void setSyncTime(int handle, int syncTime);

  public static native void start(int handle);

  public static native void stop(int handle);
}
