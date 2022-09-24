// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class DIOJNI extends JNIWrapper {
  public static native int initializeDIOPort(int halPortHandle, boolean input);

  public static native boolean checkDIOChannel(int channel);

  public static native void freeDIOPort(int dioPortHandle);

  public static native void setDIOSimDevice(int handle, int device);

  public static native void setDIO(int dioPortHandle, boolean value);

  public static native void setDIODirection(int dioPortHandle, boolean input);

  public static native boolean getDIO(int dioPortHandle);

  public static native boolean getDIODirection(int dioPortHandle);

  public static native void pulse(int dioPortHandle, double pulseLengthSeconds);

  public static native void pulseMultiple(long channelMask, double pulseLengthSeconds);

  public static native boolean isPulsing(int dioPortHandle);

  public static native boolean isAnyPulsing();

  public static native short getLoopTiming();

  public static native int allocateDigitalPWM();

  public static native void freeDigitalPWM(int pwmGenerator);

  public static native void setDigitalPWMRate(double rate);

  public static native void setDigitalPWMDutyCycle(int pwmGenerator, double dutyCycle);

  public static native void setDigitalPWMOutputChannel(int pwmGenerator, int channel);
}
