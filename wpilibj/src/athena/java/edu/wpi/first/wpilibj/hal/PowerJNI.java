/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

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
}
