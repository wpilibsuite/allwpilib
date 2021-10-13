// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class PowerJNI extends JNIWrapper {
  public static native double getVinVoltage();

  public static native double getVinCurrent();

  public static native double getUserVoltage6V();

  public static native double getUserCurrent6V();

  public static native boolean getUserActive6V();

  public static native int getUserCurrentFaults6V();

  public static native double getUserVoltage5V();

  public static native double getUserCurrent5V();

  public static native boolean getUserActive5V();

  public static native int getUserCurrentFaults5V();

  public static native double getUserVoltage3V3();

  public static native double getUserCurrent3V3();

  public static native boolean getUserActive3V3();

  public static native int getUserCurrentFaults3V3();

  public static native void setBrownoutVoltage(double voltage);

  public static native double getBrownoutVoltage();
}
