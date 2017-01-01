/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
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
                                                    double voltsPerDegreePerSecond,
                                                    double offset, int center);

  public static native void setAnalogGyroVoltsPerDegreePerSecond(int handle,
                                                                 double voltsPerDegreePerSecond);

  public static native void resetAnalogGyro(int handle);

  public static native void calibrateAnalogGyro(int handle);

  public static native void setAnalogGyroDeadband(int handle, double volts);

  public static native double getAnalogGyroAngle(int handle);

  public static native double getAnalogGyroRate(int handle);

  public static native double getAnalogGyroOffset(int handle);

  public static native int getAnalogGyroCenter(int handle);
}
