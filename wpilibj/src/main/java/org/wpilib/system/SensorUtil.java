// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.system;

import org.wpilib.hardware.hal.AnalogJNI;
import org.wpilib.hardware.hal.ConstantsJNI;
import org.wpilib.hardware.hal.DIOJNI;
import org.wpilib.hardware.hal.PWMJNI;
import org.wpilib.hardware.hal.PortsJNI;

/**
 * Stores most recent status information as well as containing utility functions for checking
 * channels and error processing.
 */
public final class SensorUtil {
  /** Ticks per microsecond. */
  public static final int kSystemClockTicksPerMicrosecond =
      ConstantsJNI.getSystemClockTicksPerMicrosecond();

  /** Number of digital channels per Systemcore. */
  public static final int kDigitalChannels = PortsJNI.getNumDigitalChannels();

  /** Number of analog input channels per Systemcore. */
  public static final int kAnalogInputChannels = PortsJNI.getNumAnalogInputs();

  /** Number of solenoid channels per module. */
  public static final int kCTRESolenoidChannels = PortsJNI.getNumCTRESolenoidChannels();

  /** Number of PWM channels per Systemcore. */
  public static final int kPwmChannels = PortsJNI.getNumPWMChannels();

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
   * Check that the digital channel number is valid. Verify that the channel number is one of the
   * legal channel numbers. Channel numbers are 0-based.
   *
   * @param channel The channel number to check.
   */
  public static void checkDigitalChannel(final int channel) {
    if (!DIOJNI.checkDIOChannel(channel)) {
      String buf =
          "Requested DIO channel is out of range. Minimum: 0, Maximum: "
              + kDigitalChannels
              + ", Requested: "
              + channel;
      throw new IllegalArgumentException(buf);
    }
  }

  /**
   * Check that the PWM channel number is valid. Verify that the channel number is one of the legal
   * channel numbers. Channel numbers are 0-based.
   *
   * @param channel The channel number to check.
   */
  public static void checkPWMChannel(final int channel) {
    if (!PWMJNI.checkPWMChannel(channel)) {
      String buf =
          "Requested PWM channel is out of range. Minimum: 0, Maximum: "
              + kPwmChannels
              + ", Requested: "
              + channel;
      throw new IllegalArgumentException(buf);
    }
  }

  /**
   * Check that the analog input number is value. Verify that the analog input number is one of the
   * legal channel numbers. Channel numbers are 0-based.
   *
   * @param channel The channel number to check.
   */
  public static void checkAnalogInputChannel(final int channel) {
    if (!AnalogJNI.checkAnalogInputChannel(channel)) {
      String buf =
          "Requested analog input channel is out of range. Minimum: 0, Maximum: "
              + kAnalogInputChannels
              + ", Requested: "
              + channel;
      throw new IllegalArgumentException(buf);
    }
  }

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
