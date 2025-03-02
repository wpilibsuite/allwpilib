// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.PortsJNI;

/**
 * Stores most recent status information as well as containing utility functions for checking
 * channels and error processing.
 */
public final class SensorUtil {
  /** Number of SmartIo Ports. */
  public static final int kSmartIoPorts = PortsJNI.getNumSmartIo();

  /** Number of solenoid channels per module. */
  public static final int kCTRESolenoidChannels = PortsJNI.getNumCTRESolenoidChannels();

  /** Number of power distribution channels per PDP. */
  public static final int kCTREPDPChannels = PortsJNI.getNumCTREPDPChannels();

  /** Number of power distribution modules per PDP. */
  public static final int kCTREPDPModules = PortsJNI.getNumCTREPDPModules();

  /** Number of PCM Modules. */
  public static final int kCTREPCMModules = PortsJNI.getNumCTREPCMModules();

  /** Number of power distribution channels per PH. */
  public static final int kREVPHChannels = PortsJNI.getNumREVPHChannels();

  /** Number of PH modules. */
  public static final int kREVPHModules = PortsJNI.getNumREVPHModules();

  /**
   * Get the number of the default solenoid module.
   *
   * @return The number of the default solenoid module.
   */
  public static int getDefaultCTREPCMModule() {
    return 0;
  }

  /**
   * Get the number of the default solenoid module.
   *
   * @return The number of the default solenoid module.
   */
  public static int getDefaultREVPHModule() {
    return 1;
  }

  private SensorUtil() {}
}
