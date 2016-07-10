/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class AnalogGyroJNI extends JNIWrapper {
  public static native int initializeAnalogGyro(int halAnalogInputHandle);

  public static native void setupAnalogGyro(int handle);

  public static native void freeAnalogGyro(int handle);

  public static native void setAnalogGyroParameters(int handle,
                                                    float voltsPerDegreePerSecond,
                                                    float offset, int center);

  public static native void setAnalogGyroVoltsPerDegreePerSecond(int handle,
                                                                 float voltsPerDegreePerSecond);

  public static native void resetAnalogGyro(int handle);

  public static native void calibrateAnalogGyro(int handle);

  public static native void setAnalogGyroDeadband(int handle, float volts);

  public static native float getAnalogGyroAngle(int handle);

  public static native double getAnalogGyroRate(int handle);

  public static native float getAnalogGyroOffset(int handle);

  public static native int getAnalogGyroCenter(int handle);
}
