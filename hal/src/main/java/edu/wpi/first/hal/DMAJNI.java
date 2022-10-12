// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class DMAJNI extends JNIWrapper {
  public static native int initialize();

  public static native void free(int handle);

  public static native void setPause(int handle, boolean pause);

  public static native void setTimedTrigger(int handle, double periodSeconds);

  public static native void setTimedTriggerCycles(int handle, int cycles);

  public static native void addEncoder(int handle, int encoderHandle);

  public static native void addEncoderPeriod(int handle, int encoderHandle);

  public static native void addCounter(int handle, int counterHandle);

  public static native void addCounterPeriod(int handle, int counterHandle);

  public static native void addDigitalSource(int handle, int digitalSourceHandle);

  public static native void addDutyCycle(int handle, int dutyCycleHandle);

  public static native void addAnalogInput(int handle, int analogInputHandle);

  public static native void addAveragedAnalogInput(int handle, int analogInputHandle);

  public static native void addAnalogAccumulator(int handle, int analogInputHandle);

  public static native int setExternalTrigger(
      int handle, int digitalSourceHandle, int analogTriggerType, boolean rising, boolean falling);

  public static native void clearSensors(int handle);

  public static native void clearExternalTriggers(int handle);

  public static native void startDMA(int handle, int queueDepth);

  public static native void stopDMA(int handle);

  // 0-21 channelOffsets
  // 22: capture size
  // 23: triggerChannels (bitflags)
  // 24: remaining
  // 25: read status
  public static native long readDMA(
      int handle, double timeoutSeconds, int[] buffer, int[] sampleStore);

  public static native DMAJNISample.BaseStore getSensorReadData(int handle);
}
