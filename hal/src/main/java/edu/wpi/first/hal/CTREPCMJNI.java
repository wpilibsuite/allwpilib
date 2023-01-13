// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class CTREPCMJNI extends JNIWrapper {
  public static native int initialize(int module);

  public static native void free(int handle);

  public static native boolean checkSolenoidChannel(int channel);

  public static native boolean getCompressor(int handle);

  public static native void setClosedLoopControl(int handle, boolean enabled);

  public static native boolean getClosedLoopControl(int handle);

  public static native boolean getPressureSwitch(int handle);

  public static native double getCompressorCurrent(int handle);

  public static native boolean getCompressorCurrentTooHighFault(int handle);

  public static native boolean getCompressorCurrentTooHighStickyFault(int handle);

  public static native boolean getCompressorShortedFault(int handle);

  public static native boolean getCompressorShortedStickyFault(int handle);

  public static native boolean getCompressorNotConnectedFault(int handle);

  public static native boolean getCompressorNotConnectedStickyFault(int handle);

  public static native int getSolenoids(int handle);

  public static native void setSolenoids(int handle, int mask, int values);

  public static native int getSolenoidDisabledList(int handle);

  public static native boolean getSolenoidVoltageFault(int handle);

  public static native boolean getSolenoidVoltageStickyFault(int handle);

  public static native void clearAllStickyFaults(int handle);

  public static native void fireOneShot(int handle, int index);

  public static native void setOneShotDuration(int handle, int index, int durMs);
}
