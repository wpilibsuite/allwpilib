/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

/**
 * Base class for all sensors.
 * Stores most recent status information as well as containing utility functions for checking
 * channels and error processing.
 *
 * XXX: Wait, there's no exception thrown if we try to allocate a non-existent module?  It that behavior correct?
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
    public static final int kDigitalChannels = 14;
    /**
     * Number of analog channels per module
     */
    public static final int kAnalogChannels = 8;
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
    public static final int kPwmChannels = 10;
    /**
     * Number of relay channels per sidecar
     */
    public static final int kRelayChannels = 8;

    private static int m_defaultSolenoidModule = 1;

    /**
     * Creates an instance of the sensor base and gets an FPGA handle
     */
    public SensorBase() {
    }

    /**
     * Set the default location for the Solenoid (9472) module.
     *
     * @param moduleNumber The number of the solenoid module to use.
     */
    public static void setDefaultSolenoidModule(final int moduleNumber) {
        checkSolenoidModule(moduleNumber);
        SensorBase.m_defaultSolenoidModule = moduleNumber;
    }

    /**
     * Verify that the solenoid module is correct.
     * Module numbers are 1 or 2 (they are no longer real cRIO slots).
     *
     * @param moduleNumber The solenoid module module number to check.
     */
    protected static void checkSolenoidModule(final int moduleNumber) {
    	// TODO: fix
        if(moduleNumber == 1 || moduleNumber == 2) {
            System.err.println("Solenoid module " + moduleNumber + " is not present.");
        }
    }

    /**
     * Check that the digital channel number is valid.
     * Verify that the channel number is one of the legal channel numbers. Channel numbers are
     * 1-based.
     *
     * @param channel The channel number to check.
     */
    protected static void checkDigitalChannel(final int channel) {
        if (channel <= 0 || channel > kDigitalChannels) {
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
        if (channel <= 0 || channel > kRelayChannels) {
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
        if (channel <= 0 || channel > kPwmChannels) {
            System.err.println("Requested PWM channel number is out of range.");
            throw new IndexOutOfBoundsException("Requested PWM channel number is out of range.");
        }
    }

    /**
     * Check that the analog channel number is value.
     * Verify that the analog channel number is one of the legal channel numbers. Channel numbers
     * are 1-based.
     *
     * @param channel The channel number to check.
     */
    protected static void checkAnalogChannel(final int channel) {
        if (channel <= 0 || channel > kAnalogChannels) {
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
        if (channel <= 0 || channel > kSolenoidChannels) {
            System.err.println("Requested solenoid channel number is out of range.");
        }
    }

    /**
     * Get the number of the default analog module.
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
