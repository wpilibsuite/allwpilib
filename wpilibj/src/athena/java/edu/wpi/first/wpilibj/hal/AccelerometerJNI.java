/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class AccelerometerJNI extends JNIWrapper {
  public static native void setAccelerometerActive(boolean active);

  public static native void setAccelerometerRange(int range);

  public static native double getAccelerometerX();

  public static native double getAccelerometerY();

  public static native double getAccelerometerZ();
}
