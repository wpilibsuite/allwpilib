// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Digital Input/Output (IO) JNI Functions.
 *
 * @see "hal/DIO.h"
 */
public class DIOJNI extends JNIWrapper {
  /**
   * Creates a new instance of a digital port.
   *
   * @param halPortHandle the port handle to create from
   * @param input true for input, false for output
   * @return the created digital handle
   * @see "HAL_InitializeDIOPort"
   */
  public static native int initializeDIOPort(int halPortHandle, boolean input);

  /**
   * Checks if a DIO channel is valid.
   *
   * @param channel the channel number to check
   * @return true if the channel is valid, otherwise false
   * @see "HAL_CheckDIOChannel"
   */
  public static native boolean checkDIOChannel(int channel);

  /**
   * Frees a DIO port.
   *
   * @param dioPortHandle the DIO channel handle
   * @see "HAL_FreeDIOPort"
   */
  public static native void freeDIOPort(int dioPortHandle);

  /**
   * Indicates the DIO channel is used by a simulated device.
   *
   * @param handle the DIO channel handle
   * @param device simulated device handle
   * @see "HAL_SetDIOSimDevice"
   */
  public static native void setDIOSimDevice(int handle, int device);

  /**
   * Writes a digital value to a DIO channel.
   *
   * @param dioPortHandle the digital port handle
   * @param value the state to set the digital channel (if it is configured as an output)
   * @see "HAL_SetDIO"
   */
  public static native void setDIO(int dioPortHandle, boolean value);

  /**
   * Sets the direction of a DIO channel.
   *
   * @param dioPortHandle the digital port handle
   * @param input true to set input, false for output
   * @see "HAL_SetDIODirection"
   */
  public static native void setDIODirection(int dioPortHandle, boolean input);

  /**
   * Reads a digital value from a DIO channel.
   *
   * @param dioPortHandle the digital port handle
   * @return the state of the specified channel
   * @see "HAL_GetDIO"
   */
  public static native boolean getDIO(int dioPortHandle);

  /**
   * Reads the direction of a DIO channel.
   *
   * @param dioPortHandle the digital port handle
   * @return true for input, false for output
   * @see "HAL_GetDIODirection"
   */
  public static native boolean getDIODirection(int dioPortHandle);

  /**
   * Generates a single digital pulse.
   *
   * <p>Write a pulse to the specified digital output channel. There can only be a single pulse
   * going at any time.
   *
   * @param dioPortHandle the digital port handle
   * @param pulseLengthSeconds the active length of the pulse (in seconds)
   * @see "HAL_Pulse"
   */
  public static native void pulse(int dioPortHandle, double pulseLengthSeconds);

  /**
   * Generates a single digital pulse on multiple channels.
   *
   * <p>Write a pulse to the channels enabled by the mask. There can only be a single pulse going at
   * any time.
   *
   * @param channelMask the channel mask
   * @param pulseLengthSeconds the active length of the pulse (in seconds)
   * @see "HAL_PulseMultiple"
   */
  public static native void pulseMultiple(long channelMask, double pulseLengthSeconds);

  /**
   * Checks a DIO line to see if it is currently generating a pulse.
   *
   * @param dioPortHandle the digital port handle
   * @return true if a pulse is in progress, otherwise false
   * @see "HAL_IsPulsing"
   */
  public static native boolean isPulsing(int dioPortHandle);

  /**
   * Checks if any DIO line is currently generating a pulse.
   *
   * @return true if a pulse on some line is in progress
   * @see "HAL_IsAnyPulsing"
   */
  public static native boolean isAnyPulsing();

  /**
   * Gets the loop timing of the PWM system.
   *
   * @return the loop time in clock ticks
   */
  public static native short getLoopTiming();

  /**
   * Allocates a DO PWM Generator.
   *
   * @return the allocated digital PWM handle
   */
  public static native int allocateDigitalPWM();

  /**
   * Frees the resource associated with a DO PWM generator.
   *
   * @param pwmGenerator the digital PWM handle
   * @see "HAL_FreeDigitalPWM"
   */
  public static native void freeDigitalPWM(int pwmGenerator);

  /**
   * Changes the frequency of the DO PWM generator.
   *
   * <p>The valid range is from 0.6 Hz to 19 kHz.
   *
   * <p>The frequency resolution is logarithmic.
   *
   * @param rate the frequency to output all digital output PWM signals
   * @see "HAL_SetDigitalPWMRate"
   */
  public static native void setDigitalPWMRate(double rate);

  /**
   * Configures the duty-cycle of the PWM generator.
   *
   * @param pwmGenerator the digital PWM handle
   * @param dutyCycle the percent duty cycle to output [0..1]
   * @see "HAL_SetDigitalPWMDutyCycle"
   */
  public static native void setDigitalPWMDutyCycle(int pwmGenerator, double dutyCycle);

  /**
   * Configures the digital PWM to be a PPS signal with specified duty cycle.
   *
   * @param pwmGenerator the digital PWM handle
   * @param dutyCycle the percent duty cycle to output [0..1]
   * @see "HAL_SetDigitalPWMPPS"
   */
  public static native void setDigitalPWMPPS(int pwmGenerator, double dutyCycle);

  /**
   * Configures which DO channel the PWM signal is output on.
   *
   * @param pwmGenerator the digital PWM handle
   * @param channel the channel to output on
   * @see "HAL_SetDigitalPWMOutputChannel"
   */
  public static native void setDigitalPWMOutputChannel(int pwmGenerator, int channel);

  /** Utility class. */
  DIOJNI() {}
}
