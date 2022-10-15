// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class InterruptJNI extends JNIWrapper {
  public static final int HalInvalidHandle = 0;

  public static native int initializeInterrupts();

  public static native void cleanInterrupts(int interruptHandle);

  public static native long waitForInterrupt(
      int interruptHandle, double timeout, boolean ignorePrevious);

  public static native long waitForMultipleInterrupts(
      int interruptHandle, long mask, double timeout, boolean ignorePrevious);

  public static native long readInterruptRisingTimestamp(int interruptHandle);

  public static native long readInterruptFallingTimestamp(int interruptHandle);

  public static native void requestInterrupts(
      int interruptHandle, int digitalSourceHandle, int analogTriggerType);

  public static native void setInterruptUpSourceEdge(
      int interruptHandle, boolean risingEdge, boolean fallingEdge);

  public static native void releaseWaitingInterrupt(int interruptHandle);
}
