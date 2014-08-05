/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.hal.DIOJNI;
import edu.wpi.first.wpilibj.hal.AnalogJNI;

/**
 * Base class for all sensors.
 * Stores most recent status information as well as containing utility functions for checking
 * channels and error processing.
 */
public abstract class SensorBase { // TODO: Refactor

	// TODO: Move this to the HAL

    /**
     * Ticks per microsecond
     */
    public static final int kSystemClockTicksPerMicrosecond = 40;
    /**
     * Number of digital channels per digital sidecar
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
     * Number of PWM channels per sidecar
     */
    public static final int kPwmChannels = 20;
    /**
     * Number of relay channels per sidecar
     */
    public static final int kRelayChannels = 4;
    /**
     * Number of power distribution channels
     */
    public static final int kPDPChannels = 16;

    private static int m_defaultSolenoidModule = 0;

    /**
     * Creates an instance of the sensor base and gets an FPGA handle
     */
    public SensorBase() {
    }

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
//        if(HALLibrary.checkSolenoidModule((byte) (moduleNumber - 1)) != 0) {
//            System.err.println("Solenoid module " + moduleNumber + " is not present.");
//        }
    }

    /**
     * Check that the digital channel number is valid.
     * Verify that the channel number is one of the legal channel numbers. Channel numbers are
     * 1-based.
     *
     * @param channel The channel number to check.
     */
    protected static void checkDigitalChannel(final int channel) {
        if (channel < 0 || channel >= kDigitalChannels) {
            System.err.println("Requested digital channel number is out of range.");
        }
    }

    /**
     * Check that the digital channel number is valid.
     * Verify that the channel number is one of the legal channel numbers. Channel numbers are
     * 1-based.
     *
     * @param channel The channel number to check.
     */
    protected static void checkRelayChannel(final int channel) {
        if (channel < 0 || channel >= kRelayChannels) {
            System.err.println("Requested relay channel number is out of range.");
            throw new IndexOutOfBoundsException("Requested relay channel number is out of range.");
        }
    }

    /**
     * Check that the digital channel number is valid.
     * Verify that the channel number is one of the legal channel numbers. Channel numbers are
     * 1-based.
     *
     * @param channel The channel number to check.
     */
    protected static void checkPWMChannel(final int channel) {
        if (channel < 0 || channel >= kPwmChannels) {
            System.err.println("Requested PWM channel number is out of range.");
            throw new IndexOutOfBoundsException("Requested PWM channel number is out of range.");
        }
    }

    /**
     * Check that the analog input number is value.
     * Verify that the analog input number is one of the legal channel numbers. Channel numbers
     * are 0-based.
     *
     * @param channel The channel number to check.
     */
    protected static void checkAnalogInputChannel(final int channel) {
        if (channel <= 0 || channel > kAnalogInputChannels) {
            System.err.println("Requested analog channel number is out of range.");
        }
    }

    /**
     * Check that the analog input number is value.
     * Verify that the analog input number is one of the legal channel numbers. Channel numbers
     * are 0-based.
     *
     * @param channel The channel number to check.
     */
    protected static void checkAnalogOutputChannel(final int channel) {
        if (channel <= 0 || channel > kAnalogOutputChannels) {
            System.err.println("Requested analog channel number is out of range.");
        }
    }

    /**
     * Verify that the solenoid channel number is within limits.  Channel numbers
     * are 1-based.
     *
     * @param channel The channel number to check.
     */
    protected static void checkSolenoidChannel(final int channel) {
        if (channel < 0 || channel >= kSolenoidChannels) {
            System.err.println("Requested solenoid channel number is out of range.");
        }
    }

    /**
     * Verify that the power distribution channel number is within limits.
     * Channel numbers are 1-based.
     *
     * @param channel The channel number to check.
     */
    protected static void checkPDPChannel(final int channel) {
        if (channel < 0 || channel >= kPDPChannels) {
            System.err.println("Requested solenoid channel number is out of range.");
        }
    }

    /**
     * Get the number of the default solenoid module.
     *
     * @return The number of the default analog module.
     */
    public static int getDefaultSolenoidModule() {
        return SensorBase.m_defaultSolenoidModule;
    }

    /**
     * Free the resources used by this object
     */
    public void free() {}
}
