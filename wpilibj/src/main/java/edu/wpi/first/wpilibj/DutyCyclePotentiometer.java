// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDevice.Direction;
import edu.wpi.first.hal.SimDouble;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableBuilder;
import edu.wpi.first.util.sendable.SendableRegistry;

/**
 * Class for reading duty cyle potentiometers. Duty cycle potentiometers read in
 * an pwm signal that
 * corresponds to a position. The position is in whichever units you choose, by
 * way of the scaling
 * and offset constants passed to the constructor.
 */
public class DutyCyclePotentiometer implements Sendable, AutoCloseable {
    private DutyCycle m_dutyCycle;
    private DigitalSource m_digitalSource;
    private boolean m_ownsDutyCycle;
    private double m_fullRange;
    private double m_offset;
    private int m_frequencyThreshold = 100;
    private double m_periodNanos;

    private SimDevice m_simDevice;
    private SimDouble m_simPosition;
    private SimBoolean m_simIsConnected;

    private final void init(double fullRange, double offset) {
        SendableRegistry.addLW(this, "DutyCyclePotentiometer", m_dutyCycle.getFPGAIndex());
        m_fullRange = fullRange;
        m_offset = offset;

        m_simDevice = SimDevice.create("DutyCyclePotentiometer", m_dutyCycle.getFPGAIndex());

        if (m_simDevice != null) {
            m_simPosition = m_simDevice.createDouble("Position", Direction.kInput, 0.0);
            m_simIsConnected = m_simDevice.createBoolean("Connected", SimDevice.Direction.kInput, true);
        }
    }

    /**
     * AnalogPotentiometer constructor.
     *
     * <p>
     * Use the fullRange and offset values so that the output produces meaningful
     * values. I.E: you
     * have a 270 degree potentiometer, and you want the output to be degrees with
     * the halfway point
     * as 0 degrees. The fullRange value is 270.0(degrees) and the offset is -135.0
     * since the halfway
     * point after scaling is 135 degrees. This will calculate the result from the
     * fullRange times the
     * fraction of the supply voltage, plus the offset.
     *
     * @param channel   The analog input channel this potentiometer is plugged into.
     *                  0-3 are on-board
     *                  and 4-7 are on the MXP port.
     * @param fullRange The scaling to multiply the fraction by to get a meaningful
     *                  unit.
     * @param offset    The offset to add to the scaled value for controlling the
     *                  zero value
     */
    @SuppressWarnings("this-escape")
    public DutyCyclePotentiometer(final int channel, double fullRange, double offset) {
        m_digitalSource = new DigitalInput(channel);
        m_dutyCycle = new DutyCycle(m_digitalSource);
        m_ownsDutyCycle = true;
        init(fullRange, offset);
        SendableRegistry.addChild(this, m_dutyCycle);
    }

    /**
     * AnalogPotentiometer constructor.
     *
     * <p>
     * Use the fullRange and offset values so that the output produces meaningful
     * values. I.E: you
     * have a 270 degree potentiometer, and you want the output to be degrees with
     * the halfway point
     * as 0 degrees. The fullRange value is 270.0(degrees) and the offset is -135.0
     * since the halfway
     * point after scaling is 135 degrees. This will calculate the result from the
     * fullRange times the
     * fraction of the supply voltage, plus the offset.
     *
     * @param input     The {@link DutyCycle} this potentiometer is plugged into.
     * @param fullRange The angular value (in desired units) representing the full
     *                  0-5V range of the
     *                  input.
     * @param offset    The angular value (in desired units) representing the
     *                  angular output at 0V.
     */
    @SuppressWarnings("this-escape")
    public DutyCyclePotentiometer(final DigitalSource input, double fullRange, double offset) {
        m_dutyCycle = new DutyCycle(requireNonNullParam(input, "input", "DutyCyclePotentiometer"));
        m_ownsDutyCycle = true;
        init(fullRange, offset);
    }

    /**
     * AnalogPotentiometer constructor.
     *
     * <p>
     * Use the fullRange and offset values so that the output produces meaningful
     * values. I.E: you
     * have a 270 degree potentiometer, and you want the output to be degrees with
     * the halfway point
     * as 0 degrees. The fullRange value is 270.0(degrees) and the offset is -135.0
     * since the halfway
     * point after scaling is 135 degrees. This will calculate the result from the
     * fullRange times the
     * fraction of the supply voltage, plus the offset.
     *
     * @param input     The {@link DutyCycle} this potentiometer is plugged into.
     * @param fullRange The angular value (in desired units) representing the full
     *                  0-5V range of the
     *                  input.
     * @param offset    The angular value (in desired units) representing the
     *                  angular output at 0V.
     */
    @SuppressWarnings("this-escape")
    public DutyCyclePotentiometer(final DutyCycle input, double fullRange, double offset) {
        m_dutyCycle = requireNonNullParam(input, "input", "DutyCyclePotentiometer");
        init(fullRange, offset);
    }

