/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class DIOJNI extends JNIWrapper {
  public static native int initializeDIOPort(int halPortHandle, boolean input);

  public static native boolean checkDIOChannel(int channel);

  public static native void freeDIOPort(int dioPortHandle);

  public static native void setDIO(int dioPortHandle, short value);

  public static native boolean getDIO(int dioPortHandle);

  public static native boolean getDIODirection(int dioPortHandle);

  public static native void pulse(int dioPortHandle, double pulseLength);

  public static native boolean isPulsing(int dioPortHandle);

  public static native boolean isAnyPulsing();

  public static native short getLoopTiming();

  public static native int allocateDigitalPWM();

  public static native void freeDigitalPWM(int pwmGenerator);

  public static native void setDigitalPWMRate(double rate);

  public static native void setDigitalPWMDutyCycle(int pwmGenerator, double dutyCycle);

  public static native void setDigitalPWMOutputChannel(int pwmGenerator, int channel);
}
