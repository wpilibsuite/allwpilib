/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Base class for all sensors. Stores most recent status information as well as
 * containing utility functions for checking channels and error processing.
 */
public abstract class SensorBase { // TODO: Refactor

  // TODO: Move this to the HAL

  /**
   * Ticks per microsecond
   */
  public static final int kSystemClockTicksPerMicrosecond = 40;
  /**
   * Number of digital channels per roboRIO
   */
  public static final int kDigitalChannels = 26;
  /**
   * Number of analog input channels
   */
  public static final int kAnalogInputChannels = 8;
  /**
   * Number of analog output channels
   */
  public static final int kAnalogOutputChannels = 2;
  /**
   * Number of solenoid channels per module
   */
  public static final int kSolenoidChannels = 8;
  /**
   * Number of solenoid modules
   */
  public static final int kSolenoidModules = 2;
  /**
   * Number of PWM channels per roboRIO
   */
  public static final int kPwmChannels = 20;
  /**
   * Number of relay channels per roboRIO
   */
  public static final int kRelayChannels = 4;
  /**
   * Number of power distribution channels
   */
  public static final int kPDPChannels = 16;
  /**
   * Number of power distribution modules
   */
  public static final int kPDPModules = 63;

  private static int m_defaultSolenoidModule = 0;

  /**
   * Creates an instance of the sensor base and gets an FPGA handle
   */
  public SensorBase() {}

  /**
   * Set the default location for the Solenoid module.
   *
   * @param moduleNumber The number of the solenoid module to use.
   */
  public static void setDefaultSolenoidModule(final int moduleNumber) {
    checkSolenoidModule(moduleNumber);
    SensorBase.m_defaultSolenoidModule = moduleNumber;
  }

  /**
   * Verify that the solenoid module is correct.
   *
   * @param moduleNumber The solenoid module module number to check.
   */
  protected static void checkSolenoidModule(final int moduleNumber) {
  }

  /**
   * Check that the digital channel number is valid. Verify that the channel
   * number is one of the legal channel numbers. Channel numbers are 1-based.
   *
   * @param channel The channel number to check.
   */
  protected static void checkDigitalChannel(final int channel) {
    if (channel < 0 || channel >= kDigitalChannels) {
      throw new IndexOutOfBoundsException("Requested digital channel number is out of range.");
    }
  }

  /**
   * Check that the digital channel number is valid. Verify that the channel
   * number is one of the legal channel numbers. Channel numbers are 1-based.
   *
   * @param channel The channel number to check.
   */
  protected static void checkRelayChannel(final int channel) {
    if (channel < 0 || channel >= kRelayChannels) {
      throw new IndexOutOfBoundsException("Requested relay channel number is out of range.");
    }
  }

  /**
   * Check that the digital channel number is valid. Verify that the channel
   * number is one of the legal channel numbers. Channel numbers are 1-based.
   *
   * @param channel The channel number to check.
   */
  protected static void checkPWMChannel(final int channel) {
    if (channel < 0 || channel >= kPwmChannels) {
      throw new IndexOutOfBoundsException("Requested PWM channel number is out of range.");
    }
  }

  /**
   * Check that the analog input number is value. Verify that the analog input
   * number is one of the legal channel numbers. Channel numbers are 0-based.
   *
   * @param channel The channel number to check.
   */
  protected static void checkAnalogInputChannel(final int channel) {
    if (channel < 0 || channel >= kAnalogInputChannels) {
      throw new IndexOutOfBoundsException("Requested analog input channel number is out of range.");
    }
  }

  /**
   * Check that the analog input number is value. Verify that the analog input
   * number is one of the legal channel numbers. Channel numbers are 0-based.
   *
   * @param channel The channel number to check.
   */
  protected static void checkAnalogOutputChannel(final int channel) {
    if (channel < 0 || channel >= kAnalogOutputChannels) {
      throw new IndexOutOfBoundsException("Requested analog output channel number is out of range.");
    }
  }

  /**
   * Verify that the solenoid channel number is within limits. Channel numbers
   * are 1-based.
   *
   * @param channel The channel number to check.
   */
  protected static void checkSolenoidChannel(final int channel) {
    if (channel < 0 || channel >= kSolenoidChannels) {
      throw new IndexOutOfBoundsException("Requested solenoid channel number is out of range.");
    }
  }

  /**
   * Verify that the power distribution channel number is within limits. Channel
   * numbers are 1-based.
   *
   * @param channel The channel number to check.
   */
  protected static void checkPDPChannel(final int channel) {
    if (channel < 0 || channel >= kPDPChannels) {
      throw new IndexOutOfBoundsException("Requested PDP channel number is out of range.");
    }
  }

  /**
   * Verify that the PDP module number is within limits. module numbers are
   * 0-based.
   *$
   * @param module The module number to check.
   */
  protected static void checkPDPModule(final int module) {
    if (module < 0 || module > kPDPModules) {
      throw new IndexOutOfBoundsException("Requested PDP module number is out of range.");
    }
  }

  /**
   * Get the number of the default solenoid module.
   *
   * @return The number of the default solenoid module.
   */
  public static int getDefaultSolenoidModule() {
    return SensorBase.m_defaultSolenoidModule;
  }

  /**
   * Free the resources used by this object
   */
  public void free() {}
}
