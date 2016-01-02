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

  public static native boolean getCompressor(long pcm_pointer);

  public static native void setClosedLoopControl(long pcm_pointer, boolean value);

  public static native boolean getClosedLoopControl(long pcm_pointer);

  public static native boolean getPressureSwitch(long pcm_pointer);

  public static native float getCompressorCurrent(long pcm_pointer);

  public static native boolean getCompressorCurrentTooHighFault(long pcm_pointer);

  public static native boolean getCompressorCurrentTooHighStickyFault(long pcm_pointer);

  public static native boolean getCompressorShortedStickyFault(long pcm_pointer);

  public static native boolean getCompressorShortedFault(long pcm_pointer);

  public static native boolean getCompressorNotConnectedStickyFault(long pcm_pointer);

  public static native boolean getCompressorNotConnectedFault(long pcm_pointer);

  public static native void clearAllPCMStickyFaults(long pcm_pointer);
}
