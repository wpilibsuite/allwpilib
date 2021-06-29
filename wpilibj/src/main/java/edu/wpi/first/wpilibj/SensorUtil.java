// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import edu.wpi.first.hal.AnalogJNI;
import edu.wpi.first.hal.ConstantsJNI;
import edu.wpi.first.hal.DIOJNI;
import edu.wpi.first.hal.PWMJNI;
import edu.wpi.first.hal.PortsJNI;
import edu.wpi.first.hal.RelayJNI;

/**
 * Stores most recent status information as well as containing utility functions for checking
 * channels and error processing.
 */
public final class SensorUtil {
  /** Ticks per microsecond. */
  public static final int kSystemClockTicksPerMicrosecond =
      ConstantsJNI.getSystemClockTicksPerMicrosecond();

  /** Number of digital channels per roboRIO. */
  public static final int kDigitalChannels = PortsJNI.getNumDigitalChannels();

  /** Number of analog input channels per roboRIO. */
  public static final int kAnalogInputChannels = PortsJNI.getNumAnalogInputs();

  /** Number of analog output channels per roboRIO. */
  public static final int kAnalogOutputChannels = PortsJNI.getNumAnalogOutputs();

  /** Number of solenoid channels per module. */
  public static final int kSolenoidChannels = PortsJNI.getNumSolenoidChannels();

  /** Number of PWM channels per roboRIO. */
  public static final int kPwmChannels = PortsJNI.getNumPWMChannels();

  /** Number of relay channels per roboRIO. */
  public static final int kRelayChannels = PortsJNI.getNumRelayHeaders();

  /** Number of power distribution channels per PDP. */
  public static final int kPDPChannels = PortsJNI.getNumPDPChannels();

  /** Number of power distribution modules per PDP. */
  public static final int kPDPModules = PortsJNI.getNumPDPModules();

  /** Number of PCM Modules. */
  public static final int kPCMModules = PortsJNI.getNumPCMModules();

  /**
   * Check that the digital channel number is valid. Verify that the channel number is one of the
   * legal channel numbers. Channel numbers are 0-based.
   *
   * @param channel The channel number to check.
   */
  public static void checkDigitalChannel(final int channel) {
    if (!DIOJNI.checkDIOChannel(channel)) {
      StringBuilder buf = new StringBuilder();
      buf.append("Requested DIO channel is out of range. Minimum: 0, Maximum: ")
          .append(kDigitalChannels)
          .append(", Requested: ")
          .append(channel);
      throw new IllegalArgumentException(buf.toString());
    }
  }

  /**
   * Check that the digital channel number is valid. Verify that the channel number is one of the
   * legal channel numbers. Channel numbers are 0-based.
   *
   * @param channel The channel number to check.
   */
  public static void checkRelayChannel(final int channel) {
    if (!RelayJNI.checkRelayChannel(channel)) {
      StringBuilder buf = new StringBuilder();
      buf.append("Requested relay channel is out of range. Minimum: 0, Maximum: ")
          .append(kRelayChannels)
          .append(", Requested: ")
          .append(channel);
      throw new IllegalArgumentException(buf.toString());
    }
  }

  /**
   * Check that the digital channel number is valid. Verify that the channel number is one of the
   * legal channel numbers. Channel numbers are 0-based.
   *
   * @param channel The channel number to check.
   */
  public static void checkPWMChannel(final int channel) {
    if (!PWMJNI.checkPWMChannel(channel)) {
      StringBuilder buf = new StringBuilder();
      buf.append("Requested PWM channel is out of range. Minimum: 0, Maximum: ")
          .append(kPwmChannels)
          .append(", Requested: ")
          .append(channel);
      throw new IllegalArgumentException(buf.toString());
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
      StringBuilder buf = new StringBuilder();
      buf.append("Requested analog input channel is out of range. Minimum: 0, Maximum: ")
          .append(kAnalogInputChannels)
          .append(", Requested: ")
          .append(channel);
      throw new IllegalArgumentException(buf.toString());
    }
  }

  /**
   * Check that the analog input number is value. Verify that the analog input number is one of the
   * legal channel numbers. Channel numbers are 0-based.
   *
   * @param channel The channel number to check.
   */
  public static void checkAnalogOutputChannel(final int channel) {
    if (!AnalogJNI.checkAnalogOutputChannel(channel)) {
      StringBuilder buf = new StringBuilder();
      buf.append("Requested analog output channel is out of range. Minimum: 0, Maximum: ")
          .append(kAnalogOutputChannels)
          .append(", Requested: ")
          .append(channel);
      throw new IllegalArgumentException(buf.toString());
    }
  }

  // /**
  //  * Verify that the power distribution channel number is within limits. Channel numbers are
  //  * 0-based.
  //  *
  //  * @param channel The channel number to check.
  //  */
  // public static void checkPDPChannel(final int channel) {
  //   if (!PowerDistributionJNI.checkPDPChannel(channel)) {
  //     StringBuilder buf = new StringBuilder();
  //     buf.append("Requested PDP channel is out of range. Minimum: 0, Maximum: ")
  //         .append(kPDPChannels)
  //         .append(", Requested: ")
  //         .append(channel);
  //     throw new IllegalArgumentException(buf.toString());
  //   }
  // }

  // /**
  //  * Verify that the PDP module number is within limits. module numbers are 0-based.
  //  *
  //  * @param module The module number to check.
  //  */
  // public static void checkPDPModule(final int module) {
  //   if (!PowerDistributionJNI.checkPDPModule(module)) {
  //     StringBuilder buf = new StringBuilder();
  //     buf.append("Requested PDP module is out of range. Minimum: 0, Maximum: ")
  //         .append(kPDPModules)
  //         .append(", Requested: ")
  //         .append(module);
  //     throw new IllegalArgumentException(buf.toString());
  //   }
  // }

  /**
   * Get the number of the default solenoid module.
   *
   * @return The number of the default solenoid module.
   */
  @SuppressWarnings("AbbreviationAsWordInName")
  public static int getDefaultCTREPCMModule() {
    return 0;
  }

  private SensorUtil() {}
}
