/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import java.nio.IntBuffer;

public class CounterJNI extends JNIWrapper {
  public static native int initializeCounter(int mode, IntBuffer index);

  public static native void freeCounter(int counterHandle);

  public static native void setCounterAverageSize(int counterHandle, int size);

  public static native void setCounterUpSource(int counterHandle, int digitalSourceHandle,
                                               int analogTriggerType);

  public static native void setCounterUpSourceEdge(int counterHandle, boolean risingEdge,
                                                   boolean fallingEdge);

  public static native void clearCounterUpSource(int counterHandle);

  public static native void setCounterDownSource(int counterHandle, int digitalSourceHandle,
                                                 int analogTriggerType);

  public static native void setCounterDownSourceEdge(int counterHandle, boolean risingEdge,
                                                     boolean fallingEdge);

  public static native void clearCounterDownSource(int counterHandle);

  public static native void setCounterUpDownMode(int counterHandle);

  public static native void setCounterExternalDirectionMode(int counterHandle);

  public static native void setCounterSemiPeriodMode(int counterHandle,
                                                     boolean highSemiPeriod);

  public static native void setCounterPulseLengthMode(int counterHandle, double threshold);

  public static native int getCounterSamplesToAverage(int counterHandle);

  public static native void setCounterSamplesToAverage(int counterHandle,
                                                       int samplesToAverage);

  public static native void resetCounter(int counterHandle);

  public static native int getCounter(int counterHandle);

  public static native double getCounterPeriod(int counterHandle);

  public static native void setCounterMaxPeriod(int counterHandle, double maxPeriod);

  public static native void setCounterUpdateWhenEmpty(int counterHandle, boolean enabled);

  public static native boolean getCounterStopped(int counterHandle);

  public static native boolean getCounterDirection(int counterHandle);

  public static native void setCounterReverseDirection(int counterHandle,
                                                       boolean reverseDirection);
}
