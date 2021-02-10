// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class AccelerometerJNI extends JNIWrapper {
  public static native void setAccelerometerActive(boolean active);

  public static native void setAccelerometerRange(int range);

  public static native double getAccelerometerX();

  public static native double getAccelerometerY();

  public static native double getAccelerometerZ();
}
