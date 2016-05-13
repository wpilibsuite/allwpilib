/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class InterruptJNI extends JNIWrapper {
  public interface InterruptJNIHandlerFunction {
    void apply(int interruptAssertedMask, Object param);
  }

  public static native long initializeInterrupts(int interruptIndex, boolean watcher);

  public static native void cleanInterrupts(long interruptPointer);

  public static native int waitForInterrupt(long interruptPointer, double timeout,
                                            boolean ignorePrevious);

  public static native void enableInterrupts(long interruptPointer);

  public static native void disableInterrupts(long interruptPointer);

  public static native double readRisingTimestamp(long interruptPointer);

  public static native double readFallingTimestamp(long interruptPointer);

  public static native void requestInterrupts(long interruptPointer, byte routingModule,
                                              int routingPin, boolean routingAnalogTrigger);

  public static native void attachInterruptHandler(long interruptPointer,
                                                   InterruptJNIHandlerFunction handler,
                                                   Object param);

  public static native void setInterruptUpSourceEdge(long interruptPointer, boolean risingEdge,
                                                     boolean fallingEdge);
}
