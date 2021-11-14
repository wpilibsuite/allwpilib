// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class REVPHJNI extends JNIWrapper {
  public static native int initialize(int module);

  public static native void free(int handle);

  public static native boolean checkSolenoidChannel(int channel);

  public static native boolean getCompressor(int handle);

  public static native void setClosedLoopControl(int handle, boolean enabled);

  public static native boolean getClosedLoopControl(int handle);

  public static native boolean getPressureSwitch(int handle);

  public static native double getAnalogPressure(int handle, int channel);

  public static native double getCompressorCurrent(int handle);

  public static native int getSolenoids(int handle);

  public static native void setSolenoids(int handle, int mask, int values);

  public static native void fireOneShot(int handle, int index, int durMs);
}
