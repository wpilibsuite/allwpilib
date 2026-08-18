// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal.simulation;

import org.wpilib.hardware.hal.JNIWrapper;

/** JNI for imu data. */
public class IMUDataJNI extends JNIWrapper {
  public static native void setAngleX(double angleRad);

  public static native void setAngleY(double angleRad);

  public static native void setAngleZ(double angleRad);

  public static native void setGyroRateX(double rateRadPerSec);

  public static native void setGyroRateY(double rateRadPerSec);

  public static native void setGyroRateZ(double rateRadPerSec);

  public static native void setAccelX(double accelMpss);

  public static native void setAccelY(double accelMpss);

  public static native void setAccelZ(double accelMpss);

  public static native void setYaw(double angleRad);
}
