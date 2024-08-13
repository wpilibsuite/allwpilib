// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * PWM JNI Functions.
 *
 * @see "hal/PWM.h"
 */
public class PWMJNI extends DIOJNI {
  /**
   * Initializes a PWM port.
   *
   * @param halPortHandle the port to initialize
   * @return the created pwm handle
   */
  public static native int initializePWMPort(int halPortHandle);

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
   * Sets the configuration settings for the PWM channel.
   *
   * <p>All values are in microseconds.
   *
   * @param pwmPortHandle the PWM handle
   * @param maxPwm the maximum PWM value
   * @param deadbandMaxPwm the high range of the center deadband
   * @param centerPwm the center PWM value
   * @param deadbandMinPwm the low range of the center deadband
   * @param minPwm the minimum PWM value
   */
  public static native void setPWMConfigMicroseconds(
      int pwmPortHandle,
      int maxPwm,
      int deadbandMaxPwm,
      int centerPwm,
      int deadbandMinPwm,
      int minPwm);

  /**
   * Gets the pwm configuration settings for the PWM channel.
   *
   * <p>Values are in microseconds.
   *
   * @param pwmPortHandle the PWM handle
   * @return the pwm configuration settings
   */
  public static native PWMConfigDataResult getPWMConfigMicroseconds(int pwmPortHandle);

  /**
   * Sets if the FPGA should output the center value if the input value is within the deadband.
   *
   * @param pwmPortHandle the PWM handle
   * @param eliminateDeadband true to eliminate deadband, otherwise false
   */
  public static native void setPWMEliminateDeadband(int pwmPortHandle, boolean eliminateDeadband);

  /**
   * Gets the current eliminate deadband value.
   *
   * @param pwmPortHandle the PWM handle
   * @return true if set, otherwise false
   */
  public static native boolean getPWMEliminateDeadband(int pwmPortHandle);

  /**
   * Sets a PWM channel to the desired pulse width in microseconds.
   *
   * @param pwmPortHandle the PWM handle
   * @param microsecondPulseTime the PWM value to set
   */
  public static native void setPulseTimeMicroseconds(int pwmPortHandle, int microsecondPulseTime);

  /**
   * Sets a PWM channel to the desired scaled value.
   *
   * <p>The values range from -1 to 1 and the period is controlled by the PWM Period and MinHigh
   * registers.
   *
   * @param pwmPortHandle the PWM handle
   * @param speed the scaled PWM value to set
   */
  public static native void setPWMSpeed(int pwmPortHandle, double speed);

  /**
   * Sets a PWM channel to the desired position value.
   *
   * <p>The values range from 0 to 1 and the period is controlled by the PWM Period and MinHigh
   * registers.
   *
   * @param pwmPortHandle the PWM handle
   * @param position the positional PWM value to set
   */
  public static native void setPWMPosition(int pwmPortHandle, double position);

  /**
   * Gets the current microsecond pulse time from a PWM channel.
   *
   * @param pwmPortHandle the PWM handle
   * @return the current PWM microsecond pulse time
   */
  public static native int getPulseTimeMicroseconds(int pwmPortHandle);

  /**
   * Gets a scaled value from a PWM channel.
   *
   * <p>The values range from -1 to 1.
   *
   * @param pwmPortHandle the PWM handle
   * @return the current speed PWM value
   */
  public static native double getPWMSpeed(int pwmPortHandle);

  /**
   * Gets a position value from a PWM channel.
   *
   * <p>The values range from 0 to 1.
   *
   * @param pwmPortHandle the PWM handle
   * @return the current positional PWM value
   */
  public static native double getPWMPosition(int pwmPortHandle);

  /**
   * Sets a PWM channel to be disabled.
   *
   * <p>The channel is disabled until the next time it is set. Note this is different from just
   * setting a 0 speed, as this will actively stop all signaling on the channel.
   *
   * @param pwmPortHandle the PWM handle.
   */
  public static native void setPWMDisabled(int pwmPortHandle);

  /**
   * Forces a PWM signal to go to 0 temporarily.
   *
   * @param pwmPortHandle the PWM handle.
   */
  public static native void latchPWMZero(int pwmPortHandle);

  /**
   * Sets the PWM output to be a continuous high signal while enabled.
   *
   * @param pwmPortHandle the PWM handle.
   */
  public static native void setAlwaysHighMode(int pwmPortHandle);

  /**
   * Sets how how often the PWM signal is squelched, thus scaling the period.
   *
   * @param pwmPortHandle the PWM handle.
   * @param squelchMask the 2-bit mask of outputs to squelch
   */
  public static native void setPWMPeriodScale(int pwmPortHandle, int squelchMask);

  /** Utility class. */
  private PWMJNI() {}
}
