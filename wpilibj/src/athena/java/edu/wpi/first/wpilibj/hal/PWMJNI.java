/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

@SuppressWarnings("AbbreviationAsWordInName")
public class PWMJNI extends DIOJNI {
  public static native int initializePWMPort(int halPortHandle);

  public static native void freePWMPort(int pwmPortHandle);

  public static native void setPWM(int pwmPortHandle, short value);

  public static native short getPWM(int pwmPortHandle);

  public static native void latchPWMZero(int pwmPortHandle);

  public static native void setPWMPeriodScale(int pwmPortHandle, int squelchMask);
}
