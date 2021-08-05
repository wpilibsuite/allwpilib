// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class PowerDistributionJNI extends JNIWrapper {
  public static native int initialize(int module, int type);

  public static native boolean checkModule(int module, int type);

  public static native boolean checkChannel(int handle, int channel);

  public static native int getType(int handle);

  public static native double getTemperature(int handle);

  public static native double getVoltage(int handle);

  public static native double getChannelCurrent(byte channel, int handle);

  public static native void getAllCurrents(int handle, double[] currents);

  public static native double getTotalCurrent(int handle);

  public static native double getTotalPower(int handle);

  public static native double getTotalEnergy(int handle);

  public static native void resetTotalEnergy(int handle);

  public static native void clearStickyFaults(int handle);
}
