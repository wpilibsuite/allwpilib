/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class SolenoidJNI extends JNIWrapper {
  public static native int initializeSolenoidPort(int halPortHandle);

  public static native boolean checkSolenoidModule(int module);

  public static native boolean checkSolenoidChannel(int channel);

  public static native void freeSolenoidPort(int portHandle);

  public static native void setSolenoid(int portHandle, boolean on);

  public static native boolean getSolenoid(int portHandle);

  public static native int getAllSolenoids(int module);

  public static native int getPCMSolenoidBlackList(int module);

  public static native boolean getPCMSolenoidVoltageStickyFault(int module);

  public static native boolean getPCMSolenoidVoltageFault(int module);

  public static native void clearAllPCMStickyFaults(int module);
}
