/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import java.nio.IntBuffer;

import com.sun.jna.Pointer;
import com.sun.jna.ptr.IntByReference;

import edu.wpi.first.wpilibj.communication.FRC_NetworkCommunicationsLibrary.tModuleType;
import edu.wpi.first.wpilibj.hal.HALLibrary;
import edu.wpi.first.wpilibj.hal.HALUtil;

/**
 * Class representing a digital module
 *
 * @author dtjones
 */
public class DigitalModule extends Module {

    /**
     * Expected loop timing
     */
    public static final int kExpectedLoopTiming = 260;
    private int m_module;
    private Pointer[] m_digital_ports;
    private Pointer[] m_relay_ports;
    private Pointer[] m_pwm_ports;

    /**
     * Get an instance of an Digital Module. Singleton digital module creation
     * where a module is allocated on the first use and the same module is
     * returned on subsequent uses.
     *
     * @param moduleNumber
     *            The number of the digital module to access.
     * @return The digital module of the specified number.
     */
    public static synchronized DigitalModule getInstance(final int moduleNumber) {
        SensorBase.checkDigitalModule(moduleNumber);
        return (DigitalModule) getModule(tModuleType.kModuleType_Digital,
                                         moduleNumber);
    }

    /**
     * @deprecated
     * Convert a channel to its fpga reference
     *
     * @param channel
     *            the channel to convert
     * @return the converted channel
     */
    public static int remapDigitalChannel(final int channel) {
        System.err.println("This is going away for compatability reasons.  Don't use it.");
        return SensorBase.kDigitalChannels - channel;
    }

    /**
     * @deprecated
     * Convert a channel from it's fpga reference
     *
     * @param channel
     *            the channel to convert
     * @return the converted channel
     */
    public static int unmapDigitalChannel(final int channel) {
        System.err.println("This is going away for compatability reasons.  Don't use it.");
        return SensorBase.kDigitalChannels - channel;
    }

    /**
     * Create a new digital module
     *
     * @param moduleNumber
     *            The number of the digital module to use (1 or 2)
     */
    protected DigitalModule(final int moduleNumber) {
        super(tModuleType.kModuleType_Digital, moduleNumber);
        m_module = moduleNumber;

        m_digital_ports = new Pointer[SensorBase.kDigitalChannels];
        for (int i = 0; i < SensorBase.kDigitalChannels; i++) {
            Pointer port_pointer = HALLibrary.getPortWithModule(
                                       (byte) moduleNumber, (byte) (i + 1));
            IntBuffer status = IntBuffer.allocate(1);
            m_digital_ports[i] = HALLibrary.initializeDigitalPort(port_pointer,
                               status);
            HALUtil.checkStatus(status);
        }

        m_relay_ports = new Pointer[SensorBase.kRelayChannels];
        for (int i = 0; i < SensorBase.kRelayChannels; i++) {
            Pointer port_pointer = HALLibrary.getPortWithModule(
                                       (byte) moduleNumber, (byte) (i + 1));
            IntBuffer status = IntBuffer.allocate(1);
            m_relay_ports[i] = HALLibrary.initializeDigitalPort(port_pointer,
                               status);
            HALUtil.checkStatus(status);
        }
        m_pwm_ports = new Pointer[SensorBase.kPwmChannels];
        for (int i = 0; i < SensorBase.kPwmChannels; i++) {
            Pointer port_pointer = HALLibrary.getPortWithModule(
                                       (byte) moduleNumber, (byte) (i + 1));
            IntBuffer status = IntBuffer.allocate(1);
            m_pwm_ports[i] = HALLibrary.initializeDigitalPort(port_pointer,
                               status);
            HALUtil.checkStatus(status);
        }

    }

    /**
     * Set a PWM channel to the desired value. The values range from 0 to 255
     * and the period is controlled by the PWM Period and MinHigh registers.
     *
     * @param channel
     *            The PWM channel to set.
     * @param value
     *            The PWM value to set.
     */
    public void setPWM(final int channel, final int value) {
        IntBuffer status = IntBuffer.allocate(1);
        HALLibrary.setPWM(m_pwm_ports[channel - 1], (short) value, status);
        HALUtil.checkStatus(status);
    }

