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
  };

  public static native long initializeInterrupts(int interruptIndex, boolean watcher);

  public static native void cleanInterrupts(long interrupt_pointer);

  public static native int waitForInterrupt(long interrupt_pointer, double timeout,
      boolean ignorePrevious);

  public static native void enableInterrupts(long interrupt_pointer);

  public static native void disableInterrupts(long interrupt_pointer);

  public static native double readRisingTimestamp(long interrupt_pointer);

  public static native double readFallingTimestamp(long interrupt_pointer);

  public static native void requestInterrupts(long interrupt_pointer, byte routing_module,
      int routing_pin, boolean routing_analog_trigger);

  public static native void attachInterruptHandler(long interrupt_pointer,
      InterruptJNIHandlerFunction handler, Object param);

  public static native void setInterruptUpSourceEdge(long interrupt_pointer, boolean risingEdge,
      boolean fallingEdge);
}
