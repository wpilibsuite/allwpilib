// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

import java.nio.IntBuffer;

public class CounterJNI extends JNIWrapper {
  public static final int TWO_PULSE = 0;
  public static final int SEMI_PERIOD = 1;
  public static final int PULSE_LENGTH = 2;
  public static final int EXTERNAL_DIRECTION = 3;

  public static native int initializeCounter(int mode, IntBuffer index);

  public static native void freeCounter(int counterHandle);

  public static native void setCounterAverageSize(int counterHandle, int size);

  public static native void setCounterUpSource(
      int counterHandle, int digitalSourceHandle, int analogTriggerType);

  public static native void setCounterUpSourceEdge(
      int counterHandle, boolean risingEdge, boolean fallingEdge);

  public static native void clearCounterUpSource(int counterHandle);

  public static native void setCounterDownSource(
      int counterHandle, int digitalSourceHandle, int analogTriggerType);

  public static native void setCounterDownSourceEdge(
      int counterHandle, boolean risingEdge, boolean fallingEdge);

  public static native void clearCounterDownSource(int counterHandle);

  public static native void setCounterUpDownMode(int counterHandle);

  public static native void setCounterExternalDirectionMode(int counterHandle);

  public static native void setCounterSemiPeriodMode(int counterHandle, boolean highSemiPeriod);

  public static native void setCounterPulseLengthMode(int counterHandle, double threshold);

  public static native int getCounterSamplesToAverage(int counterHandle);

  public static native void setCounterSamplesToAverage(int counterHandle, int samplesToAverage);

  public static native void resetCounter(int counterHandle);

  public static native int getCounter(int counterHandle);

  public static native double getCounterPeriod(int counterHandle);

  public static native void setCounterMaxPeriod(int counterHandle, double maxPeriod);

  public static native void setCounterUpdateWhenEmpty(int counterHandle, boolean enabled);

  public static native boolean getCounterStopped(int counterHandle);

  public static native boolean getCounterDirection(int counterHandle);

  public static native void setCounterReverseDirection(int counterHandle, boolean reverseDirection);
}
