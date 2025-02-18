// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.hal;

/**
 * Ports HAL JNI functions.
 *
 * @see "hal/Ports.h"
 */
public class PortsJNI extends JNIWrapper {
  /**
   * Gets the number of can buses in the current system.
   *
   * @return the number of can buses
   * @see "HAL_GetNumCanBuses"
   */
  public static native int getNumCanBuses();

  /**
   * Gets the number of analog inputs in the current system.
   *
   * @return the number of analog inputs
   * @see "HAL_GetNumAnalogInputs"
   */
  public static native int getNumAnalogInputs();

  /**
   * Gets the number of counters in the current system.
   *
   * @return the number of counters
   * @see "HAL_GetNumCounters"
   */
  public static native int getNumCounters();

  /**
   * Gets the number of digital headers in the current system.
   *
   * @return the number of digital headers
   * @see "HAL_GetNumDigitalHeaders"
   */
  public static native int getNumDigitalHeaders();

  /**
   * Gets the number of PWM headers in the current system.
   *
   * @return the number of PWM headers
   * @see "HAL_GetNumPWMHeaders"
   */
  public static native int getNumPWMHeaders();

  /**
   * Gets the number of digital channels in the current system.
   *
   * @return the number of digital channels
   * @see "HAL_GetNumDigitalChannels"
   */
  public static native int getNumDigitalChannels();

  /**
   * Gets the number of PWM channels in the current system.
   *
   * @return the number of PWM channels
   * @see "HAL_GetNumPWMChannels"
   */
  public static native int getNumPWMChannels();

  /**
   * Gets the number of digital IO PWM outputs in the current system.
   *
   * @return the number of digital IO PWM outputs
   * @see "HAL_GetNumDigitalPWMOutputs"
   */
  public static native int getNumDigitalPWMOutputs();

  /**
   * Gets the number of quadrature encoders in the current system.
   *
   * @return the number of quadrature encoders
   * @see "HAL_GetNumEncoders"
   */
  public static native int getNumEncoders();

  /**
   * Gets the number of interrupts in the current system.
   *
   * @return the number of interrupts
   * @see "HAL_GetNumInterrupts"
   */
  public static native int getNumInterrupts();

  /**
   * Gets the number of PCM modules in the current system.
   *
   * @return the number of PCM modules
   * @see "HAL_GetNumCTREPCMModules"
   */
  public static native int getNumCTREPCMModules();

  /**
   * Gets the number of solenoid channels in the current system.
   *
   * @return the number of solenoid channels
   * @see "HAL_GetNumCTRESolenoidChannels"
   */
  public static native int getNumCTRESolenoidChannels();

  /**
   * Gets the number of PDP modules in the current system.
   *
   * @return the number of PDP modules
   * @see "HAL_GetNumCTREPDPModules"
   */
  public static native int getNumCTREPDPModules();

  /**
   * Gets the number of PDP channels in the current system.
   *
   * @return the number of PDP channels
   * @see "HAL_GetNumCTREPDPChannels"
   */
  public static native int getNumCTREPDPChannels();

  /**
   * Gets the number of PDH modules in the current system.
   *
   * @return the number of PDH modules
   * @see "HAL_GetNumREVPDHModules"
   */
  public static native int getNumREVPDHModules();

  /**
   * Gets the number of PDH channels in the current system.
   *
   * @return the number of PDH channels
   * @see "HAL_GetNumREVPDHChannels"
   */
  public static native int getNumREVPDHChannels();

  /**
   * Gets the number of PH modules in the current system.
   *
   * @return the number of PH modules
   * @see "HAL_GetNumREVPHModules"
   */
  public static native int getNumREVPHModules();

  /**
   * Gets the number of PH channels in the current system.
   *
   * @return the number of PH channels
   * @see "HAL_GetNumREVPHChannels"
   */
  public static native int getNumREVPHChannels();

  /** Utility class. */
  private PortsJNI() {}
}
