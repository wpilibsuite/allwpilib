/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;

public class CounterJNI extends JNIWrapper {
  public static native long initializeCounter(int mode, IntBuffer index);

  public static native void freeCounter(long counterPointer);

  public static native void setCounterAverageSize(long counterPointer, int size);

  public static native void setCounterUpSource(long counterPointer, int pin,
                                               boolean analogTrigger);

  public static native void setCounterUpSourceEdge(long counterPointer, boolean risingEdge,
                                                   boolean fallingEdge);

  public static native void clearCounterUpSource(long counterPointer);

  public static native void setCounterDownSource(long counterPointer, int pin,
                                                 boolean analogTrigger);

  public static native void setCounterDownSourceEdge(long counterPointer, boolean risingEdge,
                                                     boolean fallingEdge);

  public static native void clearCounterDownSource(long counterPointer);

  public static native void setCounterUpDownMode(long counterPointer);

  public static native void setCounterExternalDirectionMode(long counterPointer);

  public static native void setCounterSemiPeriodMode(long counterPointer,
                                                     boolean highSemiPeriod);

  public static native void setCounterPulseLengthMode(long counterPointer, double threshold);

  public static native int getCounterSamplesToAverage(long counterPointer);

  public static native void setCounterSamplesToAverage(long counterPointer,
                                                       int samplesToAverage);

  public static native void resetCounter(long counterPointer);

  public static native int getCounter(long counterPointer);

  public static native double getCounterPeriod(long counterPointer);

  public static native void setCounterMaxPeriod(long counterPointer, double maxPeriod);

  public static native void setCounterUpdateWhenEmpty(long counterPointer, boolean enabled);

  public static native boolean getCounterStopped(long counterPointer);

  public static native boolean getCounterDirection(long counterPointer);

  public static native void setCounterReverseDirection(long counterPointer,
                                                       boolean reverseDirection);
}
