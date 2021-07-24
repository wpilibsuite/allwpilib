// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class REVPHJNI extends JNIWrapper {
  public static final int COMPRESSOR_CONFIG_TYPE_DISABLED = 0;
  public static final int COMPRESSOR_CONFIG_TYPE_DIGITAL = 1;
  public static final int COMPRESSOR_CONFIG_TYPE_ANALOG = 2;
  public static final int COMPRESSOR_CONFIG_TYPE_HYBRID = 3;

  public static native int initialize(int module);

  public static native void free(int handle);

  public static native boolean checkSolenoidChannel(int channel);

  public static native boolean getCompressor(int handle);

  public static native void setCompressorConfig(
      int handle,
      double minAnalogVoltage,
      double maxAnalogVoltage,
      boolean forceDisable,
      boolean useDigital);

  public static native void setClosedLoopControlDisabled(int handle);

  public static native void setClosedLoopControlDigital(int handle);

  public static native void setClosedLoopControlAnalog(
      int handle, double minAnalogVoltage, double maxAnalogVoltage);

  public static native void setClosedLoopControlHybrid(
      int handle, double minAnalogVoltage, double maxAnalogVoltage);

  public static native int getCompressorConfig(int handle);

  public static native boolean getPressureSwitch(int handle);

  public static native double getAnalogVoltage(int handle, int channel);

  public static native double getCompressorCurrent(int handle);

  public static native int getSolenoids(int handle);

  public static native void setSolenoids(int handle, int mask, int values);

  public static native void fireOneShot(int handle, int index, int durMs);

  public static native void clearStickyFaults(int handle);

  public static native double getInputVoltage(int handle);

  public static native double get5VVoltage(int handle);

  public static native double getSolenoidCurrent(int handle);

  public static native double getSolenoidVoltage(int handle);

  public static native int getStickyFaultsNative(int handle);

  public static REVPHStickyFaults getStickyFaults(int handle) {
    return new REVPHStickyFaults(getStickyFaultsNative(handle));
  }

  public static native int getFaultsNative(int handle);

  public static REVPHFaults getFaults(int handle) {
    return new REVPHFaults(getFaultsNative(handle));
  }

  public static native REVPHVersion getVersion(int handle);
}