    /**
     * AnalogPotentiometer constructor.
     *
     * <p>
     * Use the scale value so that the output produces meaningful values. I.E: you
     * have a 270
     * degree potentiometer, and you want the output to be degrees with the starting
     * point as 0
     * degrees. The scale value is 270.0(degrees).
     *
     * @param channel The analog input channel this potentiometer is plugged into.
     *                0-3 are on-board
     *                and 4-7 are on the MXP port.
     * @param scale   The scaling to multiply the voltage by to get a meaningful
     *                unit.
     */
    public DutyCyclePotentiometer(final int channel, double scale) {
        this(channel, scale, 0);
    }

    /**
     * AnalogPotentiometer constructor.
     *
     * <p>
     * Use the fullRange and offset values so that the output produces meaningful
     * values. I.E: you
     * have a 270 degree potentiometer, and you want the output to be degrees with
     * the starting point
     * as 0 degrees. The scale value is 270.0(degrees).
     *
     * @param input The {@link DutyCycle} this potentiometer is plugged into.
     * @param scale The scaling to multiply the voltage by to get a meaningful unit.
     */
    public DutyCyclePotentiometer(final DigitalSource input, double scale) {
        this(input, scale, 0);
    }

    /**
     * AnalogPotentiometer constructor.
     *
     * <p>
     * Use the fullRange and offset values so that the output produces meaningful
     * values. I.E: you
     * have a 270 degree potentiometer, and you want the output to be degrees with
     * the starting point
     * as 0 degrees. The scale value is 270.0(degrees).
     *
     * @param input The {@link DutyCycle} this potentiometer is plugged into.
     * @param scale The scaling to multiply the voltage by to get a meaningful unit.
     */
    public DutyCyclePotentiometer(final DutyCycle input, double scale) {
        this(input, scale, 0);
    }

    /**
     * AnalogPotentiometer constructor.
     *
     * <p>
     * The potentiometer will return a value between 0 and 1.0.
     *
     * @param channel The analog input channel this potentiometer is plugged into.
     *                0-3 are on-board
     *                and 4-7 are on the MXP port.
     */
    public DutyCyclePotentiometer(final int channel) {
        this(channel, 1, 0);
    }

    /**
     * AnalogPotentiometer constructor.
     *
     * <p>
     * The potentiometer will return a value between 0 and 1.0.
     *
     * @param input The {@link DutyCycle} this potentiometer is plugged into.
     */
    public DutyCyclePotentiometer(final DigitalSource input) {
        this(input, 1, 0);
    }

    /**
     * AnalogPotentiometer constructor.
     *
     * <p>
     * The potentiometer will return a value between 0 and 1.0.
     *
     * @param input The {@link DutyCycle} this potentiometer is plugged into.
     */
    public DutyCyclePotentiometer(final DutyCycle input) {
        this(input, 1, 0);
    }

    /**
     * Get the current reading of the potentiometer.
     *
     * @return The current position of the potentiometer.
     */
    public double get() {
        if (m_simPosition != null) {
            return m_simPosition.get();
        }
        double pos;
        if (m_periodNanos == 0.0) {
            pos = m_dutyCycle.getOutput();
        } else {
            int highTime = m_dutyCycle.getHighTimeNanoseconds();
            pos = highTime / m_periodNanos;
        }

        return pos * m_fullRange + m_offset;
    }

    /**
     * Get the frequency in Hz of the duty cycle signal from the encoder.
     *
     * @return duty cycle frequency in Hz
     */
    public int getFrequency() {
        return m_dutyCycle.getFrequency();
    }

    /**
     * Get if the sensor is connected
     *
     * <p>
     * This uses the duty cycle frequency to determine if the sensor is connected.
     * By default, a
     * value of 100 Hz is used as the threshold, and this value can be changed with
     * {@link
     * #setConnectedFrequencyThreshold(int)}.
     *
     * @return true if the sensor is connected
     */
    public boolean isConnected() {
        if (m_simIsConnected != null) {
            return m_simIsConnected.get();
        }
        return getFrequency() > m_frequencyThreshold;
    }

    /**
     * Change the frequency threshold for detecting connection used by
     * {@link #isConnected()}.
     *
     * @param frequency the minimum frequency in Hz.
     */
    public void setConnectedFrequencyThreshold(int frequency) {
        if (frequency < 0) {
            frequency = 0;
        }

        m_frequencyThreshold = frequency;
    }

    /**
     * Sets the assumed frequency of the connected device.
     *
     * <p>
     * By default, the DutyCycle engine has to compute the frequency of the input
     * signal. This can result in both delayed readings and jumpy readings. To solve
     * this, you can pass the expected frequency of the sensor to this function.
     * This will use that frequency to compute the DutyCycle percentage, rather than
     * the computed frequency.
     *
     * @param frequency
     */
    public void setAssumedFrequency(double frequency) {
        if (frequency == 0.0) {
            m_periodNanos = 0.0;
        } else {
            m_periodNanos = 1000000000 / frequency;
        }
    }

    @Override
    public void initSendable(SendableBuilder builder) {
        if (m_dutyCycle != null) {
            builder.setSmartDashboardType("Analog Input");
            builder.addDoubleProperty("Value", this::get, null);
        }
    }

    @Override
    public void close() {
        SendableRegistry.remove(this);
        if (m_ownsDutyCycle) {
            m_dutyCycle.close();
            m_dutyCycle = null;
            m_ownsDutyCycle = false;
        }
        if (m_digitalSource != null) {
            m_digitalSource.close();
        }
    }
}
