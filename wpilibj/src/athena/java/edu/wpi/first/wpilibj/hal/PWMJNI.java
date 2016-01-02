/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.hal;

public class PWMJNI extends DIOJNI {
  public static native boolean allocatePWMChannel(long digital_port_pointer);

  public static native void freePWMChannel(long digital_port_pointer);

  public static native void setPWM(long digital_port_pointer, short value);

  public static native short getPWM(long digital_port_pointer);

  public static native void latchPWMZero(long digital_port_pointer);

  public static native void setPWMPeriodScale(long digital_port_pointer, int squelchMask);

  public static native long allocatePWM();

  public static native void freePWM(long pwmGenerator);

  public static native void setPWMRate(double rate);

  public static native void setPWMDutyCycle(long pwmGenerator, double dutyCycle);

  public static native void setPWMOutputChannel(long pwmGenerator, int pin);
}
