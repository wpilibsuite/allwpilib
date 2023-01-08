// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class PowerDistributionJNI extends JNIWrapper {
  public static final int AUTOMATIC_TYPE = 0;
  public static final int CTRE_TYPE = 1;
  public static final int REV_TYPE = 2;
  public static final int DEFAULT_MODULE = -1;

  public static native int initialize(int module, int type);

  public static native void free(int handle);

  public static native int getModuleNumber(int handle);

  public static native boolean checkModule(int module, int type);

  public static native boolean checkChannel(int handle, int channel);

  public static native int getType(int handle);

  public static native int getNumChannels(int handle);

  public static native double getTemperature(int handle);

  public static native double getVoltage(int handle);

  public static native double getChannelCurrent(int handle, int channel);

  public static native void getAllCurrents(int handle, double[] currents);

  public static native double getTotalCurrent(int handle);

  public static native double getTotalPower(int handle);

  public static native double getTotalEnergy(int handle);

  public static native void resetTotalEnergy(int handle);

  public static native void clearStickyFaults(int handle);

  public static native boolean getSwitchableChannel(int handle);

  public static native void setSwitchableChannel(int handle, boolean enabled);

  public static native double getVoltageNoError(int handle);

  public static native double getChannelCurrentNoError(int handle, int channel);

  public static native double getTotalCurrentNoError(int handle);

  public static native boolean getSwitchableChannelNoError(int handle);

  public static native void setSwitchableChannelNoError(int handle, boolean enabled);

  public static native int getFaultsNative(int handle);

  public static PowerDistributionFaults getFaults(int handle) {
    return new PowerDistributionFaults(getFaultsNative(handle));
  }

  public static native int getStickyFaultsNative(int handle);

  public static PowerDistributionStickyFaults getStickyFaults(int handle) {
    return new PowerDistributionStickyFaults(getStickyFaultsNative(handle));
  }

  public static native PowerDistributionVersion getVersion(int handle);
}
