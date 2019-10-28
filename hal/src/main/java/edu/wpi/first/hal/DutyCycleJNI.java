/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

public class DutyCycleJNI extends JNIWrapper {
  public static native int initialize(int digitalSourceHandle, int analogTriggerType);
  public static native void free(int handle);

  public static native int getFrequency(int handle);
  public static native double getOutput(int handle);
  public static native int getOutputRaw(int handle);
  public static native int getOutputScaleFactor(int handle);

  @SuppressWarnings("AbbreviationAsWordInName")
  public static native int getFPGAIndex(int handle);
}