    /**
     * Get a value from a PWM channel. The values range from 0 to 255.
     *
     * @param channel
     *            The PWM channel to read from.
     * @return The raw PWM value.
     */
    public int getPWM(final int channel) {
        IntBuffer status = IntBuffer.allocate(1);
        int value = (int) HALLibrary.getPWM(m_pwm_ports[channel - 1], status);
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Set how how often the PWM signal is squelched, thus scaling the period.
     *
     * @param channel
     *            The PWM channel to configure.
     * @param squelchMask
     *            The 2-bit mask of outputs to squelch.
     */
    public void setPWMPeriodScale(final int channel, final int squelchMask) {
        IntBuffer status = IntBuffer.allocate(1);
        HALLibrary.setPWMPeriodScale(m_pwm_ports[channel - 1], squelchMask,
                                     status);
        HALUtil.checkStatus(status);
    }

    /**
     * Set the state of a relay. Set the state of a relay output to be forward.
     * Relays have two outputs and each is independently set to 0v or 12v.
     *
     * @param channel
     *            The Relay channel.
     * @param on
     *            Indicates whether to set the relay to the On state.
     */
    public void setRelayForward(final int channel, final boolean on) {
        IntBuffer status = IntBuffer.allocate(1);
        HALLibrary.setRelayForward(m_relay_ports[channel - 1], (byte) (on ? 1
                                   : 0), status);
        HALUtil.checkStatus(status);
    }

    /**
     * Set the state of a relay. Set the state of a relay output to be reverse.
     * Relays have two outputs and each is independently set to 0v or 12v.
     *
     * @param channel
     *            The Relay channel.
     * @param on
     *            Indicates whether to set the relay to the On state.
     */
    public void setRelayReverse(final int channel, final boolean on) {
        IntBuffer status = IntBuffer.allocate(1);
        HALLibrary.setRelayReverse(m_relay_ports[channel - 1], (byte) (on ? 1
                                   : 0), status);
        HALUtil.checkStatus(status);
    }

    /**
     * Get the current state of the forward relay channel
     *
     * @param channel
     *            the channel of the relay to get
     * @return The current state of the relay.
     */
    public boolean getRelayForward(int channel) {
        IntBuffer status = IntBuffer.allocate(1);
        boolean value = HALLibrary.getRelayForward(m_relay_ports[channel - 1],
                        status) != 0;
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Get the current state of all of the forward relay channels on this
     * module.
     *
     * @return The state of all forward relay channels as a byte.
     */
    public byte getRelayForward() {
        byte value = 0;
        IntBuffer status = IntBuffer.allocate(1);
        for (int i = 0; i < SensorBase.kRelayChannels; i++) {
            value |= HALLibrary.getRelayForward(m_relay_ports[i], status) << i;
        }
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Get the current state of the reverse relay channel
     *
     * @param channel
     *            the channel of the relay to get
     * @return The current statte of the relay
     */
    public boolean getRelayReverse(int channel) {
        IntBuffer status = IntBuffer.allocate(1);
        boolean value = HALLibrary.getRelayReverse(m_relay_ports[channel - 1],
                        status) != 0;
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Get the current state of all of the reverse relay channels on this
     * module.
     *
     * @return The state of all forward relay channels as a byte.
     */
    public byte getRelayReverse() {
        byte value = 0;
        IntBuffer status = IntBuffer.allocate(1);
        for (int i = 0; i < SensorBase.kRelayChannels; i++) {
            value |= HALLibrary.getRelayReverse(m_relay_ports[i], status) << i;
        }
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Allocate Digital I/O channels. Allocate channels so that they are not
     * accidently reused. Also the direction is set at the time of the
     * allocation.
     *
     * @param channel
     *            The channel to allocate.
     * @param input
     *            Indicates whether the I/O pin is an input (true) or an output
     *            (false).
     * @return True if the I/O pin was allocated, false otherwise.
     */
    public boolean allocateDIO(final int channel, final boolean input) {
        IntBuffer status = IntBuffer.allocate(1);
        boolean allocated = HALLibrary.allocateDIO(
                                m_digital_ports[channel - 1], (byte) (input ? 1 : 0), status) != 0;
        HALUtil.checkStatus(status);
        return allocated;
    }

    /**
     * Free the resource associated with a digital I/O channel.
     *
     * @param channel
     *            The channel whose resources should be freed.
     */
    public void freeDIO(final int channel) {
        IntBuffer status = IntBuffer.allocate(1);
        HALLibrary.freeDIO(m_digital_ports[channel - 1], status);
        HALUtil.checkStatus(status);
    }

    /**
     * Write a digital I/O bit to the FPGA. Set a single value on a digital I/O
     * channel.
     *
     * @param channel
     *            The channel to set.
     * @param value
     *            The value to set.
     */
    public void setDIO(final int channel, final boolean value) {
        IntBuffer status = IntBuffer.allocate(1);
        HALLibrary.setDIO(m_digital_ports[channel - 1], (byte) (value ? 1 : 0),
                          status);
        HALUtil.checkStatus(status);
    }

    /**
     * Read a digital I/O bit from the FPGA. Get a single value from a digital
     * I/O channel.
     *
     * @param channel
     *            The channel to read
     * @return The value of the selected channel
     */
    public boolean getDIO(final int channel) {
        IntBuffer status = IntBuffer.allocate(1);
        boolean value = HALLibrary.getDIO(m_digital_ports[channel - 1], status) != 0;
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Read the state of all the Digital I/O lines from the FPGA These are not
     * remapped to logical order. They are still in hardware order.
     *
     * @return The state of all the Digital IO lines in hardware order
     */
    public short getAllDIO() {
        byte value = 0;
        IntBuffer status = IntBuffer.allocate(1);
        for (int i = 0; i < SensorBase.kDigitalChannels; i++) {
            value |= HALLibrary.getDIO(m_digital_ports[i], status) << i;
        }
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Read the direction of a digital I/O line
     *
     * @param channel
     *            The channel of the DIO to get the direction of.
     * @return True if the digital channel is configured as an output, false if
     *         it is an input
     */
    public boolean getDIODirection(int channel) {
        IntBuffer status = IntBuffer.allocate(1);
        boolean value = HALLibrary.getDIODirection(
                            m_digital_ports[channel - 1], status) != 0;
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Read the direction of all the Digital I/O lines from the FPGA A 1 bit
     * means output and a 0 bit means input. These are not remapped to logical
     * order. They are still in hardware order.
     *
     * @return The direction of all the Digital IO lines in hardware order
     */
    public short getDIODirection() {
        byte value = 0;
        IntBuffer status = IntBuffer.allocate(1);
        for (int i = 0; i < SensorBase.kDigitalChannels; i++) {
            value |= HALLibrary.getDIODirection(m_digital_ports[i], status) << i;
        }
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Generate a single pulse. Write a pulse to the specified digital output
     * channel. There can only be a single pulse going at any time.
     *
     * @param channel
     *            The channel to pulse.
     * @param pulseLength
     *            The length of the pulse.
     */
    public void pulse(final int channel, final float pulseLength) {
        IntBuffer status = IntBuffer.allocate(1);
        HALLibrary.pulse(m_digital_ports[channel - 1], pulseLength, status);
        HALUtil.checkStatus(status);
    }

    /**
     * @deprecated Generate a single pulse. Write a pulse to the specified
     *             digital output channel. There can only be a single pulse
     *             going at any time.
     *
     * @param channel
     *            The channel to pulse.
     * @param pulseLength
     *            The length of the pulse.
     */
    public void pulse(final int channel, final int pulseLength) {
        IntBuffer status = IntBuffer.allocate(1);
        float convertedPulse = (float) (pulseLength / 1.0e9 * (HALLibrary
                                        .getLoopTiming(status) * 25));
        System.err
        .println("You should use the float version of pulse for portability.  This is deprecated");
        HALLibrary.pulse(m_digital_ports[channel - 1], convertedPulse, status);
        HALUtil.checkStatus(status);
    }

    /**
     * Check a DIO line to see if it is currently generating a pulse.
     *
     * @param channel
     *            The channel to check.
     * @return True if the channel is pulsing, false otherwise.
     */
    public boolean isPulsing(final int channel) {
        IntBuffer status = IntBuffer.allocate(1);
        boolean value = HALLibrary.isPulsing(m_digital_ports[channel - 1],
                                             status) != 0;
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Check if any DIO line is currently generating a pulse.
     *
     * @return True if any channel is pulsing, false otherwise.
     */
    public boolean isPulsing() {
        boolean value;
        IntBuffer status = IntBuffer.allocate(1);
        value = HALLibrary.isAnyPulsingWithModule((byte) m_module, status) != 0;
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Allocate a DO PWM Generator. Allocate PWM generators so that they are not
     * accidently reused.
     */
    public int allocateDO_PWM() {
        IntBuffer status = IntBuffer.allocate(1);
        int value = HALLibrary.allocatePWMWithModule((byte) m_module, status)
                    .getInt(0); // XXX: Hacky, needs heavy testing
        HALUtil.checkStatus(status);
        return value;
    }

    /**
     * Free the resource associated with a DO PWM generator.
     */
    public void freeDO_PWM(int pwmGenerator) {
        IntBuffer status = IntBuffer.allocate(1);
        Pointer pointer = new IntByReference(pwmGenerator).getPointer();
        HALLibrary.freePWMWithModule((byte) m_module, pointer, status);
        HALUtil.checkStatus(status);
    }

    /**
     * Change the frequency of the DO PWM generator.
     *
     * The valid range is from 0.6 Hz to 19 kHz. The frequency resolution is
     * logarithmic.
     *
     * @param rate
     *            The frequency to output all digital output PWM signals on this
     *            module.
     */
    public void setDO_PWMRate(double rate) {
        IntBuffer status = IntBuffer.allocate(1);
        HALLibrary.setPWMRateWithModuleIntHack((byte) m_module, Float.floatToIntBits((float) rate), status);
        HALUtil.checkStatus(status);
    }

    /**
     * Configure which DO channel the PWM siganl is output on
     *
     * @param pwmGenerator
     *            The generator index reserved by allocateDO_PWM()
     * @param channel
     *            The Digital Output channel to output on
     */
    public void setDO_PWMOutputChannel(int pwmGenerator, int channel) {
        IntBuffer status = IntBuffer.allocate(1);
        Pointer pointer = new IntByReference(pwmGenerator).getPointer();
        HALLibrary.setPWMOutputChannelWithModule((byte) m_module, pointer,
                channel, status);
        HALUtil.checkStatus(status);
    }

    /**
     * Configure the duty-cycle of the PWM generator
     *
     * @param pwmGenerator
     *            The generator index reserved by allocateDO_PWM()
     * @param dutyCycle
     *            The percent duty cycle to output [0..1].
     */
    public void setDO_PWMDutyCycle(int pwmGenerator, double dutyCycle) {
        IntBuffer status = IntBuffer.allocate(1);
        Pointer pointer = new IntByReference(pwmGenerator).getPointer();
        HALLibrary.setPWMDutyCycleWithModuleIntHack((byte) m_module, pointer,
                                                    Float.floatToIntBits((float) dutyCycle), status);
        HALUtil.checkStatus(status);
    }

    /**
     * Return an I2C object for this digital module
     *
     * @param address
     *            The device address.
     * @return The associated I2C object.
     */
    public I2C getI2C(final int address) {
        return new I2C(this, address);
    }

    /**
     * Get the loop timing of the Digital Module
     *
     * @return The number of clock ticks per DIO loop
     */
    public int getLoopTiming() {
        IntBuffer status = IntBuffer.allocate(1);
        int value = HALLibrary.getLoopTiming(status);
        HALUtil.checkStatus(status);
        return value;
    }
}
