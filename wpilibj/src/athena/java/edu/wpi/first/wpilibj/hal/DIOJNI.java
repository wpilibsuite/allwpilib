/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class DIOJNI extends JNIWrapper {
  public static native long initializeDigitalPort(long portPointer);

  public static native void freeDigitalPort(long portPointer);

  public static native boolean allocateDIO(long digitalPortPointer, boolean input);

  public static native void freeDIO(long digitalPortPointer);

  public static native void setDIO(long digitalPortPointer, short value);

  public static native boolean getDIO(long digitalPortPointer);

  public static native boolean getDIODirection(long digitalPortPointer);

  public static native void pulse(long digitalPortPointer, double pulseLength);

  public static native boolean isPulsing(long digitalPortPointer);

  public static native boolean isAnyPulsing();

  public static native short getLoopTiming();
}
