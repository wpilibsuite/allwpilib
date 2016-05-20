/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class CompressorJNI extends JNIWrapper {
  public static native long initializeCompressor(byte module);

  public static native boolean checkCompressorModule(byte module);

  public static native boolean getCompressor(long pcmPointer);

  public static native void setClosedLoopControl(long pcmPointer, boolean value);

  public static native boolean getClosedLoopControl(long pcmPointer);

  public static native boolean getPressureSwitch(long pcmPointer);

  public static native float getCompressorCurrent(long pcmPointer);

  public static native boolean getCompressorCurrentTooHighFault(long pcmPointer);

  public static native boolean getCompressorCurrentTooHighStickyFault(long pcmPointer);

  public static native boolean getCompressorShortedStickyFault(long pcmPointer);

  public static native boolean getCompressorShortedFault(long pcmPointer);

  public static native boolean getCompressorNotConnectedStickyFault(long pcmPointer);

  public static native boolean getCompressorNotConnectedFault(long pcmPointer);

  public static native void clearAllPCMStickyFaults(long pcmPointer);
}
