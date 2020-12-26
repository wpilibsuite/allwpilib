// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class InterruptJNI extends JNIWrapper {
  public static final int HalInvalidHandle = 0;

  public interface InterruptJNIHandlerFunction {
    void apply(int interruptAssertedMask, Object param);
  }

  public static native int initializeInterrupts(boolean watcher);

  public static native void cleanInterrupts(int interruptHandle);

  public static native int waitForInterrupt(int interruptHandle, double timeout,
                                            boolean ignorePrevious);

  public static native void enableInterrupts(int interruptHandle);

  public static native void disableInterrupts(int interruptHandle);

  public static native long readInterruptRisingTimestamp(int interruptHandle);

  public static native long readInterruptFallingTimestamp(int interruptHandle);

  public static native void requestInterrupts(int interruptHandle, int digitalSourceHandle,
                                              int analogTriggerType);

  public static native void attachInterruptHandler(int interruptHandle,
                                                   InterruptJNIHandlerFunction handler,
                                                   Object param);

  public static native void setInterruptUpSourceEdge(int interruptHandle, boolean risingEdge,
                                                     boolean fallingEdge);

  public static native void releaseWaitingInterrupt(int interruptHandle);
}
