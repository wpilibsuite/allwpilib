/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

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
