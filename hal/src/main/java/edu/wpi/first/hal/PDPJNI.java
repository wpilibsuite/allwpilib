/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class PDPJNI extends JNIWrapper {
  public static native int initializePDP(int module);

  public static native boolean checkPDPModule(int module);

  public static native boolean checkPDPChannel(int channel);

  public static native double getPDPTemperature(int handle);

  public static native double getPDPVoltage(int handle);

  public static native double getPDPChannelCurrent(byte channel, int handle);

  public static native double getPDPTotalCurrent(int handle);

  public static native double getPDPTotalPower(int handle);

  public static native double getPDPTotalEnergy(int handle);

  public static native void resetPDPTotalEnergy(int handle);

  public static native void clearPDPStickyFaults(int handle);
}
