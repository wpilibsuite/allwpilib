// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class AnalogGyroJNI extends JNIWrapper {
  public static native int initializeAnalogGyro(int halAnalogInputHandle);

  public static native void setupAnalogGyro(int handle);

  public static native void freeAnalogGyro(int handle);

  public static native void setAnalogGyroParameters(
      int handle, double voltsPerDegreePerSecond, double offset, int center);

  public static native void setAnalogGyroVoltsPerDegreePerSecond(
      int handle, double voltsPerDegreePerSecond);

  public static native void resetAnalogGyro(int handle);

  public static native void calibrateAnalogGyro(int handle);

  public static native void setAnalogGyroDeadband(int handle, double volts);

  public static native double getAnalogGyroAngle(int handle);

  public static native double getAnalogGyroRate(int handle);

  public static native double getAnalogGyroOffset(int handle);

  public static native int getAnalogGyroCenter(int handle);
}
