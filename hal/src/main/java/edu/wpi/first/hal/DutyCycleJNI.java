// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class DutyCycleJNI extends JNIWrapper {
  public static native int initialize(int digitalSourceHandle, int analogTriggerType);

  public static native void free(int handle);

  public static native int getFrequency(int handle);

  public static native double getOutput(int handle);

  public static native int getHighTime(int handle);

  public static native int getOutputScaleFactor(int handle);

  public static native int getFPGAIndex(int handle);
}
