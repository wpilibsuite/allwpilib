// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import org.wpilib.hardware.hal.PortsJNI;

/**
 * Stores most recent status information as well as containing utility functions for checking
 * channels and error processing.
 */
public final class SensorUtil {
  /** Number of SmartIo Ports. */
  public static final int kSmartIoPorts = PortsJNI.getNumSmartIo();

  /** Number of solenoid channels per module. */
  public static final int NUM_CTRE_SOLENOID_CHANNELS = PortsJNI.getNumCTRESolenoidChannels();

  /** Number of power distribution channels per PDP. */
  public static final int NUM_CTRE_PDP_CHANNELS = PortsJNI.getNumCTREPDPChannels();

  /** Number of power distribution modules per PDP. */
  public static final int NUM_CTRE_PDP_MODULES = PortsJNI.getNumCTREPDPModules();

  /** Number of PCM Modules. */
  public static final int NUM_CTRE_PCM_MODULES = PortsJNI.getNumCTREPCMModules();

  /** Number of power distribution channels per PH. */
  public static final int NUM_REV_PH_CHANNELS = PortsJNI.getNumREVPHChannels();

  /** Number of PH modules. */
  public static final int NUM_REV_PH_MODULES = PortsJNI.getNumREVPHModules();

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
