/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class PDPJNI extends JNIWrapper {
  public static native void initializePDP(int module);

  public static native double getPDPTemperature(int module);

  public static native double getPDPVoltage(int module);

  public static native double getPDPChannelCurrent(byte channel, int module);

  public static native double getPDPTotalCurrent(int module);

  public static native double getPDPTotalPower(int module);

  public static native double getPDPTotalEnergy(int module);

  public static native void resetPDPTotalEnergy(int module);

  public static native void clearPDPStickyFaults(int module);
}
