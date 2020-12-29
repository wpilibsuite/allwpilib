// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class PDPJNI extends JNIWrapper {
  public static native int initializePDP(int module);

  public static native boolean checkPDPModule(int module);

  public static native boolean checkPDPChannel(int channel);

  public static native double getPDPTemperature(int handle);

  public static native double getPDPVoltage(int handle);

  public static native double getPDPChannelCurrent(byte channel, int handle);

  public static native void getPDPAllCurrents(int handle, double[] currents);

  public static native double getPDPTotalCurrent(int handle);

  public static native double getPDPTotalPower(int handle);

  public static native double getPDPTotalEnergy(int handle);

  public static native void resetPDPTotalEnergy(int handle);

  public static native void clearPDPStickyFaults(int handle);
}
