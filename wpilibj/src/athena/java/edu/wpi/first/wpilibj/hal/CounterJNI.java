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

  public static native void freeCounter(long counter_pointer);

  public static native void setCounterAverageSize(long counter_pointer, int size);

  public static native void setCounterUpSource(long counter_pointer, int pin,
      boolean analogTrigger);

  public static native void setCounterUpSourceEdge(long counter_pointer, boolean risingEdge,
      boolean fallingEdge);

  public static native void clearCounterUpSource(long counter_pointer);

  public static native void setCounterDownSource(long counter_pointer, int pin,
      boolean analogTrigger);

  public static native void setCounterDownSourceEdge(long counter_pointer, boolean risingEdge,
      boolean fallingEdge);

  public static native void clearCounterDownSource(long counter_pointer);

  public static native void setCounterUpDownMode(long counter_pointer);

  public static native void setCounterExternalDirectionMode(long counter_pointer);

  public static native void setCounterSemiPeriodMode(long counter_pointer,
      boolean highSemiPeriod);

  public static native void setCounterPulseLengthMode(long counter_pointer, double threshold);

  public static native int getCounterSamplesToAverage(long counter_pointer);

  public static native void setCounterSamplesToAverage(long counter_pointer,
      int samplesToAverage);

  public static native void resetCounter(long counter_pointer);

  public static native int getCounter(long counter_pointer);

  public static native double getCounterPeriod(long counter_pointer);

  public static native void setCounterMaxPeriod(long counter_pointer, double maxPeriod);

  public static native void setCounterUpdateWhenEmpty(long counter_pointer, boolean enabled);

  public static native boolean getCounterStopped(long counter_pointer);

  public static native boolean getCounterDirection(long counter_pointer);

  public static native void setCounterReverseDirection(long counter_pointer,
      boolean reverseDirection);
}
