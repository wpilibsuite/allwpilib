// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.hal;

/**
 * PWM JNI Functions.
 *
 * @see "hal/PWM.h"
 */
public class PWMJNI extends DIOJNI {
  /**
   * Initializes a PWM port.
   *
   * @param channel the smartio channel
   * @return the created pwm handle
   */
  public static native int initializePWMPort(int channel);

  /**
   * Checks if a pwm channel is valid.
   *
   * @param channel the channel to check
   * @return true if the channel is valid, otherwise false
   */
  public static native boolean checkPWMChannel(int channel);

  /**
   * Frees a PWM port.
   *
   * @param pwmPortHandle the pwm handle
   */
  public static native void freePWMPort(int pwmPortHandle);

  /**
   * Indicates the pwm is used by a simulated device.
   *
   * @param handle the pwm handle
   * @param device simulated device handle
   * @see "HAL_SetPWMSimDevice"
   */
  public static native void setPWMSimDevice(int handle, int device);

  /**
   * Sets a PWM channel to the desired pulse width in microseconds.
   *
   * @param pwmPortHandle the PWM handle
   * @param microsecondPulseTime the PWM value to set
   */
  public static native void setPulseTimeMicroseconds(int pwmPortHandle, int microsecondPulseTime);

  /**
   * Gets the current microsecond pulse time from a PWM channel.
   *
   * @param pwmPortHandle the PWM handle
   * @return the current PWM microsecond pulse time
   */
  public static native int getPulseTimeMicroseconds(int pwmPortHandle);

  /**
   * Sets the PWM output period.
   *
   * @param pwmPortHandle the PWM handle.
   * @param period 0 for 5ms, 1 or 2 for 10ms, 3 for 20ms
   */
  public static native void setPWMOutputPeriod(int pwmPortHandle, int period);

  /** Utility class. */
  private PWMJNI() {}
}
