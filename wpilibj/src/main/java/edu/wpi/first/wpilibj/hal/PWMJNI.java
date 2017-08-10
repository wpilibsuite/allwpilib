/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

import edu.wpi.first.wpilibj.PWMConfigDataResult;

@SuppressWarnings("AbbreviationAsWordInName")
public class PWMJNI extends DIOJNI {
  public static native int initializePWMPort(int halPortHandle);

  public static native boolean checkPWMChannel(int channel);

  public static native void freePWMPort(int pwmPortHandle);

  public static native void setPWMConfigRaw(int pwmPortHandle, int maxPwm,
                                            int deadbandMaxPwm, int centerPwm,
                                            int deadbandMinPwm, int minPwm);

  public static native void setPWMConfig(int pwmPortHandle, double maxPwm,
                                         double deadbandMaxPwm, double centerPwm,
                                         double deadbandMinPwm, double minPwm);

  public static native PWMConfigDataResult getPWMConfigRaw(int pwmPortHandle);

  public static native void setPWMEliminateDeadband(int pwmPortHandle, boolean eliminateDeadband);

  public static native boolean getPWMEliminateDeadband(int pwmPortHandle);

  public static native void setPWMRaw(int pwmPortHandle, short value);

  public static native void setPWMSpeed(int pwmPortHandle, double speed);

  public static native void setPWMPosition(int pwmPortHandle, double position);

  public static native short getPWMRaw(int pwmPortHandle);

  public static native double getPWMSpeed(int pwmPortHandle);

  public static native double getPWMPosition(int pwmPortHandle);

  public static native  void setPWMDisabled(int pwmPortHandle);

  public static native void latchPWMZero(int pwmPortHandle);

  public static native void setPWMPeriodScale(int pwmPortHandle, int squelchMask);
}
