// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class PWMJNI extends DIOJNI {
  public static native int initializePWMPort(int halPortHandle);

  public static native boolean checkPWMChannel(int channel);

  public static native void freePWMPort(int pwmPortHandle);

  public static native void setPWMConfigMicroseconds(
      int pwmPortHandle,
      int maxPwm,
      int deadbandMaxPwm,
      int centerPwm,
      int deadbandMinPwm,
      int minPwm);

  public static native PWMConfigDataResult getPWMConfigMicroseconds(int pwmPortHandle);

  public static native void setPWMEliminateDeadband(int pwmPortHandle, boolean eliminateDeadband);

  public static native boolean getPWMEliminateDeadband(int pwmPortHandle);

  public static native void setPulseTimeMicroseconds(int pwmPortHandle, int microsecondPulseTime);

  public static native void setPWMSpeed(int pwmPortHandle, double speed);

  public static native void setPWMPosition(int pwmPortHandle, double position);

  public static native int getPulseTimeMicroseconds(int pwmPortHandle);

  public static native double getPWMSpeed(int pwmPortHandle);

  public static native double getPWMPosition(int pwmPortHandle);

  public static native void setPWMDisabled(int pwmPortHandle);

  public static native void latchPWMZero(int pwmPortHandle);

  public static native void setAlwaysHighMode(int pwmPortHandle);

  public static native void setPWMPeriodScale(int pwmPortHandle, int squelchMask);
}
