/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2012. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/
package edu.wpi.first.wpilibj;

import java.nio.ByteOrder;
import java.nio.IntBuffer;
import java.nio.ByteBuffer;

//import com.sun.jna.Pointer;
//import com.sun.jna.ptr.IntByReference;


import edu.wpi.first.wpilibj.communication.FRCNetworkCommunicationsLibrary.tModuleType;
import edu.wpi.first.wpilibj.hal.DIOJNI;
import edu.wpi.first.wpilibj.hal.PWMJNI;
import edu.wpi.first.wpilibj.hal.RelayJNI;
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
    private ByteBuffer[] m_digital_ports;
    private ByteBuffer[] m_relay_ports;
    private ByteBuffer[] m_pwm_ports;

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
     * Create a new digital module
     *
     * @param moduleNumber
     *            The number of the digital module to use (1 or 2)
     */
    protected DigitalModule(final int moduleNumber) {
        super(tModuleType.kModuleType_Digital, moduleNumber);
        m_module = moduleNumber;

        m_digital_ports = new ByteBuffer[SensorBase.kDigitalChannels];
        for (int i = 0; i < SensorBase.kDigitalChannels; i++) {
            ByteBuffer port_pointer = DIOJNI.getPortWithModule(
                                       (byte) moduleNumber, (byte) i);
            ByteBuffer status = ByteBuffer.allocateDirect(4);
    		// set the byte order
    		status.order(ByteOrder.LITTLE_ENDIAN);
            m_digital_ports[i] = DIOJNI.initializeDigitalPort(port_pointer,
                               status.asIntBuffer());
            HALUtil.checkStatus(status.asIntBuffer());
        }

        m_relay_ports = new ByteBuffer[SensorBase.kRelayChannels];
        for (int i = 0; i < SensorBase.kRelayChannels; i++) {
            ByteBuffer port_pointer = RelayJNI.getPortWithModule(
                                       (byte) moduleNumber, (byte) i);
            ByteBuffer status = ByteBuffer.allocateDirect(4);
    		// set the byte order
    		status.order(ByteOrder.LITTLE_ENDIAN);
            m_relay_ports[i] = RelayJNI.initializeDigitalPort(port_pointer,
                               status.asIntBuffer());
            HALUtil.checkStatus(status.asIntBuffer());
        }
        m_pwm_ports = new ByteBuffer[SensorBase.kPwmChannels];
        for (int i = 0; i < SensorBase.kPwmChannels; i++) {
            ByteBuffer port_pointer = PWMJNI.getPortWithModule(
                                       (byte) moduleNumber, (byte) i);
            ByteBuffer status = ByteBuffer.allocateDirect(4);
    		// set the byte order
    		status.order(ByteOrder.LITTLE_ENDIAN);
            m_pwm_ports[i] = PWMJNI.initializeDigitalPort(port_pointer,
                               status.asIntBuffer());
            HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        PWMJNI.setPWM(m_pwm_ports[channel], (short) value, status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
    }

    /**
     * Get a value from a PWM channel. The values range from 0 to 255.
     *
     * @param channel
     *            The PWM channel to read from.
     * @return The raw PWM value.
     */
    public int getPWM(final int channel) {
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        int value = (int) PWMJNI.getPWM(m_pwm_ports[channel], status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        PWMJNI.setPWMPeriodScale(m_pwm_ports[channel], squelchMask,
                                     status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        RelayJNI.setRelayForward(m_relay_ports[channel], (byte) (on ? 1
                                   : 0), status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        RelayJNI.setRelayReverse(m_relay_ports[channel], (byte) (on ? 1
                                   : 0), status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
    }

    /**
     * Get the current state of the forward relay channel
     *
     * @param channel
     *            the channel of the relay to get
     * @return The current state of the relay.
     */
    public boolean getRelayForward(int channel) {
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        boolean value = RelayJNI.getRelayForward(m_relay_ports[channel],
                        status.asIntBuffer()) != 0;
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        for (int i = 0; i < SensorBase.kRelayChannels; i++) {
            value |= RelayJNI.getRelayForward(m_relay_ports[i], status.asIntBuffer()) << i;
        }
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        boolean value = RelayJNI.getRelayReverse(m_relay_ports[channel],
                        status.asIntBuffer()) != 0;
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        for (int i = 0; i < SensorBase.kRelayChannels; i++) {
            value |= RelayJNI.getRelayReverse(m_relay_ports[i], status.asIntBuffer()) << i;
        }
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        boolean allocated = DIOJNI.allocateDIO(
                                m_digital_ports[channel], (byte) (input ? 1 : 0), status.asIntBuffer()) != 0;
        HALUtil.checkStatus(status.asIntBuffer());
        return allocated;
    }

    /**
     * Free the resource associated with a digital I/O channel.
     *
     * @param channel
     *            The channel whose resources should be freed.
     */
    public void freeDIO(final int channel) {
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        DIOJNI.freeDIO(m_digital_ports[channel], status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        DIOJNI.setDIO(m_digital_ports[channel], (byte) (value ? 1 : 0),
                          status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        boolean value = DIOJNI.getDIO(m_digital_ports[channel], status.asIntBuffer()) != 0;
        HALUtil.checkStatus(status.asIntBuffer());
        return value;
    }

    /**
     * Read the state of all the Digital I/O lines from the FPGA These are not
     * remapped to logical order. They are still in hardware order.
     *
     * @return The state of all the Digital IO lines in hardware order
     */
    public short getAllDIO() {
        short value = 0;
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        for (int i = 0; i < SensorBase.kDigitalChannels; i++) {
            value |= DIOJNI.getDIO(m_digital_ports[i], status.asIntBuffer()) << i;
        }
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        boolean value = DIOJNI.getDIODirection(
                            m_digital_ports[channel], status.asIntBuffer()) != 0;
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        for (int i = 0; i < SensorBase.kDigitalChannels; i++) {
            value |= DIOJNI.getDIODirection(m_digital_ports[i], status.asIntBuffer()) << i;
        }
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        DIOJNI.pulse(m_digital_ports[channel], pulseLength, status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        float convertedPulse = (float) (pulseLength / 1.0e9 * (DIOJNI.getLoopTiming(status.asIntBuffer()) * 25));
        System.err
        .println("You should use the float version of pulse for portability.  This is deprecated");
        DIOJNI.pulse(m_digital_ports[channel], convertedPulse, status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
    }

    /**
     * Check a DIO line to see if it is currently generating a pulse.
     *
     * @param channel
     *            The channel to check.
     * @return True if the channel is pulsing, false otherwise.
     */
    public boolean isPulsing(final int channel) {
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        boolean value = DIOJNI.isPulsing(m_digital_ports[channel],
                                             status.asIntBuffer()) != 0;
        HALUtil.checkStatus(status.asIntBuffer());
        return value;
    }

    /**
     * Check if any DIO line is currently generating a pulse.
     *
     * @return True if any channel is pulsing, false otherwise.
     */
    public boolean isPulsing() {
        boolean value;
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        value = DIOJNI.isAnyPulsingWithModule((byte) m_module, status.asIntBuffer()) != 0;
        HALUtil.checkStatus(status.asIntBuffer());
        return value;
    }

    /**
     * Allocate a DO PWM Generator. Allocate PWM generators so that they are not
     * accidently reused.
     */
    public ByteBuffer allocateDO_PWM() {
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        ByteBuffer value = PWMJNI.allocatePWMWithModule((byte) m_module, status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
        return value;
    }

    /**
     * Free the resource associated with a DO PWM generator.
     */
    public void freeDO_PWM(ByteBuffer pwmGenerator) {
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        PWMJNI.freePWMWithModule((byte) m_module, pwmGenerator, status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        PWMJNI.setPWMRateWithModule((byte) m_module, rate, status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
    }

    /**
     * Configure which DO channel the PWM siganl is output on
     *
     * @param pwmGenerator
     *            The generator index reserved by allocateDO_PWM()
     * @param channel
     *            The Digital Output channel to output on
     */
    public void setDO_PWMOutputChannel(ByteBuffer pwmGenerator, int channel) {
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        PWMJNI.setPWMOutputChannelWithModule((byte) m_module, pwmGenerator,
                channel, status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
    }

    /**
     * Configure the duty-cycle of the PWM generator
     *
     * @param pwmGenerator
     *            The generator index reserved by allocateDO_PWM()
     * @param dutyCycle
     *            The percent duty cycle to output [0..1].
     */
    public void setDO_PWMDutyCycle(ByteBuffer pwmGenerator, double dutyCycle) {
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        PWMJNI.setPWMDutyCycleWithModule((byte) m_module, pwmGenerator, dutyCycle, status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
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
        ByteBuffer status = ByteBuffer.allocateDirect(4);
		// set the byte order
		status.order(ByteOrder.LITTLE_ENDIAN);
        int value = DIOJNI.getLoopTiming(status.asIntBuffer());
        HALUtil.checkStatus(status.asIntBuffer());
        return value;
    }
}
