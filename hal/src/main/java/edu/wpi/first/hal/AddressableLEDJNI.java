/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class AddressableLEDJNI extends JNIWrapper {
  public static native int initialize(int pwmHandle);
  public static native void free(int handle);

  public static native void setLength(int handle, int length);
  public static native void setData(int handle, byte[] data);

  public static native void setBitTiming(int handle, int lowTime0, int highTime0, int lowTime1, int highTime1);
  public static native void setSyncTime(int handle, int syncTime);

  public static native void start(int handle);
  public static native void stop(int handle);
}
