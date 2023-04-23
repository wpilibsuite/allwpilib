// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

public class PWMJNI extends DIOJNI {
  public static native int initializePWMPort(int halPortHandle);

  public static native boolean checkPWMChannel(int channel);

  public static native void freePWMPort(int pwmPortHandle);

  public static native void setPWMConfig(
      int pwmPortHandle,
      double maxPwm,
      double deadbandMaxPwm,
      double centerPwm,
      double deadbandMinPwm,
      double minPwm);

  public static native PWMConfigDataResult getPWMConfig(int pwmPortHandle);

  public static native void setPWMEliminateDeadband(int pwmPortHandle, boolean eliminateDeadband);

  public static native boolean getPWMEliminateDeadband(int pwmPortHandle);

  public static native void setPulseTimeMilliseconds(
      int pwmPortHandle, double millisecondPulseTime);

  public static native void setPWMSpeed(int pwmPortHandle, double speed);

  public static native void setPWMPosition(int pwmPortHandle, double position);

  public static native double getPulseTimeMilliseconds(int pwmPortHandle);

  public static native double getPWMSpeed(int pwmPortHandle);

  public static native double getPWMPosition(int pwmPortHandle);

  public static native void setPWMDisabled(int pwmPortHandle);

  public static native void latchPWMZero(int pwmPortHandle);

  public static native void setAlwaysHighMode(int pwmPortHandle);

  public static native void setPWMPeriodScale(int pwmPortHandle, int squelchMask);
}
