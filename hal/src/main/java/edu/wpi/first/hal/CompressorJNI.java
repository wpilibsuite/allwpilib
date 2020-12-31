// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class CompressorJNI extends JNIWrapper {
  public static native int initializeCompressor(byte module);

  public static native boolean checkCompressorModule(byte module);

  public static native boolean getCompressor(int compressorHandle);

  public static native void setCompressorClosedLoopControl(int compressorHandle, boolean value);

  public static native boolean getCompressorClosedLoopControl(int compressorHandle);

  public static native boolean getCompressorPressureSwitch(int compressorHandle);

  public static native double getCompressorCurrent(int compressorHandle);

  public static native boolean getCompressorCurrentTooHighFault(int compressorHandle);

  public static native boolean getCompressorCurrentTooHighStickyFault(int compressorHandle);

  public static native boolean getCompressorShortedStickyFault(int compressorHandle);

  public static native boolean getCompressorShortedFault(int compressorHandle);

  public static native boolean getCompressorNotConnectedStickyFault(int compressorHandle);

  public static native boolean getCompressorNotConnectedFault(int compressorHandle);

  public static native void clearAllPCMStickyFaults(byte compressorModule);
}
