// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

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

  public static native void setOneShotDuration(int portHandle, long durationMS);

  public static native void fireOneShot(int portHandle);
}
